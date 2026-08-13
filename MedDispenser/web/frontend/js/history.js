/**
 * MedBox — History Page Logic
 * Loads and renders the dispense log from the backend.
 */

document.addEventListener('DOMContentLoaded', () => {
  loadHistory();
});

async function loadHistory() {
  const tbody = document.getElementById('history-body');
  if (!tbody) return;

  try {
    const logs = await Api.getHistory(100);
    if (logs && logs.length > 0) {
      tbody.innerHTML = logs.map(renderHistoryRow).join('');
    }
  } catch (err) {
    console.warn('Load history (using placeholder data):', err.message);
    // Placeholder data is already in HTML
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
  const dateStr = log.actualTime || log.scheduledTime || '—';

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
