/**
 * MedBox — Settings Page Logic
 * Handles saving notification preferences, device configuration, and testing.
 */

document.addEventListener('DOMContentLoaded', () => {
  // Load saved settings
  loadNotificationSettings();

  // Save Notification Settings button
  const btnSaveNotify = document.getElementById('btn-save-notify');
  if (btnSaveNotify) {
    btnSaveNotify.addEventListener('click', handleSaveNotificationSettings);
  }

  // Send Test Notification button
  const btnTestNotify = document.getElementById('btn-test-notify');
  if (btnTestNotify) {
    btnTestNotify.addEventListener('click', handleTestNotification);
  }
});

function cleanTopicName(topicInput) {
  if (!topicInput) return 'med-box-notification';
  let cleaned = topicInput.trim();
  // Strip trailing slashes and extract last path component if full URL
  if (cleaned.includes('/')) {
    const parts = cleaned.replace(/\/+$/, '').split('/');
    cleaned = parts[parts.length - 1];
  }
  return cleaned || 'med-box-notification';
}

function loadNotificationSettings() {
  const topicInput = document.getElementById('ntfy-topic');
  const intervalInput = document.getElementById('reminder-interval');
  const maxInput = document.getElementById('max-reminders');

  const savedTopic = localStorage.getItem('medbox_ntfy_topic') || 'med-box-notification';
  const savedInterval = localStorage.getItem('medbox_reminder_interval') || '5';
  const savedMax = localStorage.getItem('medbox_max_reminders') || '12';

  if (topicInput) topicInput.value = savedTopic;
  if (intervalInput) intervalInput.value = savedInterval;
  if (maxInput) maxInput.value = savedMax;
}

function handleSaveNotificationSettings(e) {
  e.preventDefault();
  const rawTopic = document.getElementById('ntfy-topic').value;
  const topic = cleanTopicName(rawTopic);
  const interval = document.getElementById('reminder-interval').value;
  const maxReminders = document.getElementById('max-reminders').value;

  localStorage.setItem('medbox_ntfy_topic', topic);
  localStorage.setItem('medbox_reminder_interval', interval);
  localStorage.setItem('medbox_max_reminders', maxReminders);

  // Update input field to display the clean topic
  document.getElementById('ntfy-topic').value = topic;

  showToast(`Saved! ntfy Topic: ${topic}`, 'success');
}

async function handleTestNotification(e) {
  e.preventDefault();
  const rawTopic = document.getElementById('ntfy-topic').value;
  const topic = cleanTopicName(rawTopic);

  showToast(`Sending test alert to ${topic}...`, 'info');

  try {
    await Api.sendNotification({
      medicineName: 'Test Medicine',
      dose: 1,
      time: 'Now',
      moduleId: 1,
      type: 'reminder',
      topic: topic
    });
    showToast(`Test alert sent to ntfy topic: ${topic}!`, 'success');
  } catch (err) {
    console.error('Test notification failed:', err);
    showToast('Failed to send test notification', 'error');
  }
}
