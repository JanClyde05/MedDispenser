/**
 * MedBox — Medications API (Netlify Function)
 * CRUD operations for medication records.
 *
 * Database: Stub — replace with actual DB client (Supabase, MongoDB, etc.)
 * when database choice is finalized.
 */

// ── Stub Data Store (replace with real DB) ────────────────────────────
let medications = [];
let nextId = 1;

exports.handler = async (event, context) => {
  const { httpMethod, body, path } = event;
  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type',
  };

  // CORS preflight
  if (httpMethod === 'OPTIONS') {
    return { statusCode: 204, headers, body: '' };
  }

  try {
    // ── GET: List all medications ──────────────────────────────────
    if (httpMethod === 'GET') {
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(medications),
      };
    }

    // ── POST: Create medication ───────────────────────────────────
    if (httpMethod === 'POST') {
      const data = JSON.parse(body);
      const med = {
        id: String(nextId++),
        name: data.name,
        moduleId: data.moduleId || 1,
        pillsPerDose: data.pillsPerDose || 1,
        time: data.time,
        daysOfWeek: data.daysOfWeek || 0,
        startDate: data.startDate || null,
        endDate: data.endDate || null,
        enabled: data.enabled !== false,
        createdAt: new Date().toISOString(),
      };
      medications.push(med);
      return {
        statusCode: 201,
        headers,
        body: JSON.stringify(med),
      };
    }

    // ── PUT: Update medication ────────────────────────────────────
    if (httpMethod === 'PUT') {
      const segments = path.split('/');
      const id = segments[segments.length - 1];
      const data = JSON.parse(body);
      const idx = medications.findIndex(m => m.id === id);
      if (idx === -1) {
        return { statusCode: 404, headers, body: JSON.stringify({ error: 'Not found' }) };
      }
      medications[idx] = { ...medications[idx], ...data, updatedAt: new Date().toISOString() };
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(medications[idx]),
      };
    }

    // ── DELETE: Remove medication ─────────────────────────────────
    if (httpMethod === 'DELETE') {
      const segments = path.split('/');
      const id = segments[segments.length - 1];
      medications = medications.filter(m => m.id !== id);
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify({ deleted: id }),
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
