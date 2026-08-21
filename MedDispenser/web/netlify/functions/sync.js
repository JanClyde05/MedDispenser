/**
 * MedBox — Device Sync API (Netlify Function)
 * Endpoint for ESP32-S3 to fetch its schedule payload.
 *
 * The ESP32 calls this periodically to sync its local schedule.
 * Response format is optimized for the firmware's MedSchedule struct.
 */

const { getStore, connectLambda } = require('@netlify/blobs');

exports.handler = async (event, context) => {
  connectLambda(event);

  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
  };

  if (event.httpMethod !== 'GET') {
    return { statusCode: 405, headers, body: JSON.stringify({ error: 'Method not allowed' }) };
  }

  try {
    const medStore = getStore('medications');
    const index = await medStore.get('_index', { type: 'json' }) || [];

    // Also update device heartbeat
    const deviceId = event.queryStringParameters?.deviceId || 'unknown';
    const devStore = getStore('devices');

    if (deviceId !== 'unknown') {
      const deviceInfo = await devStore.get(`dev_${deviceId}`, { type: 'json' }) || {};
      await devStore.set(`dev_${deviceId}`, JSON.stringify({
        ...deviceInfo,
        deviceId,
        lastSeen: new Date().toISOString(),
        lastSyncEpoch: Math.floor(Date.now() / 1000),
        status: 'online',
      }));
    }

    // Build schedule array for firmware
    const schedules = [];
    for (const id of index) {
      const med = await medStore.get(`med_${id}`, { type: 'json' });
      if (med && med.enabled) {
        // Parse time string "HH:MM" into hour/minute integers
        const [hour, minute] = (med.time || '00:00').split(':').map(Number);

        schedules.push({
          moduleId: med.moduleId || 1,
          medicineName: (med.name || 'Unknown').substring(0, 31),
          pillsPerDose: med.pillsPerDose || 1,
          hour,
          minute,
          daysOfWeek: med.daysOfWeek || 0,
          startDate: med.startDate ? Math.floor(new Date(med.startDate).getTime() / 1000) : 0,
          endDate: med.endDate ? Math.floor(new Date(med.endDate).getTime() / 1000) : 0,
          enabled: true,
        });
      }
    }

    // Fetch and clear pending dispense commands
    const cmdStore = getStore('dispense-commands');
    let pendingCommands = [];
    try {
      pendingCommands = await cmdStore.get('_pending', { type: 'json' }) || [];
      if (pendingCommands.length > 0) {
        // Clear after fetching — commands are delivered once
        await cmdStore.set('_pending', JSON.stringify([]));
      }
    } catch (cmdErr) {
      // Non-fatal — commands are optional
      console.warn('Failed to fetch pending commands:', cmdErr.message);
    }

    return {
      statusCode: 200,
      headers,
      body: JSON.stringify({
        serverTime: Math.floor(Date.now() / 1000),
        schedules,
        pendingCommands,
      }),
    };

  } catch (err) {
    return {
      statusCode: 500,
      headers,
      body: JSON.stringify({ error: err.message }),
    };
  }
};
