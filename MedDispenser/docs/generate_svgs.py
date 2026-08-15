import os

docs_dir = r"d:\JCTalosig\Projects\MedDispenser\MedDispenser\docs\diagrams"
os.makedirs(docs_dir, exist_ok=True)

# ---------------------------------------------------------------------------
# 1. System Architecture SVG
# ---------------------------------------------------------------------------
svg_1 = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1100 850" width="100%" height="100%" style="background:#0f172a; font-family: system-ui, -apple-system, sans-serif;">
  <defs>
    <linearGradient id="cloudGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#1e293b" />
      <stop offset="100%" stop-color="#0f172a" />
    </linearGradient>
    <linearGradient id="blueCard" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#1e3a8a" stop-opacity="0.4" />
      <stop offset="100%" stop-color="#1e293b" stop-opacity="0.8" />
    </linearGradient>
    <linearGradient id="purpleCard" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#581c87" stop-opacity="0.4" />
      <stop offset="100%" stop-color="#1e293b" stop-opacity="0.8" />
    </linearGradient>
    <linearGradient id="cyanCard" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#155e75" stop-opacity="0.4" />
      <stop offset="100%" stop-color="#1e293b" stop-opacity="0.8" />
    </linearGradient>
    <filter id="glow" x="-20%" y="-20%" width="140%" height="140%">
      <feGaussianBlur stdDeviation="6" result="blur" />
      <feComposite in="SourceGraphic" in2="blur" operator="over" />
    </filter>
    <marker id="arrow" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#38bdf8" />
    </marker>
    <marker id="arrowPurple" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#c084fc" />
    </marker>
  </defs>

  <style>
    .title { font-size: 24px; font-weight: 700; fill: #f8fafc; }
    .subtitle { font-size: 14px; fill: #94a3b8; }
    .section-title { font-size: 16px; font-weight: 600; fill: #38bdf8; }
    .node-title { font-size: 15px; font-weight: 600; fill: #f1f5f9; }
    .node-sub { font-size: 12px; fill: #94a3b8; }
    .badge { font-size: 11px; font-weight: 600; fill: #38bdf8; }
    .edge-label { font-size: 11px; fill: #cbd5e1; font-weight: 500; }
  </style>

  <!-- Header -->
  <text x="40" y="45" class="title">MedBox — System Architecture Overview</text>
  <text x="40" y="70" class="subtitle">Cloud Backend, Phone Push Alerts, Dual ESP32 Controller &amp; Modular Servos</text>

  <!-- CLOUD LAYER -->
  <rect x="40" y="100" width="1020" height="200" rx="16" fill="url(#cloudGrad)" stroke="#334155" stroke-width="2" />
  <text x="60" y="130" class="section-title">☁️ CLOUD LAYER (Netlify Serverless + ntfy Push)</text>

  <!-- Web Dashboard -->
  <rect x="70" y="150" width="260" height="120" rx="12" fill="url(#blueCard)" stroke="#3b82f6" stroke-width="1.5" />
  <text x="90" y="180" class="node-title">💻 Web Dashboard</text>
  <text x="90" y="205" class="node-sub">• HTML/CSS/JS Frontend</text>
  <text x="90" y="225" class="node-sub">• Schedule Management UI</text>
  <text x="90" y="245" class="node-sub">• Instant "Test Alert" Trigger</text>

  <!-- Netlify API -->
  <rect x="400" y="150" width="300" height="120" rx="12" fill="url(#blueCard)" stroke="#06b6d4" stroke-width="1.5" />
  <text x="420" y="180" class="node-title">⚡ Netlify Functions &amp; Blobs</text>
  <text x="420" y="205" class="node-sub">• /api/sync (Schedule Distribution)</text>
  <text x="420" y="225" class="node-sub">• /api/notify (ntfy Publisher)</text>
  <text x="420" y="245" class="node-sub">• /api/dispense-log (Audit History)</text>

  <!-- ntfy Server -->
  <rect x="770" y="150" width="260" height="120" rx="12" fill="url(#purpleCard)" stroke="#a855f7" stroke-width="1.5" />
  <text x="790" y="180" class="node-title">🔔 ntfy.sh Server</text>
  <text x="790" y="205" class="node-sub">• Topic: med-box-notification</text>
  <text x="790" y="225" class="node-sub">• High-Priority Push Gateway</text>
  <text x="790" y="245" class="node-sub">• Phone Alert Dispatcher</text>

  <!-- Connectors Cloud -->
  <path d="M 330 210 L 400 210" stroke="#38bdf8" stroke-width="2" marker-end="url(#arrow)" />
  <path d="M 700 210 L 770 210" stroke="#c084fc" stroke-width="2" marker-end="url(#arrowPurple)" />

  <!-- PHONE LAYER -->
  <rect x="770" y="340" width="260" height="100" rx="12" fill="url(#purpleCard)" stroke="#c084fc" stroke-width="2" filter="url(#glow)" />
  <text x="790" y="375" class="node-title">📱 Patient Mobile Phone</text>
  <text x="790" y="400" class="node-sub">• ntfy App Alert</text>
  <text x="790" y="420" class="node-sub">• "💊 Medication Reminder"</text>
  <path d="M 900 270 L 900 340" stroke="#c084fc" stroke-width="2.5" stroke-dasharray="4" marker-end="url(#arrowPurple)" />
  <text x="910" y="310" class="edge-label" fill="#c084fc">Push Alert</text>

  <!-- HARDWARE LAYER -->
  <rect x="40" y="470" width="1020" height="340" rx="16" fill="url(#cloudGrad)" stroke="#334155" stroke-width="2" />
  <text x="60" y="500" class="section-title">📦 HARDWARE LAYER (MedBox Physical Unit)</text>

  <!-- ESP32-S3 Main Brain -->
  <rect x="70" y="525" width="290" height="260" rx="12" fill="url(#blueCard)" stroke="#3b82f6" stroke-width="2" filter="url(#glow)" />
  <text x="90" y="555" class="node-title">🧠 ESP32-S3 (Main Brain)</text>
  <text x="90" y="585" class="node-sub">• SoftAP Captive Portal Wi-Fi</text>
  <text x="90" y="610" class="node-sub">• NTP Real-Time Clock Sync</text>
  <text x="90" y="635" class="node-sub">• NVS Local Flash Schedule</text>
  <text x="90" y="660" class="node-sub">• IR Proximity Sensor Gate</text>
  <text x="90" y="685" class="node-sub">• Piezo Buzzer Controller</text>
  <text x="90" y="710" class="node-sub">• State Machine Logic</text>

  <!-- ESP32-C3 Motion -->
  <rect x="430" y="525" width="260" height="260" rx="12" fill="url(#cyanCard)" stroke="#06b6d4" stroke-width="2" />
  <text x="450" y="555" class="node-title">⚙️ ESP32-C3 (Motion Brain)</text>
  <text x="450" y="585" class="node-sub">• Dedicated Servo Controller</text>
  <text x="450" y="610" class="node-sub">• UART Packet Parser</text>
  <text x="450" y="635" class="node-sub">• 6-Channel PWM Driver</text>
  <text x="450" y="660" class="node-sub">• Dispenser Rotor Driver</text>
  <text x="450" y="685" class="node-sub">• Hatch Door Opener</text>

  <!-- Actuators -->
  <rect x="750" y="525" width="280" height="260" rx="12" fill="url(#blueCard)" stroke="#10b981" stroke-width="1.5" />
  <text x="770" y="555" class="node-title">🤖 Modular Motors &amp; Servos</text>
  <text x="770" y="585" class="node-sub">• Module 1: Dispenser &amp; Hatch</text>
  <text x="770" y="610" class="node-sub">• Module 2: Dispenser &amp; Hatch</text>
  <text x="770" y="635" class="node-sub">• Module 3: Dispenser &amp; Hatch</text>
  <text x="770" y="670" style="font-size: 11px; fill: #10b981; font-weight:600;">• Gravity Reservoir</text>
  <text x="770" y="690" style="font-size: 11px; fill: #10b981; font-weight:600;">• Anti-Double-Feed Pocket</text>

  <!-- Hardware Interconnects -->
  <path d="M 360 655 L 430 655" stroke="#38bdf8" stroke-width="3" marker-end="url(#arrow)" />
  <text x="375" y="645" class="edge-label">UART</text>

  <path d="M 690 655 L 750 655" stroke="#10b981" stroke-width="3" marker-end="url(#arrow)" />
  <text x="705" y="645" class="edge-label">PWM</text>

  <!-- HTTPS Sync arrow S3 -> Netlify API -->
  <path d="M 215 525 L 215 360 L 520 360 L 520 270" fill="none" stroke="#38bdf8" stroke-width="2" stroke-dasharray="6" marker-end="url(#arrow)" />
  <text x="225" y="380" class="edge-label">HTTPS GET /api/sync &amp; POST /api/notify</text>
</svg>"""

with open(os.path.join(docs_dir, "1_system_architecture.svg"), "w", encoding="utf-8") as f:
    f.write(svg_1)

# ---------------------------------------------------------------------------
# 2. Microcontroller Interconnects SVG
# ---------------------------------------------------------------------------
svg_2 = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 900 450" width="100%" height="100%" style="background:#0f172a; font-family: system-ui, -apple-system, sans-serif;">
  <defs>
    <linearGradient id="s3Grad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#1e3a8a" />
      <stop offset="100%" stop-color="#1e293b" />
    </linearGradient>
    <linearGradient id="c3Grad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#155e75" />
      <stop offset="100%" stop-color="#1e293b" />
    </linearGradient>
    <marker id="arrCyan" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#38bdf8" />
    </marker>
    <marker id="arrGreen" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#34d399" />
    </marker>
  </defs>

  <style>
    .title { font-size: 22px; font-weight: 700; fill: #f8fafc; }
    .sub { font-size: 13px; fill: #94a3b8; }
    .mcu-title { font-size: 18px; font-weight: 700; fill: #f1f5f9; }
    .pin { font-size: 13px; font-weight: 600; fill: #fbbf24; }
    .desc { font-size: 12px; fill: #cbd5e1; }
  </style>

  <text x="40" y="45" class="title">MedBox — Microcontroller Interconnects (UART)</text>
  <text x="40" y="70" class="sub">Hardware Serial Communication between Main Brain (ESP32-S3) and Motion Controller (ESP32-C3)</text>

  <!-- ESP32-S3 Card -->
  <rect x="50" y="110" width="340" height="280" rx="16" fill="url(#s3Grad)" stroke="#3b82f6" stroke-width="2" />
  <text x="80" y="150" class="mcu-title">🧠 ESP32-S3 SuperMini</text>
  <text x="80" y="175" style="font-size: 13px; fill: #60a5fa; font-weight:600;">System Brain &amp; Cloud Gateway</text>

  <text x="80" y="215" class="desc">• Wi-Fi &amp; NTP Timekeeping</text>
  <text x="80" y="240" class="desc">• Schedule Evaluation Logic</text>
  <text x="80" y="265" class="desc">• Proximity &amp; Piezo Control</text>

  <rect x="250" y="300" width="120" height="35" rx="6" fill="#1e293b" stroke="#fbbf24" stroke-width="1.5" />
  <text x="265" y="322" class="pin">TX: GPIO 17</text>

  <rect x="250" y="345" width="120" height="35" rx="6" fill="#1e293b" stroke="#fbbf24" stroke-width="1.5" />
  <text x="265" y="367" class="pin">RX: GPIO 18</text>

  <!-- ESP32-C3 Card -->
  <rect x="510" y="110" width="340" height="280" rx="16" fill="url(#c3Grad)" stroke="#06b6d4" stroke-width="2" />
  <text x="540" y="150" class="mcu-title">⚙️ ESP32-C3 SuperMini</text>
  <text x="540" y="175" style="font-size: 13px; fill: #22d3ee; font-weight:600;">Dedicated Servo Motion Controller</text>

  <text x="540" y="215" class="desc">• Single-Byte Command Parser</text>
  <text x="540" y="240" class="desc">• 6 PWM Servo Channels</text>
  <text x="540" y="265" class="desc">• Dispenser Rotor &amp; Hatch Opener</text>

  <rect x="530" y="300" width="120" height="35" rx="6" fill="#1e293b" stroke="#fbbf24" stroke-width="1.5" />
  <text x="545" y="322" class="pin">RX: GPIO 2</text>

  <rect x="530" y="345" width="120" height="35" rx="6" fill="#1e293b" stroke="#fbbf24" stroke-width="1.5" />
  <text x="545" y="367" class="pin">TX: GPIO 3</text>

  <!-- Arrows -->
  <path d="M 370 317 L 530 317" stroke="#38bdf8" stroke-width="3" marker-end="url(#arrCyan)" />
  <text x="405" y="310" style="font-size:11px; fill:#38bdf8; font-weight:600;">CMD_OPEN / CMD_DISPENSE (115200 Baud)</text>

  <path d="M 530 362 L 370 362" stroke="#34d399" stroke-width="3" marker-end="url(#arrGreen)" />
  <text x="415" y="380" style="font-size:11px; fill:#34d399; font-weight:600;">ACK / Status Response</text>
</svg>"""

with open(os.path.join(docs_dir, "2_microcontroller_interconnects.svg"), "w", encoding="utf-8") as f:
    f.write(svg_2)

# ---------------------------------------------------------------------------
# 3. Electrical Power Topology SVG
# ---------------------------------------------------------------------------
svg_3 = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 900 450" width="100%" height="100%" style="background:#0f172a; font-family: system-ui, -apple-system, sans-serif;">
  <defs>
    <linearGradient id="pwrGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#854d0e" />
      <stop offset="100%" stop-color="#1e293b" />
    </linearGradient>
    <marker id="arrAmber" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#f59e0b" />
    </marker>
  </defs>

  <style>
    .title { font-size: 22px; font-weight: 700; fill: #f8fafc; }
    .sub { font-size: 13px; fill: #94a3b8; }
    .box-title { font-size: 15px; font-weight: 700; fill: #fef08a; }
    .box-sub { font-size: 12px; fill: #cbd5e1; }
  </style>

  <text x="40" y="45" class="title">MedBox — Electrical Power Topology</text>
  <text x="40" y="70" class="sub">Dual Power Rail Isolation (Logic 3.3V vs High-Current 5V Servo Rail)</text>

  <!-- Power Source -->
  <rect x="50" y="150" width="180" height="140" rx="12" fill="url(#pwrGrad)" stroke="#eab308" stroke-width="2" />
  <text x="70" y="190" class="box-title">🔌 USB-C Input</text>
  <text x="70" y="215" class="box-sub">5V DC / 2A–3A</text>
  <text x="70" y="240" class="box-sub">Main Power Input</text>

  <!-- Fuse / Protection -->
  <rect x="280" y="150" width="160" height="140" rx="12" fill="#1e293b" stroke="#f59e0b" stroke-width="1.5" />
  <text x="300" y="190" class="box-title">🛡️ Protection</text>
  <text x="300" y="215" class="box-sub">• Reverse Polarity</text>
  <text x="300" y="240" class="box-sub">• Overcurrent Fuse</text>
  <text x="300" y="265" class="box-sub">• Bulk Capacitance</text>

  <!-- Logic Branch -->
  <rect x="520" y="110" width="330" height="100" rx="12" fill="#1e3a8a" stroke="#3b82f6" stroke-width="1.5" />
  <text x="540" y="145" class="box-title" style="fill:#93c5fd;">💻 3.3V Logic Rail (LDO Regulated)</text>
  <text x="540" y="170" class="box-sub">• Powers ESP32-S3, ESP32-C3 &amp; IR Proximity Sensor</text>

  <!-- Servo Bus Branch -->
  <rect x="520" y="240" width="330" height="100" rx="12" fill="#065f46" stroke="#10b981" stroke-width="1.5" />
  <text x="540" y="275" class="box-title" style="fill:#6ee7b7;">⚡ 5V Dedicated Servo Power Bus</text>
  <text x="540" y="300" class="box-sub">• Direct 5V power to 6 Servos (Prevents Microcontroller Reset)</text>

  <!-- Common GND Bar -->
  <rect x="50" y="370" width="800" height="35" rx="6" fill="#334155" stroke="#94a3b8" stroke-width="1.5" />
  <text x="340" y="393" style="font-size:14px; font-weight:700; fill:#f8fafc;">⏚ Shared Reference Ground (Common GND)</text>

  <!-- Lines -->
  <path d="M 230 220 L 280 220" stroke="#f59e0b" stroke-width="3" marker-end="url(#arrAmber)" />
  <path d="M 440 220 L 480 220 L 480 160 L 520 160" fill="none" stroke="#3b82f6" stroke-width="2.5" marker-end="url(#arrAmber)" />
  <path d="M 480 220 L 480 290 L 520 290" fill="none" stroke="#10b981" stroke-width="2.5" marker-end="url(#arrAmber)" />
</svg>"""

with open(os.path.join(docs_dir, "3_electrical_power_topology.svg"), "w", encoding="utf-8") as f:
    f.write(svg_3)

# ---------------------------------------------------------------------------
# 4. Firmware State Machine SVG
# ---------------------------------------------------------------------------
svg_4 = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 600" width="100%" height="100%" style="background:#0f172a; font-family: system-ui, -apple-system, sans-serif;">
  <defs>
    <marker id="arrBlue" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#38bdf8" />
    </marker>
    <marker id="arrRed" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#f87171" />
    </marker>
  </defs>

  <style>
    .title { font-size: 22px; font-weight: 700; fill: #f8fafc; }
    .sub { font-size: 13px; fill: #94a3b8; }
    .state-box { font-size: 14px; font-weight: 700; fill: #f1f5f9; }
    .state-desc { font-size: 11px; fill: #94a3b8; }
    .label { font-size: 11px; font-weight: 600; fill: #38bdf8; }
    .err-label { font-size: 11px; font-weight: 600; fill: #f87171; }
  </style>

  <text x="40" y="45" class="title">MedBox — Firmware State Machine Flow</text>
  <text x="40" y="70" class="sub">ESP32-S3 State Machine Logic (Proximity Safety &amp; Dispensing Pipeline)</text>

  <!-- State Nodes -->
  <!-- IDLE -->
  <rect x="50" y="250" width="140" height="70" rx="10" fill="#1e293b" stroke="#3b82f6" stroke-width="2" />
  <text x="100" y="280" class="state-box" text-anchor="middle">IDLE</text>
  <text x="100" y="300" class="state-desc" text-anchor="middle">Wait schedule match</text>

  <!-- REMINDER -->
  <rect x="260" y="250" width="160" height="70" rx="10" fill="#1e3a8a" stroke="#60a5fa" stroke-width="2" />
  <text x="340" y="280" class="state-box" text-anchor="middle">REMINDER</text>
  <text x="340" y="300" class="state-desc" text-anchor="middle">Buzz + ntfy Push Alert</text>

  <!-- WAITING_FOR_USER -->
  <rect x="490" y="250" width="180" height="70" rx="10" fill="#581c87" stroke="#c084fc" stroke-width="2" />
  <text x="580" y="280" class="state-box" text-anchor="middle">WAITING_FOR_USER</text>
  <text x="580" y="300" class="state-desc" text-anchor="middle">Poll IR Proximity Sensor</text>

  <!-- HATCH_OPENING -->
  <rect x="740" y="130" width="180" height="65" rx="10" fill="#155e75" stroke="#22d3ee" stroke-width="2" />
  <text x="830" y="160" class="state-box" text-anchor="middle">HATCH_OPENING</text>
  <text x="830" y="180" class="state-desc" text-anchor="middle">Send CMD_OPEN to C3</text>

  <!-- DISPENSING -->
  <rect x="740" y="250" width="180" height="65" rx="10" fill="#065f46" stroke="#34d399" stroke-width="2" />
  <text x="830" y="280" class="state-box" text-anchor="middle">DISPENSING</text>
  <text x="830" y="300" class="state-desc" text-anchor="middle">Send CMD_DISPENSE</text>

  <!-- USER_TAKING -->
  <rect x="740" y="370" width="180" height="65" rx="10" fill="#155e75" stroke="#22d3ee" stroke-width="2" />
  <text x="830" y="400" class="state-box" text-anchor="middle">USER_TAKING</text>
  <text x="830" y="420" class="state-desc" text-anchor="middle">Wait 15s hatch window</text>

  <!-- HATCH_CLOSING & COMPLETED -->
  <rect x="490" y="370" width="180" height="65" rx="10" fill="#1e293b" stroke="#10b981" stroke-width="2" />
  <text x="580" y="400" class="state-box" text-anchor="middle">HATCH_CLOSING</text>
  <text x="580" y="420" class="state-desc" text-anchor="middle">CMD_CLOSE &amp; Log to DB</text>

  <!-- MISSED DOSE -->
  <rect x="490" y="130" width="180" height="65" rx="10" fill="#7f1d1d" stroke="#f87171" stroke-width="2" />
  <text x="580" y="160" class="state-box" text-anchor="middle">MISSED_DOSE</text>
  <text x="580" y="180" class="state-desc" text-anchor="middle">Timeout 1h -> Log missed</text>

  <!-- Connectors -->
  <path d="M 190 285 L 260 285" stroke="#38bdf8" stroke-width="2" marker-end="url(#arrBlue)" />
  <path d="M 420 285 L 490 285" stroke="#38bdf8" stroke-width="2" marker-end="url(#arrBlue)" />
  
  <!-- Proximity True -->
  <path d="M 670 270 L 740 162" stroke="#34d399" stroke-width="2" marker-end="url(#arrBlue)" />
  <text x="670" y="210" class="label" fill="#34d399">User Present</text>

  <!-- Proximity Timeout -->
  <path d="M 580 250 L 580 195" stroke="#f87171" stroke-width="2" marker-end="url(#arrRed)" />
  <text x="590" y="230" class="err-label">Max Retries</text>

  <path d="M 830 195 L 830 250" stroke="#38bdf8" stroke-width="2" marker-end="url(#arrBlue)" />
  <path d="M 830 315 L 830 370" stroke="#38bdf8" stroke-width="2" marker-end="url(#arrBlue)" />
  <path d="M 740 402 L 670 402" stroke="#38bdf8" stroke-width="2" marker-end="url(#arrBlue)" />
  <path d="M 490 402 L 120 402 L 120 320" stroke="#38bdf8" stroke-width="2" marker-end="url(#arrBlue)" fill="none" />
  <path d="M 490 162 L 120 162 L 120 250" stroke="#f87171" stroke-width="2" marker-end="url(#arrRed)" fill="none" />
</svg>"""

with open(os.path.join(docs_dir, "4_firmware_state_machine.svg"), "w", encoding="utf-8") as f:
    f.write(svg_4)

# ---------------------------------------------------------------------------
# 5. End-to-End Sequence Diagram SVG
# ---------------------------------------------------------------------------
svg_5 = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1100 780" width="100%" height="100%" style="background:#0f172a; font-family: system-ui, -apple-system, sans-serif;">
  <defs>
    <marker id="arr1" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#38bdf8" />
    </marker>
    <marker id="arrPurple1" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#c084fc" />
    </marker>
  </defs>

  <style>
    .title { font-size: 22px; font-weight: 700; fill: #f8fafc; }
    .sub { font-size: 13px; fill: #94a3b8; }
    .actor { font-size: 14px; font-weight: 700; fill: #f1f5f9; }
    .msg { font-size: 12px; font-weight: 500; fill: #cbd5e1; }
    .num { font-size: 11px; font-weight: 700; fill: #0f172a; }
  </style>

  <text x="40" y="40" class="title">MedBox — End-to-End Sequence Diagram</text>
  <text x="40" y="65" class="sub">Complete Step-by-Step Data &amp; Execution Pipeline across Web, Cloud, ESP32, Servos, and Phone</text>

  <!-- Lifelines -->
  <!-- 1. Patient / Caregiver (X=100) -->
  <!-- 2. Web Dashboard (X=300) -->
  <!-- 3. Netlify Backend (X=500) -->
  <!-- 4. ESP32-S3 Main (X=700) -->
  <!-- 5. ESP32-C3 Motion (X=900) -->
  <!-- 6. Patient Phone (X=1020) -->

  <g stroke="#334155" stroke-width="1.5" stroke-dasharray="4">
    <line x1="100" y1="120" x2="100" y2="720" />
    <line x1="300" y1="120" x2="300" y2="720" />
    <line x1="500" y1="120" x2="500" y2="720" />
    <line x1="700" y1="120" x2="700" y2="720" />
    <line x1="900" y1="120" x2="900" y2="720" />
    <line x1="1020" y1="120" x2="1020" y2="720" />
  </g>

  <!-- Actor Boxes -->
  <rect x="30" y="90" width="140" height="35" rx="6" fill="#1e293b" stroke="#38bdf8" stroke-width="1.5" />
  <text x="100" y="113" class="actor" text-anchor="middle">👤 Patient</text>

  <rect x="230" y="90" width="140" height="35" rx="6" fill="#1e3a8a" stroke="#3b82f6" stroke-width="1.5" />
  <text x="300" y="113" class="actor" text-anchor="middle">💻 Web UI</text>

  <rect x="430" y="90" width="140" height="35" rx="6" fill="#1e3a8a" stroke="#06b6d4" stroke-width="1.5" />
  <text x="500" y="113" class="actor" text-anchor="middle">☁️ Netlify API</text>

  <rect x="630" y="90" width="140" height="35" rx="6" fill="#155e75" stroke="#22d3ee" stroke-width="1.5" />
  <text x="700" y="113" class="actor" text-anchor="middle">🧠 ESP32-S3</text>

  <rect x="830" y="90" width="140" height="35" rx="6" fill="#065f46" stroke="#10b981" stroke-width="1.5" />
  <text x="900" y="113" class="actor" text-anchor="middle">⚙️ ESP32-C3</text>

  <rect x="960" y="90" width="120" height="35" rx="6" fill="#581c87" stroke="#c084fc" stroke-width="1.5" />
  <text x="1020" y="113" class="actor" text-anchor="middle">📱 Phone</text>

  <!-- Sequence Steps -->
  <!-- Step 1: Web UI Schedule creation -->
  <path d="M 100 160 L 300 160" stroke="#38bdf8" stroke-width="2" marker-end="url(#arr1)" />
  <text x="200" y="152" class="msg" text-anchor="middle">1. Add Medication Schedule</text>

  <path d="M 300 190 L 500 190" stroke="#38bdf8" stroke-width="2" marker-end="url(#arr1)" />
  <text x="400" y="182" class="msg" text-anchor="middle">2. POST /api/medications</text>

  <!-- Step 2: ESP32 Periodic Sync -->
  <path d="M 700 240 L 500 240" stroke="#38bdf8" stroke-width="2" marker-end="url(#arr1)" />
  <text x="600" y="232" class="msg" text-anchor="middle">3. GET /api/sync (Every 5 min)</text>

  <path d="M 500 270 L 700 270" stroke="#34d399" stroke-width="2" stroke-dasharray="4" marker-end="url(#arr1)" />
  <text x="600" y="262" class="msg" text-anchor="middle">4. Return JSON Schedules (Save NVS)</text>

  <!-- Step 3: Alarm Reached -->
  <path d="M 700 320 L 500 320" stroke="#c084fc" stroke-width="2" marker-end="url(#arrPurple1)" />
  <text x="600" y="312" class="msg" text-anchor="middle">5. POST /api/notify (Reminder)</text>

  <path d="M 500 350 L 1020 350" stroke="#c084fc" stroke-width="2.5" marker-end="url(#arrPurple1)" />
  <text x="760" y="342" class="msg" text-anchor="middle" fill="#c084fc">6. Push Phone Alert ("💊 Medication Reminder")</text>

  <!-- Step 4: Proximity Detection -->
  <path d="M 100 410 L 700 410" stroke="#fbbf24" stroke-width="2" marker-end="url(#arr1)" />
  <text x="400" y="402" class="msg" text-anchor="middle" fill="#fbbf24">7. Patient walks up to MedBox (IR Sensor Detected)</text>

  <!-- Step 5: Motor Dispense -->
  <path d="M 700 470 L 900 470" stroke="#38bdf8" stroke-width="2" marker-end="url(#arr1)" />
  <text x="800" y="462" class="msg" text-anchor="middle">8. UART CMD_OPEN (Hatch Opens)</text>

  <path d="M 700 520 L 900 520" stroke="#38bdf8" stroke-width="2" marker-end="url(#arr1)" />
  <text x="800" y="512" class="msg" text-anchor="middle">9. UART CMD_DISPENSE (Rotor Steps Pill Drop)</text>

  <!-- Step 6: Patient takes pill & Hatch Close -->
  <path d="M 700 580 L 100 580" stroke="#34d399" stroke-width="2" stroke-dasharray="4" marker-end="url(#arr1)" />
  <text x="400" y="572" class="msg" text-anchor="middle" fill="#34d399">10. Patient retrieves pill (15s Hatch Open Window)</text>

  <path d="M 700 630 L 900 630" stroke="#38bdf8" stroke-width="2" marker-end="url(#arr1)" />
  <text x="800" y="622" class="msg" text-anchor="middle">11. UART CMD_CLOSE (Hatch Closes)</text>

  <!-- Step 7: Cloud Logging -->
  <path d="M 700 680 L 500 680" stroke="#38bdf8" stroke-width="2" marker-end="url(#arr1)" />
  <text x="600" y="672" class="msg" text-anchor="middle">12. POST /api/dispense-log ("dispensed")</text>
</svg>"""

with open(os.path.join(docs_dir, "5_end_to_end_sequence.svg"), "w", encoding="utf-8") as f:
    f.write(svg_5)

# ---------------------------------------------------------------------------
# 6. Mechanical Dispensing Concept SVG
# ---------------------------------------------------------------------------
svg_6 = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 800 500" width="100%" height="100%" style="background:#0f172a; font-family: system-ui, -apple-system, sans-serif;">
  <defs>
    <linearGradient id="mechGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#1e293b" />
      <stop offset="100%" stop-color="#0f172a" />
    </linearGradient>
    <marker id="arrGreen1" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#10b981" />
    </marker>
  </defs>

  <style>
    .title { font-size: 22px; font-weight: 700; fill: #f8fafc; }
    .sub { font-size: 13px; fill: #94a3b8; }
    .mech-title { font-size: 16px; font-weight: 700; fill: #34d399; }
    .mech-desc { font-size: 12px; fill: #cbd5e1; }
  </style>

  <text x="40" y="45" class="title">MedBox — Mechanical Dispensing Mechanism</text>
  <text x="40" y="70" class="sub">Gravity-Fed Storage Cylinder + Anti-Double-Feed Rotary Metering Rotor</text>

  <!-- Step 1: Cylinder -->
  <rect x="50" y="120" width="200" height="320" rx="12" fill="url(#mechGrad)" stroke="#10b981" stroke-width="2" />
  <text x="70" y="160" class="mech-title">1. Cylinder Reservoir</text>
  <text x="70" y="190" class="mech-desc">• Stores pills vertically</text>
  <text x="70" y="210" class="mech-desc">• Pure gravity feed</text>
  <text x="70" y="230" class="mech-desc">• No mechanical augers</text>
  <!-- Pills -->
  <circle cx="150" cy="280" r="14" fill="#38bdf8" />
  <circle cx="150" cy="320" r="14" fill="#38bdf8" />
  <circle cx="150" cy="360" r="14" fill="#38bdf8" />
  <circle cx="150" cy="400" r="14" fill="#38bdf8" />

  <!-- Arrow -->
  <path d="M 250 280 L 300 280" stroke="#10b981" stroke-width="3" marker-end="url(#arrGreen1)" />

  <!-- Step 2: Rotor -->
  <rect x="300" y="120" width="220" height="320" rx="12" fill="url(#mechGrad)" stroke="#06b6d4" stroke-width="2" />
  <text x="320" y="160" class="mech-title" style="fill:#22d3ee;">2. Rotary Rotor</text>
  <text x="320" y="190" class="mech-desc">• Indexed Pocket Wheel</text>
  <text x="320" y="210" class="mech-desc">• 1 Step (15°) = 1 Pill</text>
  <text x="320" y="230" class="mech-desc">• Driven by Dispenser Servo</text>
  <text x="320" y="250" class="mech-desc">• Anti-double-feed wall</text>
  <!-- Wheel drawing -->
  <circle cx="410" cy="340" r="60" fill="#1e293b" stroke="#06b6d4" stroke-width="3" />
  <circle cx="410" cy="300" r="10" fill="#38bdf8" />

  <!-- Arrow -->
  <path d="M 520 280 L 570 280" stroke="#10b981" stroke-width="3" marker-end="url(#arrGreen1)" />

  <!-- Step 3: Hatch & Pickup -->
  <rect x="570" y="120" width="180" height="320" rx="12" fill="url(#mechGrad)" stroke="#3b82f6" stroke-width="2" />
  <text x="590" y="160" class="mech-title" style="fill:#60a5fa;">3. Hatch Pickup</text>
  <text x="590" y="190" class="mech-desc">• Drop Chute</text>
  <text x="590" y="210" class="mech-desc">• Hatch Servo Opener</text>
  <text x="590" y="230" class="mech-desc">• Opens on Proximity</text>
  <text x="590" y="250" class="mech-desc">• Auto-closes in 15s</text>
  <!-- Hatch box -->
  <rect x="610" y="300" width="100" height="80" rx="8" fill="#1e3a8a" stroke="#3b82f6" stroke-width="2" />
  <circle cx="660" cy="350" r="14" fill="#38bdf8" />
</svg>"""

with open(os.path.join(docs_dir, "6_mechanical_dispensing.svg"), "w", encoding="utf-8") as f:
    f.write(svg_6)

print("All 6 SVG diagrams generated successfully!")
