/**
 * MedBox — Dispense Log API (Netlify Function)
 * Records and retrieves medication dispensing events.
 * Persistent storage via Netlify Blobs.
 */

const { getStore, connectLambda } = require('@netlify/blobs');

const STORE_NAME = 'dispense-logs';

exports.handler = async (event, context) => {
  connectLambda(event);

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
    const store = getStore(STORE_NAME);

    // ── GET: Retrieve logs ────────────────────────────────────────
    if (event.httpMethod === 'GET') {
      const limit = parseInt(event.queryStringParameters?.limit || '50');
      const index = await store.get('_index', { type: 'json' }) || [];

      // Get the most recent N logs
      const recentIds = index.slice(-limit).reverse();
      const logs = [];
      for (const id of recentIds) {
        const log = await store.get(`log_${id}`, { type: 'json' });
        if (log) logs.push(log);
      }

      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(logs),
      };
    }

    // ── POST: Record a dispense event (called by ESP32 or backend) ─
    if (event.httpMethod === 'POST') {
      const data = JSON.parse(event.body);

      // Get next ID
      const counter = await store.get('_counter');
      const nextId = counter ? parseInt(counter) + 1 : 1;
      await store.set('_counter', String(nextId));

      const log = {
        id: String(nextId),
        medicationId: data.medicationId || null,
        medicineName: data.medicineName || 'Unknown',
        moduleId: data.moduleId,
        scheduledTime: data.scheduledTime || null,
        actualTime: data.actualTime || new Date().toISOString(),
        requestedQuantity: data.requestedQuantity || 1,
        status: data.status || 'dispensed',
        error: data.error || null,
        deviceId: data.deviceId || 'unknown',
      };

      await store.set(`log_${log.id}`, JSON.stringify(log));

      // Add to index
      const index = await store.get('_index', { type: 'json' }) || [];
      index.push(log.id);
      // Keep only last 500 log IDs in the index to prevent unbounded growth
      const trimmedIndex = index.slice(-500);
      await store.set('_index', JSON.stringify(trimmedIndex));

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
