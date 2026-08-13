/**
 * MedBox — Schedule Display Logic
 * Used on the dashboard to render today's schedule cards.
 * Schedule CRUD is handled through the medications page (schedule is part of medication).
 */

function formatTime12h(time24) {
  if (!time24) return '--:--';
  const [h, m] = time24.split(':').map(Number);
  const ampm = h >= 12 ? 'PM' : 'AM';
  const h12 = h % 12 || 12;
  return `${h12}:${String(m).padStart(2, '0')} ${ampm}`;
}

function formatTimeShort(time24) {
  if (!time24) return '--:--';
  return time24;  // Keep 24h for compact display
}

function isScheduleActiveToday(schedule) {
  const today = new Date().getDay();  // 0=Sun
  return (schedule.daysOfWeek >> today) & 1;
}

function sortSchedulesByTime(schedules) {
  return schedules.sort((a, b) => {
    if (a.time < b.time) return -1;
    if (a.time > b.time) return 1;
    return 0;
  });
}
