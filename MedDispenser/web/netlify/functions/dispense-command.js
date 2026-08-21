/**
 * MedBox — Dispense Command API (Netlify Function)
 * Queues dispense commands for the ESP32 to pick up during its sync cycle.
 *
 * POST — Queue a new dispense command (from web frontend test/schedule trigger)
 * DELETE — Clear acknowledged commands (called by ESP32 after processing)
 *
 * Commands are stored in the 'dispense-commands' Netlify Blob store.
 */

const { getStore, connectLambda } = require('@netlify/blobs');

exports.handler = async (event, context) => {
  connectLambda(event);

  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'GET, POST, DELETE, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type',
  };

  if (event.httpMethod === 'OPTIONS') {
    return { statusCode: 204, headers, body: '' };
  }

  const store = getStore('dispense-commands');

  // ── POST: Queue a new dispense command ─────────────────────────────
  if (event.httpMethod === 'POST') {
    try {
      const body = JSON.parse(event.body);

      const command = {
        id: `cmd_${Date.now()}_${Math.random().toString(36).substr(2, 6)}`,
        moduleId: body.moduleId || 1,
        medicineName: body.medicineName || 'Unknown',
        dose: body.dose || 1,
        time: body.time || 'now',
        type: body.type || 'test',           // 'test', 'scheduled', 'manual', 'test_hardware'
        createdAt: new Date().toISOString(),
        acknowledged: false,
        // Pass through extra fields (command, irAction) for hardware test commands
        ...(body.command && { command: body.command }),
        ...(body.irAction && { irAction: body.irAction }),
      };

      // Append to pending commands list
      const pending = await store.get('_pending', { type: 'json' }) || [];
      pending.push(command);
      await store.set('_pending', JSON.stringify(pending));

      return {
        statusCode: 200,
        headers,
        body: JSON.stringify({ success: true, command }),
      };

    } catch (err) {
      return {
        statusCode: 500,
        headers,
        body: JSON.stringify({ error: err.message }),
      };
    }
  }

  // ── GET: Retrieve pending commands ─────────────────────────────────
  if (event.httpMethod === 'GET') {
    try {
      const pending = await store.get('_pending', { type: 'json' }) || [];
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(pending),
      };
    } catch (err) {
      return {
        statusCode: 500,
        headers,
        body: JSON.stringify({ error: err.message }),
      };
    }
  }

  // ── DELETE: Clear acknowledged commands ─────────────────────────────
  if (event.httpMethod === 'DELETE') {
    try {
      const body = event.body ? JSON.parse(event.body) : {};

      if (body.clearAll) {
        // Clear everything
        await store.set('_pending', JSON.stringify([]));
      } else if (body.ids && Array.isArray(body.ids)) {
        // Clear specific command IDs
        const pending = await store.get('_pending', { type: 'json' }) || [];
        const remaining = pending.filter(cmd => !body.ids.includes(cmd.id));
        await store.set('_pending', JSON.stringify(remaining));
      }

      return {
        statusCode: 200,
        headers,
        body: JSON.stringify({ success: true }),
      };
    } catch (err) {
      return {
        statusCode: 500,
        headers,
        body: JSON.stringify({ error: err.message }),
      };
    }
  }

  return {
    statusCode: 405,
    headers,
    body: JSON.stringify({ error: 'Method not allowed' }),
  };
};
