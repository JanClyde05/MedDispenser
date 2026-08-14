// =============================================================================
// MedBox WiFi Captive Portal — Client-Side Logic
// =============================================================================
// Handles: tab switching, network scanning, password modal,
// and WiFi connection requests via fetch() to the ESP32's local endpoints.
//
// Adapted from AiBot portal (QR scanner removed — doesn't work over HTTP).
// =============================================================================

// --- State ---
let selectedSsid = '';

// --- Tab Switching ---
document.querySelectorAll('.tab').forEach(tab => {
  tab.addEventListener('click', () => {
    const tabName = tab.dataset.tab;

    // Update active tab
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    tab.classList.add('active');

    // Update active panel
    document.querySelectorAll('.tab-content').forEach(p => p.classList.remove('active'));
    document.getElementById('panel-' + tabName).classList.add('active');
  });
});

// --- Network Scanning ---
function scanNetworks() {
  const list = document.getElementById('network-list');
  list.innerHTML = '<div class="loading"><div class="spinner"></div><p>Scanning nearby networks...</p></div>';

  fetch('/scan')
    .then(res => res.json())
    .then(networks => {
      if (networks.length === 0) {
        list.innerHTML = '<div class="loading"><p>No networks found. Try rescanning.</p></div>';
        return;
      }

      // Sort by signal strength
      networks.sort((a, b) => b.rssi - a.rssi);

      list.innerHTML = networks.map(net => {
        const signal = getSignalIcon(net.rssi);
        const lock = net.enc ? '&#x1f512;' : '';
        const signalText = net.rssi + ' dBm';

        return `
          <div class="network-item" onclick="selectNetwork('${escapeHtml(net.ssid)}', ${net.enc})">
            <div class="network-info">
              <div class="network-ssid">${escapeHtml(net.ssid)}</div>
              <div class="network-meta">${signalText}</div>
            </div>
            <span class="network-signal">${signal}</span>
            <span class="network-lock">${lock}</span>
          </div>
        `;
      }).join('');
    })
    .catch(err => {
      list.innerHTML = '<div class="loading"><p>Scan failed. Try again.</p></div>';
      console.error('Scan error:', err);
    });
}

function getSignalIcon(rssi) {
  if (rssi > -50) return '&#x2588;&#x2588;&#x2588;&#x2588;';  // Excellent
  if (rssi > -60) return '&#x2588;&#x2588;&#x2588;&#x2591;';  // Good
  if (rssi > -70) return '&#x2588;&#x2588;&#x2591;&#x2591;';  // Fair
  return '&#x2588;&#x2591;&#x2591;&#x2591;';                    // Weak
}

function escapeHtml(str) {
  const div = document.createElement('div');
  div.appendChild(document.createTextNode(str));
  return div.innerHTML;
}

// --- Network Selection ---
function selectNetwork(ssid, encrypted) {
  selectedSsid = ssid;

  if (encrypted) {
    // Show password modal
    document.getElementById('modal-ssid-display').textContent = ssid;
    document.getElementById('modal-pass').value = '';
    document.getElementById('password-modal').classList.add('active');
    setTimeout(() => document.getElementById('modal-pass').focus(), 100);
  } else {
    // Open network — connect directly
    connectToNetwork(ssid, '');
  }
}

function closeModal() {
  document.getElementById('password-modal').classList.remove('active');
}

function connectFromModal() {
  const pass = document.getElementById('modal-pass').value;
  closeModal();
  connectToNetwork(selectedSsid, pass);
}

// --- Manual Entry ---
function connectManual() {
  const ssid = document.getElementById('manual-ssid').value.trim();
  const pass = document.getElementById('manual-pass').value;

  if (!ssid) {
    document.getElementById('manual-ssid').focus();
    return;
  }

  connectToNetwork(ssid, pass);
}

// --- Connect to Network ---
function connectToNetwork(ssid, pass) {
  showStatus('Connecting to ' + ssid + '...');

  fetch('/connect', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ ssid: ssid, pass: pass })
  })
  .then(res => res.json())
  .then(data => {
    if (data.status === 'connecting') {
      // Poll for result
      pollConnectionStatus();
    } else {
      showStatus('Connection failed: ' + (data.message || 'Unknown error'));
    }
  })
  .catch(err => {
    showStatus('Request failed. Please try again.');
    console.error('Connect error:', err);
  });
}

function pollConnectionStatus() {
  let attempts = 0;
  const maxAttempts = 30;  // 30 seconds max

  const poll = () => {
    attempts++;
    if (attempts > maxAttempts) {
      showStatus('Connection timed out. Please try again.');
      return;
    }

    fetch('/status')
      .then(res => res.json())
      .then(data => {
        if (data.connectResult === 'success') {
          showSuccess(data.ip || '');
        } else if (data.connectResult === 'failed') {
          showStatus('Connection failed. Check password and try again.');
        } else {
          // Still connecting...
          setTimeout(poll, 1000);
        }
      })
      .catch(() => {
        // ESP32 may have switched to STA mode, losing AP connectivity
        // If we can't reach it, assume success
        showSuccess('');
      });
  };

  setTimeout(poll, 2000);  // Wait 2s before first poll
}

// --- UI Helpers ---
function showStatus(text) {
  const bar = document.getElementById('status-bar');
  const textEl = document.getElementById('status-text');
  textEl.textContent = text;
  bar.style.display = 'flex';
}

function hideStatus() {
  document.getElementById('status-bar').style.display = 'none';
}

function showSuccess(ip) {
  document.querySelector('.container').innerHTML = `
    <div class="success-message">
      <div class="success-icon">&#x2705;</div>
      <h2>Connected!</h2>
      <p>${ip ? 'IP Address: ' + ip : 'Your MedBox is now online!'}</p>
      <p class="success-note">You can close this page now.</p>
    </div>
  `;
}

function togglePassword(inputId) {
  const input = document.getElementById(inputId);
  input.type = input.type === 'password' ? 'text' : 'password';
}

// --- Initialize ---
document.addEventListener('DOMContentLoaded', () => {
  scanNetworks();
});
