/**
 * MedBox — Core App Logic
 * Shared utilities: toast notifications, navigation, dashboard rendering.
 */

// ── Toast Notifications ───────────────────────────────────────────────
function showToast(message, type = 'info') {
  const container = document.getElementById('toast-container');
  if (!container) return;

  const toast = document.createElement('div');
  toast.className = 'toast';
  toast.style.borderLeft = `4px solid var(--color-${type})`;
  toast.textContent = message;
  container.appendChild(toast);

  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateX(100%)';
    toast.style.transition = 'all 0.3s ease';
    setTimeout(() => toast.remove(), 300);
  }, 3000);
}

// ── Dashboard ─────────────────────────────────────────────────────────
async function loadDashboard() {
  const scheduleList = document.getElementById('schedule-list');
  if (!scheduleList) return;

  try {
    const schedules = await Api.getTodaySchedules();
    if (schedules && schedules.length > 0) {
      scheduleList.innerHTML = schedules.map(s => renderScheduleCard(s)).join('');
    }
  } catch (err) {
    console.warn('Dashboard load (using placeholder data):', err.message);
    // Placeholder data is already in HTML for offline/demo
  }
}

function renderScheduleCard(schedule) {
  const statusClass = schedule.status === 'completed' ? 'success' :
                      schedule.status === 'missed' ? 'error' : 'warning';
  const statusText  = schedule.status === 'completed' ? 'Completed' :
                      schedule.status === 'missed' ? 'Missed' : 'Pending';

  return `
    <div class="schedule-card">
      <div class="schedule-time">${schedule.time || '--:--'}</div>
      <div class="schedule-info">
        <div class="schedule-medicine">${schedule.medicineName || 'Unknown'}</div>
        <div class="schedule-dose">${schedule.pillsPerDose || 1} tablet(s) · Module ${schedule.moduleId || '?'}</div>
      </div>
      <span class="badge badge-${statusClass}">${statusText}</span>
    </div>
  `;
}

// ── Init ──────────────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  loadDashboard();
});
