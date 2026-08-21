/**
 * MedBox — Core App Logic
 * Handles: toast notifications, device visualization hub, dashboard rendering.
 */

// ── Toast Notifications ───────────────────────────────────────────────
function showToast(message, type = 'info') {
  const container = document.getElementById('toast-container');
  if (!container) return;

  const colorMap = {
    success: 'var(--color-success)',
    error:   'var(--color-error)',
    warning: 'var(--color-warning)',
    info:    'var(--color-primary-light)',
  };

  const toast = document.createElement('div');
  toast.className = 'toast';
  toast.style.borderLeft = `4px solid ${colorMap[type] || colorMap.info}`;
  toast.textContent = message;
  container.appendChild(toast);

  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateX(120%)';
    toast.style.transition = 'all 0.4s cubic-bezier(0.4, 0, 0.2, 1)';
    setTimeout(() => toast.remove(), 400);
  }, 3500);
}


// ── Device Visualization Hub ──────────────────────────────────────────

/**
 * Render interactive MedBox device icons in the device hub.
 * - Main module: larger box with blue glow
 * - Expansion modules: smaller boxes with cyan glow
 * - Only active modules are shown
 */
function renderDeviceHub(devices) {
  const hub = document.getElementById('device-hub');
  const empty = document.getElementById('device-empty');
  if (!hub) return;

  // If no devices, show the empty placeholder
  if (!devices || devices.length === 0) {
    if (empty) empty.style.display = '';
    return;
  }

  // Hide the empty placeholder
  if (empty) empty.style.display = 'none';

  // Collect all active modules across all devices
  const modules = [];
  let hasMain = false;

  devices.forEach(dev => {
    const activeModules = dev.activeModules || [1];
    activeModules.forEach(modId => {
      const isMain = (dev.isMain && modId === 1);
      if (isMain) hasMain = true;
      modules.push({
        deviceId: dev.deviceId,
        deviceName: dev.name || dev.deviceId,
        moduleId: modId,
        isMain,
        status: dev.status || 'offline',
        ip: dev.ip,
        lastSeen: dev.lastSeen,
      });
    });
  });

  // Sort: main first, then by moduleId
  modules.sort((a, b) => {
    if (a.isMain && !b.isMain) return -1;
    if (!a.isMain && b.isMain) return 1;
    return a.moduleId - b.moduleId;
  });

  // If only one device with one module and no expansion, still show it as main
  if (modules.length === 1 && !hasMain) {
    modules[0].isMain = true;
  }

  // Build HTML
  const html = modules.map(mod => {
    const typeClass = mod.isMain ? 'main' : 'expansion';
    const statusClass = mod.status === 'online' ? '' : 'offline';
    const label = mod.isMain ? 'Main Module' : `Module ${mod.moduleId}`;
    const icon = mod.isMain ? '💊' : '💉';
    const sublabel = mod.status === 'online'
      ? (mod.ip ? `IP: ${mod.ip}` : 'Connected')
      : 'Offline';

    return `
      <div class="device-module ${typeClass}" title="${mod.deviceName} — Module ${mod.moduleId}">
        <div class="device-box">
          <div class="device-status-dot ${statusClass}"></div>
          <div class="device-icon">${icon}</div>
          <span style="font-size: var(--font-size-xs); color: var(--color-text-muted); font-weight: 500;">
            ${mod.isMain ? '⭐' : ''} #${mod.moduleId}
          </span>
        </div>
        <div class="device-module-label">${label}</div>
        <div class="device-module-sublabel">${sublabel}</div>
      </div>
    `;
  }).join('');

  // Replace hub content (keep empty div hidden)
  // Remove existing module elements but keep the empty placeholder
  Array.from(hub.children).forEach(child => {
    if (child.id !== 'device-empty') child.remove();
  });

  hub.insertAdjacentHTML('afterbegin', html);
}


// ── Dashboard ─────────────────────────────────────────────────────────

async function loadDashboard() {
  // Load devices
  loadDevices();

  // Load today's schedules
  loadTodaySchedules();
}

async function loadDevices() {
  try {
    const devices = await Api.getDevices();
    renderDeviceHub(devices);

    // Update status bar
    const statusEl = document.getElementById('device-status');
    const modulesEl = document.getElementById('active-modules');
    const syncEl = document.getElementById('last-sync');

    if (devices && devices.length > 0) {
      const onlineDevices = devices.filter(d => d.status === 'online');
      const totalModules = devices.reduce((sum, d) => sum + (d.activeModules?.length || 1), 0);

      if (statusEl) {
        statusEl.textContent = onlineDevices.length > 0 ? 'Online' : 'Offline';
        statusEl.style.color = onlineDevices.length > 0 ? 'var(--color-success)' : 'var(--color-error)';
      }
      if (modulesEl) modulesEl.textContent = totalModules;

      // Last sync from most recently seen device
      if (syncEl && onlineDevices.length > 0) {
        const sorted = onlineDevices.sort((a, b) =>
          new Date(b.lastSeen) - new Date(a.lastSeen)
        );
        const lastSeen = new Date(sorted[0].lastSeen);
        const now = new Date();
        const diffMin = Math.floor((now - lastSeen) / 60000);
        syncEl.textContent = diffMin < 1 ? 'Just now'
                           : diffMin < 60 ? `${diffMin}m ago`
                           : `${Math.floor(diffMin / 60)}h ago`;
      }
    } else {
      // No devices registered — show defaults
      if (statusEl) {
        statusEl.textContent = 'No devices';
        statusEl.style.color = 'var(--color-text-dim)';
      }
      if (modulesEl) modulesEl.textContent = '0';
    }
  } catch (err) {
    console.warn('Load devices:', err.message);
    // Show empty state
    renderDeviceHub([]);
  }
}

