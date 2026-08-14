/**
 * MedBox — History Page Logic
 * Loads and renders the dispense log from the backend.
 */

document.addEventListener('DOMContentLoaded', () => {
  loadHistory();
});

async function loadHistory() {
  const tbody = document.getElementById('history-body');
  const emptyState = document.getElementById('history-empty');
  if (!tbody) return;

  try {
    const logs = await Api.getHistory(100);
    if (logs && logs.length > 0) {
      tbody.innerHTML = logs.map(renderHistoryRow).join('');
      if (emptyState) emptyState.style.display = 'none';
    } else {
      tbody.innerHTML = '';
      if (emptyState) emptyState.style.display = '';
    }
  } catch (err) {
    console.warn('Load history:', err.message);
    tbody.innerHTML = '';
    if (emptyState) emptyState.style.display = '';
  }
}

function renderHistoryRow(log) {
  const statusMap = {
    completed: { cls: 'success', text: 'Completed' },
    dispensed: { cls: 'success', text: 'Dispensed' },
    missed:    { cls: 'error',   text: 'Missed' },
    pending:   { cls: 'warning', text: 'Pending' },
    error:     { cls: 'error',   text: 'Error' },
  };

  const status = statusMap[log.status] || statusMap.pending;

  // Format the date nicely
  let dateStr = '—';
  const rawDate = log.actualTime || log.scheduledTime;
  if (rawDate) {
    try {
      const d = new Date(rawDate);
      dateStr = d.toLocaleDateString('en-PH', {
        month: '2-digit', day: '2-digit',
      }) + ' ' + d.toLocaleTimeString('en-PH', {
        hour: '2-digit', minute: '2-digit', hour12: false,
      });
    } catch {
      dateStr = rawDate;
    }
  }

  return `
    <tr>
      <td>${dateStr}</td>
      <td>${log.medicineName || '—'}</td>
      <td>${log.requestedQuantity || 1} tablet(s)</td>
      <td>Module ${log.moduleId || '?'}</td>
      <td><span class="badge badge-${status.cls}">${status.text}</span></td>
    </tr>
  `;
}
