/**
 * MedBox — API Client
 * Fetch wrappers for Netlify Functions backend.
 * All API calls go through these functions.
 */

const API_BASE = '/api';  // Netlify Functions are mounted at /.netlify/functions/ or redirected

const Api = {
  // ── Medications ─────────────────────────────────────────────────────
  async getMedications() {
    const res = await fetch(`${API_BASE}/medications`);
    if (!res.ok) throw new Error(`GET medications failed: ${res.status}`);
    return res.json();
  },

  async createMedication(data) {
    const res = await fetch(`${API_BASE}/medications`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data),
    });
    if (!res.ok) throw new Error(`POST medication failed: ${res.status}`);
    return res.json();
  },

  async updateMedication(id, data) {
    const res = await fetch(`${API_BASE}/medications/${id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data),
    });
    if (!res.ok) throw new Error(`PUT medication failed: ${res.status}`);
    return res.json();
  },

  async deleteMedication(id) {
    const res = await fetch(`${API_BASE}/medications/${id}`, {
      method: 'DELETE',
    });
    if (!res.ok) throw new Error(`DELETE medication failed: ${res.status}`);
    return res.json();
  },

  // ── Schedules ───────────────────────────────────────────────────────
  async getSchedules() {
    const res = await fetch(`${API_BASE}/schedules`);
    if (!res.ok) throw new Error(`GET schedules failed: ${res.status}`);
    return res.json();
  },

  async getTodaySchedules() {
    const res = await fetch(`${API_BASE}/schedules?today=true`);
    if (!res.ok) throw new Error(`GET today schedules failed: ${res.status}`);
    return res.json();
  },

  // ── Dispense Log ────────────────────────────────────────────────────
  async getHistory(limit = 50) {
    const res = await fetch(`${API_BASE}/dispense-log?limit=${limit}`);
    if (!res.ok) throw new Error(`GET history failed: ${res.status}`);
    return res.json();
  },

  // ── Device Sync ─────────────────────────────────────────────────────
  async getDeviceSync() {
    const res = await fetch(`${API_BASE}/sync`);
    if (!res.ok) throw new Error(`GET sync failed: ${res.status}`);
    return res.json();
  },
};
