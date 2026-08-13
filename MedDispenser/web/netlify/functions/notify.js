/**
 * MedBox — ntfy Notification Publisher (Netlify Function)
 * Server-side ntfy publishing to keep credentials out of frontend JS.
 *
 * IMPORTANT: ntfy topic and auth credentials must be stored as
 * Netlify environment variables, NOT in this source file.
 *
 * Environment variables:
 *   NTFY_TOPIC    — the ntfy topic name (e.g., "medbox-user123")
 *   NTFY_SERVER   — ntfy server URL (default: https://ntfy.sh)
 *   NTFY_TOKEN    — optional auth token for private topics
 */

exports.handler = async (event, context) => {
  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'POST, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type',
  };

  if (event.httpMethod === 'OPTIONS') {
    return { statusCode: 204, headers, body: '' };
  }

  if (event.httpMethod !== 'POST') {
    return { statusCode: 405, headers, body: JSON.stringify({ error: 'Method not allowed' }) };
  }

  try {
    const { medicineName, dose, time, moduleId, type } = JSON.parse(event.body);

    const ntfyServer = process.env.NTFY_SERVER || 'https://ntfy.sh';
    const ntfyTopic  = process.env.NTFY_TOPIC;
    const ntfyToken  = process.env.NTFY_TOKEN;

    if (!ntfyTopic) {
      return {
        statusCode: 500,
        headers,
        body: JSON.stringify({ error: 'NTFY_TOPIC not configured' }),
      };
    }

    // Build notification
    const title = type === 'missed'
      ? '⚠️ Missed Medication'
      : '💊 Medication Reminder';

    const message = [
      `Medicine: ${medicineName || 'Unknown'}`,
      `Dose: ${dose || 1} tablet(s)`,
      `Time: ${time || 'now'}`,
      `Module: ${moduleId || '?'}`,
      '',
      type === 'missed'
        ? 'This medication was not taken.'
        : 'Please take your scheduled medication.',
    ].join('\n');

    // Publish to ntfy
    const ntfyHeaders = { 'Title': title };
    if (ntfyToken) {
      ntfyHeaders['Authorization'] = `Bearer ${ntfyToken}`;
    }

    const res = await fetch(`${ntfyServer}/${ntfyTopic}`, {
      method: 'POST',
      headers: ntfyHeaders,
      body: message,
    });

    if (!res.ok) {
      throw new Error(`ntfy publish failed: ${res.status}`);
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
};