async function loadTodaySchedules() {
  const scheduleList = document.getElementById('schedule-list');
  const emptyState = document.getElementById('schedule-empty');
  const countEl = document.getElementById('today-count');
  if (!scheduleList) return;

  try {
    const schedules = await Api.getTodaySchedules();

    if (schedules && schedules.length > 0) {
      const sorted = sortSchedulesByTime(schedules);
      scheduleList.innerHTML = sorted.map(renderScheduleCard).join('');
      if (emptyState) emptyState.style.display = 'none';
      if (countEl) countEl.textContent = schedules.length;
    } else {
      scheduleList.innerHTML = '';
      if (emptyState) emptyState.style.display = '';
      if (countEl) countEl.textContent = '0';
    }
  } catch (err) {
    console.warn('Dashboard schedule load:', err.message);
    scheduleList.innerHTML = '';
    if (emptyState) emptyState.style.display = '';
    if (countEl) countEl.textContent = '—';
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


// ── Automatic Schedule Checker ────────────────────────────────────────
// Runs on the dashboard page to detect when a medication time is reached
// and automatically fire notifications + hardware dispense commands.

const _triggeredSchedules = new Set();  // "medId_HH:MM_YYYY-MM-DD" keys
let _scheduleCheckInterval = null;

/**
 * Check all today's schedules against the current time.
 * If a match is found and hasn't been triggered yet, fire notification + dispense.
 */
async function checkSchedulesNow() {
  try {
    const schedules = await Api.getTodaySchedules();
    if (!schedules || schedules.length === 0) return;

    const now = new Date();
    const currentHH = String(now.getHours()).padStart(2, '0');
    const currentMM = String(now.getMinutes()).padStart(2, '0');
    const currentTime = `${currentHH}:${currentMM}`;
    const todayDate = now.toISOString().split('T')[0];  // YYYY-MM-DD

    for (const sched of schedules) {
      if (!sched.time) continue;

      // Normalize schedule time to HH:MM
      const schedTime = sched.time.substring(0, 5);  // "07:00" from "07:00" or "07:00:00"

      if (schedTime !== currentTime) continue;

      // Build unique key to avoid re-triggering within the same minute
      const key = `${sched.medicationId || sched.medicineName}_${schedTime}_${todayDate}`;
      if (_triggeredSchedules.has(key)) continue;

      // Mark as triggered
      _triggeredSchedules.add(key);

      console.log(`[Schedule Checker] Time match: ${sched.medicineName} at ${schedTime}`);

      // Show prominent toast
      showToast(
        `⏰ Time for ${sched.medicineName}! ${sched.pillsPerDose || 1} tablet(s) — Module ${sched.moduleId || 1}`,
        'warning'
      );

      // Send ntfy notification (same as test button)
      try {
        await Api.sendNotification({
          medicineName: sched.medicineName,
          dose: sched.pillsPerDose || 1,
          time: schedTime,
          moduleId: sched.moduleId || 1,
          type: 'reminder',
        });
        console.log(`[Schedule Checker] Notification sent for ${sched.medicineName}`);
      } catch (notifyErr) {
        console.warn('[Schedule Checker] Notification failed:', notifyErr.message);
      }

      // Send dispense command to ESP32 (buzzer + servo + IR flow)
      try {
        await Api.sendDispenseCommand({
          moduleId: sched.moduleId || 1,
          medicineName: sched.medicineName,
          dose: sched.pillsPerDose || 1,
          time: schedTime,
          type: 'scheduled',
        });
        console.log(`[Schedule Checker] Dispense command queued for ${sched.medicineName}`);
      } catch (cmdErr) {
        console.warn('[Schedule Checker] Dispense command failed:', cmdErr.message);
      }
    }

    // Cleanup old trigger keys (from previous days)
    for (const key of _triggeredSchedules) {
      if (!key.endsWith(todayDate)) {
        _triggeredSchedules.delete(key);
      }
    }

  } catch (err) {
    console.warn('[Schedule Checker] Error:', err.message);
  }
}

/**
 * Start the schedule checker interval (every 30 seconds).
 */
function startScheduleChecker() {
  if (_scheduleCheckInterval) return;  // Already running

  // Run immediately on start
  checkSchedulesNow();

  // Then check every 30 seconds
  _scheduleCheckInterval = setInterval(checkSchedulesNow, 30000);
  console.log('[Schedule Checker] Started (30s interval)');
}


// ── Init ──────────────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  // Only run dashboard logic on the dashboard page
  if (document.getElementById('device-hub')) {
    loadDashboard();

    // Start the automatic schedule checker
    startScheduleChecker();

    // Auto-refresh device status every 60 seconds
    setInterval(() => {
      loadDevices();
      loadTodaySchedules();
    }, 60000);
  }
});
