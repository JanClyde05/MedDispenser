/**
 * MedBox — Medications Page Logic
 * Handles the medication form, day selector, and medication list rendering.
 */

// ── Day Selector ──────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  const daySelector = document.getElementById('day-selector');
  if (daySelector) {
    daySelector.addEventListener('click', (e) => {
      if (e.target.classList.contains('day-btn')) {
        e.target.classList.toggle('selected');
      }
    });
  }

  // Load existing medications
  loadMedications();

  // Form submission
  const form = document.getElementById('medication-form');
  if (form) {
    form.addEventListener('submit', handleSaveMedication);
  }
});

// ── Build day bitmask from selected buttons ───────────────────────────
function getDaysBitmask() {
  let bitmask = 0;
  document.querySelectorAll('.day-btn.selected').forEach(btn => {
    const day = parseInt(btn.dataset.day);
    bitmask |= (1 << day);
  });
  return bitmask;
}

function getDaysArray() {
  const days = [];
  document.querySelectorAll('.day-btn.selected').forEach(btn => {
    days.push(parseInt(btn.dataset.day));
  });
  return days;
}

// ── Save Medication ───────────────────────────────────────────────────
async function handleSaveMedication(e) {
  e.preventDefault();

  const data = {
    name: document.getElementById('med-name').value.trim(),
    moduleId: parseInt(document.getElementById('med-module').value),
    pillsPerDose: parseInt(document.getElementById('med-dose').value),
    time: document.getElementById('med-time').value,
    daysOfWeek: getDaysBitmask(),
    days: getDaysArray(),
    startDate: document.getElementById('med-start').value || null,
    endDate: document.getElementById('med-end').value || null,
    enabled: true,
  };

  if (!data.name) {
    showToast('Please enter a medicine name', 'error');
    return;
  }

  try {
    await Api.createMedication(data);
    showToast('Medication saved successfully', 'success');
    document.getElementById('medication-form').reset();
    // Re-select weekday defaults
    document.querySelectorAll('.day-btn').forEach(btn => {
      const day = parseInt(btn.dataset.day);
      if (day >= 1 && day <= 5) btn.classList.add('selected');
      else btn.classList.remove('selected');
    });
    loadMedications();
  } catch (err) {
    console.error('Save medication error:', err);
    showToast('Failed to save medication', 'error');
  }
}

// ── Load & Render Medication List ─────────────────────────────────────
async function loadMedications() {
  const list = document.getElementById('medication-list');
  const emptyState = document.getElementById('med-empty');
  if (!list) return;

  try {
    const meds = await Api.getMedications();
    if (meds && meds.length > 0) {
      list.innerHTML = meds.map(renderMedicationCard).join('');
      if (emptyState) emptyState.style.display = 'none';
    } else {
      list.innerHTML = '';
      if (emptyState) emptyState.style.display = '';
    }
  } catch (err) {
    console.warn('Load medications:', err.message);
    list.innerHTML = '';
    if (emptyState) emptyState.style.display = '';
  }
}

function renderMedicationCard(med) {
  const dayNames = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
  const activeDays = dayNames.filter((_, i) => (med.daysOfWeek >> i) & 1).join(', ');
  const statusClass = med.enabled ? 'success' : 'error';
  const statusText = med.enabled ? 'Active' : 'Disabled';

  return `
    <div class="med-card">
      <div class="card-header">
        <span class="card-title">${med.name}</span>
        <span class="badge badge-${statusClass}">${statusText}</span>
      </div>
      <div class="med-card-detail">
        <p>📦 Module ${med.moduleId} · 💊 ${med.pillsPerDose} tablet(s) · ⏰ ${med.time}</p>
        <p>📅 ${activeDays || 'No days selected'}</p>
        ${med.startDate ? `<p>🟢 From: ${med.startDate}</p>` : ''}
        ${med.endDate ? `<p>🔴 Until: ${med.endDate}</p>` : ''}
      </div>
      <div class="med-card-actions">
        <button class="btn btn-primary btn-sm" onclick="testNotification('${med.name}', ${med.pillsPerDose}, '${med.time}', ${med.moduleId})">🔔 Test Alert</button>
        <button class="btn btn-ghost btn-sm" onclick="toggleMedication('${med.id}', ${!med.enabled})">
          ${med.enabled ? '⏸️ Disable' : '▶️ Enable'}
        </button>
        <button class="btn btn-danger btn-sm" onclick="deleteMedication('${med.id}')">🗑️ Delete</button>
      </div>
    </div>
  `;
}

// ── Test Notification ──────────────────────────────────────────────────
async function testNotification(name, dose, time, moduleId) {
  try {
    showToast('Sending test notification to phone...', 'info');
    await Api.sendNotification({
      medicineName: name,
      dose: dose || 1,
      time: time || 'now',
      moduleId: moduleId || 1,
      type: 'reminder'
    });
    showToast('Notification sent to phone!', 'success');
  } catch (err) {
    showToast('Failed to send notification', 'error');
  }
}

// ── Actions ───────────────────────────────────────────────────────────
async function toggleMedication(id, enabled) {
  try {
    await Api.updateMedication(id, { enabled });
    showToast(enabled ? 'Medication enabled' : 'Medication disabled', 'info');
    loadMedications();
  } catch (err) {
    showToast('Failed to update medication', 'error');
  }
}

async function deleteMedication(id) {
  if (!confirm('Delete this medication?')) return;
  try {
    await Api.deleteMedication(id);
    showToast('Medication deleted', 'success');
    loadMedications();
  } catch (err) {
    showToast('Failed to delete medication', 'error');
  }
}
