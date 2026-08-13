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
    name:         document.getElementById('med-name').value.trim(),
    moduleId:     parseInt(document.getElementById('med-module').value),
    pillsPerDose: parseInt(document.getElementById('med-dose').value),
    time:         document.getElementById('med-time').value,
    daysOfWeek:   getDaysBitmask(),
    days:         getDaysArray(),
    startDate:    document.getElementById('med-start').value || null,
    endDate:      document.getElementById('med-end').value || null,
    enabled:      true,
  };

  if (!data.name) {
    showToast('Please enter a medicine name', 'error');
    return;
  }

  try {
    await Api.createMedication(data);
    showToast('Medication saved successfully', 'success');
    document.getElementById('medication-form').reset();
    loadMedications();
  } catch (err) {
    console.error('Save medication error:', err);
    showToast('Failed to save medication', 'error');
  }
}

// ── Load & Render Medication List ─────────────────────────────────────
async function loadMedications() {
  const list = document.getElementById('medication-list');
  if (!list) return;

  try {
    const meds = await Api.getMedications();
    if (meds && meds.length > 0) {
      list.innerHTML = meds.map(renderMedicationCard).join('');
    } else {
      list.innerHTML = '<p style="color: var(--color-text-muted);">No medications configured yet.</p>';
    }
  } catch (err) {
    console.warn('Load medications (backend not ready):', err.message);
    list.innerHTML = '<p style="color: var(--color-text-dim);">Connect backend to see medications.</p>';
  }
}

function renderMedicationCard(med) {
  const dayNames = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
  const activeDays = dayNames.filter((_, i) => (med.daysOfWeek >> i) & 1).join(', ');
  const statusClass = med.enabled ? 'success' : 'error';
  const statusText  = med.enabled ? 'Active' : 'Disabled';

  return `
    <div class="card">
      <div class="card-header">
        <span class="card-title">${med.name}</span>
        <span class="badge badge-${statusClass}">${statusText}</span>
      </div>
      <div style="color: var(--color-text-muted); font-size: var(--font-size-sm);">
        <p>Module ${med.moduleId} · ${med.pillsPerDose} tablet(s) · ${med.time}</p>
        <p>${activeDays || 'No days selected'}</p>
        ${med.startDate ? `<p>From: ${med.startDate}</p>` : ''}
        ${med.endDate ? `<p>Until: ${med.endDate}</p>` : ''}
      </div>
      <div style="margin-top: var(--space-4); display: flex; gap: var(--space-2);">
        <button class="btn btn-ghost" onclick="toggleMedication('${med.id}', ${!med.enabled})">
          ${med.enabled ? 'Disable' : 'Enable'}
        </button>
        <button class="btn btn-danger" onclick="deleteMedication('${med.id}')">Delete</button>
      </div>
    </div>
  `;
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
