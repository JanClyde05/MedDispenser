/**
 * MedBox — Schedules API (Netlify Function)
 * Returns schedule data derived from medications.
 * Schedules are embedded in medication records (time + daysOfWeek).
 */

exports.handler = async (event, context) => {
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
    // TODO: Query database for medications and build schedule view
    // For now, return a stub response
    const today = event.queryStringParameters?.today === 'true';

    const stubSchedules = [
      {
        medicineName: 'Metformin',
        moduleId: 1,
        pillsPerDose: 1,
        time: '07:00',
        daysOfWeek: 0b0111110, // Mon-Fri
        status: 'pending',
      },
      {
        medicineName: 'Amlodipine',
        moduleId: 2,
        pillsPerDose: 1,
        time: '12:00',
        daysOfWeek: 0b1111111, // Every day
        status: 'pending',
      },
    ];

    return {
      statusCode: 200,
      headers,
      body: JSON.stringify(stubSchedules),
    };

  } catch (err) {
    return {
      statusCode: 500,
      headers,
      body: JSON.stringify({ error: err.message }),
    };
  }
};
