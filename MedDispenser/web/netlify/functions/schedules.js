/**
 * MedBox — Schedules API (Netlify Function)
 * Returns schedule data derived from medications.
 * Reads from the medications Netlify Blob store.
 */

const { getStore, connectLambda } = require('@netlify/blobs');

exports.handler = async (event, context) => {
  connectLambda(event);

  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
  };

  if (event.httpMethod === 'OPTIONS') {
    return { statusCode: 204, headers, body: '' };
  }

  if (event.httpMethod !== 'GET') {
    return { statusCode: 405, headers, body: JSON.stringify({ error: 'Method not allowed' }) };
  }

  try {
    const store = getStore('medications');
    const index = await store.get('_index', { type: 'json' }) || [];

    const schedules = [];
    for (const id of index) {
      const med = await store.get(`med_${id}`, { type: 'json' });
      if (med && med.enabled) {
        schedules.push({
          medicationId: med.id,
          medicineName: med.name,
          moduleId: med.moduleId,
          pillsPerDose: med.pillsPerDose,
          time: med.time,
          daysOfWeek: med.daysOfWeek,
          startDate: med.startDate,
          endDate: med.endDate,
          status: 'pending',
        });
      }
    }

    // If ?today=true, filter to only schedules active today
    const today = event.queryStringParameters?.today === 'true';
    if (today) {
      const dow = new Date().getDay(); // 0=Sun
      const filtered = schedules.filter(s => (s.daysOfWeek >> dow) & 1);
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(filtered),
      };
    }

    return {
      statusCode: 200,
      headers,
      body: JSON.stringify(schedules),
    };

  } catch (err) {
    return {
      statusCode: 500,
      headers,
      body: JSON.stringify({ error: err.message }),
    };
  }
};
