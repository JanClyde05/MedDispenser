/**
 * MedBox — Device Sync API (Netlify Function)
 * Endpoint for ESP32-S3 to fetch its schedule payload.
 *
 * The ESP32 calls this periodically to sync its local schedule.
 * Response format is optimized for the firmware's MedSchedule struct.
 */

exports.handler = async (event, context) => {
  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
  };

  if (event.httpMethod !== 'GET') {
    return { statusCode: 405, headers, body: JSON.stringify({ error: 'Method not allowed' }) };
  }

  try {
    // TODO: Query database for all active, enabled medications
    // and format them for the ESP32 firmware.
    //
    // The firmware expects an array of schedule objects matching:
    // {
    //   moduleId: uint8,
    //   medicineName: string (max 31 chars),
    //   pillsPerDose: uint8,
    //   hour: uint8 (0-23),
    //   minute: uint8 (0-59),
    //   daysOfWeek: uint8 (bitmask, bit0=Sun),
    //   startDate: uint32 (epoch) or 0,
    //   endDate: uint32 (epoch) or 0,
    //   enabled: bool
    // }

    const stubPayload = {
      serverTime: Math.floor(Date.now() / 1000),
      schedules: [
        {
          moduleId: 1,
          medicineName: 'Metformin',
          pillsPerDose: 1,
          hour: 7,
          minute: 0,
          daysOfWeek: 0b0111110,
          startDate: 0,
          endDate: 0,
          enabled: true,
        },
      ],
    };

    return {
      statusCode: 200,
      headers,
      body: JSON.stringify(stubPayload),
    };

  } catch (err) {
    return {
      statusCode: 500,
      headers,
      body: JSON.stringify({ error: err.message }),
    };
  }
};
