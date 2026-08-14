/**
 * MedBox — Medications API (Netlify Function)
 * CRUD operations for medication records.
 * Persistent storage via Netlify Blobs.
 */

const { getStore, connectLambda } = require('@netlify/blobs');

const STORE_NAME = 'medications';

async function getAllMedications(store) {
  const list = await store.get('_index', { type: 'json' });
  if (!list || !Array.isArray(list)) return [];
  const meds = [];
  for (const id of list) {
    const med = await store.get(`med_${id}`, { type: 'json' });
    if (med) meds.push(med);
  }
  return meds;
}

async function getNextId(store) {
  const counter = await store.get('_counter');
  const next = counter ? parseInt(counter) + 1 : 1;
  await store.set('_counter', String(next));
  return next;
}

async function addToIndex(store, id) {
  const list = await store.get('_index', { type: 'json' }) || [];
  list.push(id);
  await store.set('_index', JSON.stringify(list));
}

async function removeFromIndex(store, id) {
  let list = await store.get('_index', { type: 'json' }) || [];
  list = list.filter(i => i !== id);
  await store.set('_index', JSON.stringify(list));
}

exports.handler = async (event, context) => {
  connectLambda(event);

  const headers = {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type',
  };

  if (event.httpMethod === 'OPTIONS') {
    return { statusCode: 204, headers, body: '' };
  }

  try {
    const store = getStore(STORE_NAME);

    // ── GET: List all medications ──────────────────────────────────
    if (event.httpMethod === 'GET') {
      const meds = await getAllMedications(store);
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(meds),
      };
    }

    // ── POST: Create medication ───────────────────────────────────
    if (event.httpMethod === 'POST') {
      const data = JSON.parse(event.body);
      const id = await getNextId(store);
      const med = {
        id: String(id),
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
      await store.set(`med_${med.id}`, JSON.stringify(med));
      await addToIndex(store, med.id);
      return {
        statusCode: 201,
        headers,
        body: JSON.stringify(med),
      };
    }

    // ── PUT: Update medication ────────────────────────────────────
    if (event.httpMethod === 'PUT') {
      const segments = event.path.split('/');
      const id = segments[segments.length - 1];
      const existing = await store.get(`med_${id}`, { type: 'json' });
      if (!existing) {
        return { statusCode: 404, headers, body: JSON.stringify({ error: 'Not found' }) };
      }
      const data = JSON.parse(event.body);
      const updated = { ...existing, ...data, updatedAt: new Date().toISOString() };
      await store.set(`med_${id}`, JSON.stringify(updated));
      return {
        statusCode: 200,
        headers,
        body: JSON.stringify(updated),
      };
    }

    // ── DELETE: Remove medication ─────────────────────────────────
    if (event.httpMethod === 'DELETE') {
      const segments = event.path.split('/');
      const id = segments[segments.length - 1];
      await store.delete(`med_${id}`);
      await removeFromIndex(store, id);
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
