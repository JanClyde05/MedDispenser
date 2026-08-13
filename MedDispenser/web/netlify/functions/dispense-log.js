/**
 * MedBox — Dispense Log API (Netlify Function)
 * Records and retrieves medication dispensing events.
 */

let dispenseLogs = [];
let logNextId = 1;

exports.handler = async (event, context) => {
  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type',
  };

  if (event.httpMethod === 'OPTIONS') {
    return { statusCode: 204, headers, body: '' };
  }

  try {
    // ── GET: Retrieve logs ────────────────────────────────────────
    if (event.httpMethod === 'GET') {
      const limit = parseInt(event.queryStringParameters?.limit || '50');
      const result = dispenseLogs.slice(-limit).reverse();
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(result),
      };
    }

    // ── POST: Record a dispense event (called by ESP32 or backend) ─
    if (event.httpMethod === 'POST') {
      const data = JSON.parse(event.body);
      const log = {
        id: String(logNextId++),
        medicationId: data.medicationId,
        medicineName: data.medicineName || 'Unknown',
        moduleId: data.moduleId,
        scheduledTime: data.scheduledTime,
        actualTime: data.actualTime || new Date().toISOString(),
        requestedQuantity: data.requestedQuantity || 1,
        status: data.status || 'dispensed',
        error: data.error || null,
      };
      dispenseLogs.push(log);
      return {
        statusCode: 201,
        headers,
        body: JSON.stringify(log),
      };
    }

    return { statusCode: 405, headers, body: JSON.stringify({ error: 'Method not allowed' }) };

  } catch (err) {
    return {
      statusCode: 500,
      headers,
      body: JSON.stringify({ error: err.message }),
    };
  }
};
