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


// ══════════════════════════════════════════════════════════════════════
// Hardware Test Panel — Direct hardware commands to ESP32
// ══════════════════════════════════════════════════════════════════════

const HW_COMMAND_LABELS = {
  BUZZ:     '🔊 Buzzer',
  PING:     '📡 Ping C3',
  OPEN:     '🔓 Open Hatch',
  CLOSE:    '🔒 Close Hatch',
  DISPENSE: '⚙️ Dispense',
  HOME:     '🏠 Home',
};

/**
 * Send a direct hardware test command to the ESP32.
 * @param {string} command - BUZZ, PING, OPEN, CLOSE, DISPENSE, HOME
 * @param {number} [moduleId=0] - Module ID (1-3), 0 for non-module commands
 */
async function testHW(command, moduleId = 0) {
  const label = HW_COMMAND_LABELS[command] || command;
  const moduleLabel = moduleId > 0 ? ` (Module ${moduleId})` : '';

  try {
    showToast(`Sending ${label}${moduleLabel}...`, 'info');

    await Api.sendDispenseCommand({
      moduleId: moduleId || 1,
      medicineName: `HW_TEST_${command}`,
      dose: 1,
      time: 'now',
      type: 'test_hardware',
      command: command,
    });

    showToast(`✅ ${label}${moduleLabel} command queued!`, 'success');
  } catch (err) {
    console.error(`Hardware test (${command}) failed:`, err);
    showToast(`Failed to send ${label} command`, 'error');
  }
}

/**
 * Send a dispense test command with selected pill count (1, 2, or 3 revolutions).
 * @param {number} moduleId - Module ID (1-3)
 */
async function testHWDispense(moduleId) {
  const selectElem = document.getElementById(`hw-dose-${moduleId}`);
  const count = parseInt(selectElem ? selectElem.value : '1');

  try {
    showToast(`Sending Dispense M${moduleId} (${count} pill(s) / ${count} rev(s))...`, 'info');

    await Api.sendDispenseCommand({
      moduleId: moduleId,
      medicineName: `HW_TEST_DISPENSE`,
      dose: count,
      time: 'now',
      type: 'test_hardware',
      command: 'DISPENSE',
    });

    showToast(`✅ Dispense M${moduleId} (${count} revs) command queued!`, 'success');
  } catch (err) {
    console.error(`Hardware dispense test (M${moduleId}) failed:`, err);
    showToast(`Failed to send dispense command`, 'error');
  }
}


/**
 * Send an IR sensor test command — arms the IR sensor to trigger an action on next detection.
 * @param {string} action - 'buzz', 'servo', or 'both'
 */
async function testIR(action) {
  const moduleId = parseInt(document.getElementById('ir-test-module')?.value || '1');

  const actionLabels = {
    buzz:  '🔊 Buzzer Only',
    servo: `⚙️ Open Hatch M${moduleId}`,
    both:  `🔊+⚙️ Buzzer + Hatch M${moduleId}`,
  };

  const label = actionLabels[action] || action;

  try {
    showToast(`Arming IR sensor → ${label}...`, 'info');

    await Api.sendDispenseCommand({
      moduleId: moduleId,
      medicineName: `IR_TEST_${action.toUpperCase()}`,
      dose: 1,
      time: 'now',
      type: 'test_hardware',
      command: 'IR_TEST',
      irAction: action,
    });

    showToast(`✅ IR sensor armed! Wave your hand to trigger ${label}`, 'success');
  } catch (err) {
    console.error(`IR test (${action}) failed:`, err);
    showToast('Failed to arm IR sensor test', 'error');
  }
}
