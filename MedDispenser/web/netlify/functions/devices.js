/**
 * MedBox — Device Registration & Heartbeat API (Netlify Function)
 * Tracks active MedBox devices and their modules.
 *
 * GET  — List all registered devices
 * POST — Register/update a device heartbeat
 */

const { getStore, connectLambda } = require('@netlify/blobs');

const STORE_NAME = 'devices';

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

    // ── GET: List all devices ─────────────────────────────────────
    if (event.httpMethod === 'GET') {
      const index = await store.get('_index', { type: 'json' }) || [];
      const devices = [];
      const now = Date.now();

      for (const id of index) {
        const dev = await store.get(`dev_${id}`, { type: 'json' });
        if (dev) {
          // Mark as offline if not seen in 10 minutes
          const lastSeen = new Date(dev.lastSeen).getTime();
          const isOnline = (now - lastSeen) < 600000; // 10 min
          devices.push({
            ...dev,
            status: isOnline ? 'online' : 'offline',
          });
        }
      }

      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(devices),
      };
    }

    // ── POST: Register / heartbeat ────────────────────────────────
    if (event.httpMethod === 'POST') {
      const data = JSON.parse(event.body);
      const deviceId = data.deviceId;

      if (!deviceId) {
        return { statusCode: 400, headers, body: JSON.stringify({ error: 'deviceId required' }) };
      }

      const existing = await store.get(`dev_${deviceId}`, { type: 'json' }) || {};

      const device = {
        deviceId,
        name: data.name || existing.name || deviceId,
        isMain: data.isMain !== undefined ? data.isMain : (existing.isMain !== undefined ? existing.isMain : true),
        moduleCount: data.moduleCount || existing.moduleCount || 1,
        activeModules: data.activeModules || existing.activeModules || [1],
        firmware: data.firmware || existing.firmware || 'unknown',
        ip: data.ip || existing.ip || null,
        lastSeen: new Date().toISOString(),
        status: 'online',
        registeredAt: existing.registeredAt || new Date().toISOString(),
      };

      await store.set(`dev_${deviceId}`, JSON.stringify(device));

      // Update index
      const index = await store.get('_index', { type: 'json' }) || [];
      if (!index.includes(deviceId)) {
        index.push(deviceId);
        await store.set('_index', JSON.stringify(index));
      }

      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(device),
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
