/*
 * MedBox — Wi-Fi Manager
 * Captive portal provisioning + connection management.
 *
 * Adapted from the AiBot WifiProvisioning module.
 */

#include "wifi_manager.h"
#include "config.h"
#include "nvs_store.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// ── Internal state ──────────────────────────────────────────────────────────

static WifiState _state = WIFI_DISCONNECTED;
static DNSServer _dnsServer;
static AsyncWebServer _server(80);
static bool _apActive = false;
static bool _serverStarted = false;

// Pending connection from portal POST (processed in update(), not in async handler)
static String _pendingSsid;
static String _pendingPass;
static bool _pendingConnect = false;
static bool _pendingConnectResult = false;
static bool _pendingConnectDone = false;

// ── Forward declarations ────────────────────────────────────────────────────

static bool    _tryConnect(const String& ssid, const String& pass, unsigned long timeoutMs);
static void    _startAP();
static void    _stopAP();
static void    _setupRoutes();
static void    _setState(WifiState newState);
static String  _scanNetworksJson();

// ── Public API ──────────────────────────────────────────────────────────────

void wifiManagerInit() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  // Initialize LittleFS for serving captive portal assets
  if (!LittleFS.begin(true)) {  // true = format on first use
    Serial.println(F("[WIFI] LittleFS mount failed!"));
  }

  // --- Boot connection sequence ---

  // 1. Try NVS-saved credentials
  if (nvsHasWifiCreds()) {
    String ssid = nvsGetWifiSsid();
    String pass = nvsGetWifiPass();
    Serial.print(F("[WIFI] Trying saved network: "));
    Serial.println(ssid);
    _setState(WIFI_CONNECTING);

    if (_tryConnect(ssid, pass, WIFI_CONNECT_TIMEOUT_MS)) {
      Serial.println(F("[WIFI] Connected to saved network!"));
      _setState(WIFI_CONNECTED);
      return;
    }
    Serial.println(F("[WIFI] Saved network failed"));
  }

  // 2. Fall back to SoftAP + captive portal
  Serial.println(F("[WIFI] No saved credentials or connection failed — starting AP"));
  _startAP();
}

void wifiManagerUpdate() {
  // Process DNS in AP mode (required for captive portal redirect)
  if (_apActive) {
    _dnsServer.processNextRequest();
  }

  // Handle pending connection attempts (from portal POST)
  if (_pendingConnect) {
    _pendingConnect = false;
    _setState(WIFI_CONNECTING);

    // Keep AP running while trying STA connection
    WiFi.mode(WIFI_AP_STA);

    bool success = _tryConnect(_pendingSsid, _pendingPass, WIFI_CONNECT_TIMEOUT_MS);

    _pendingConnectResult = success;
    _pendingConnectDone = true;

    if (success) {
      Serial.print(F("[WIFI] Connected via portal to: "));
      Serial.println(_pendingSsid);
      nvsSaveWifi(_pendingSsid, _pendingPass);

      // Brief delay to let the status response reach the client
      delay(1000);
      _stopAP();
      _setState(WIFI_CONNECTED);
    } else {
      Serial.println(F("[WIFI] Portal connection attempt failed"));
      // Stay in AP mode
      WiFi.mode(WIFI_AP);
      WiFi.softAP(WIFI_AP_SSID);
      _setState(WIFI_AP_MODE);
    }
  }

  // Monitor connection health
  if (_state == WIFI_CONNECTED && WiFi.status() != WL_CONNECTED) {
    Serial.println(F("[WIFI] Connection lost, attempting reconnect..."));
    _setState(WIFI_CONNECTING);

    WiFi.reconnect();
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < WIFI_CONNECT_TIMEOUT_MS) {
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
      _setState(WIFI_CONNECTED);
    } else {
      Serial.println(F("[WIFI] Reconnect failed, entering AP mode"));
      _startAP();
    }
  }
}

bool wifiIsConnected() {
  return _state == WIFI_CONNECTED && WiFi.status() == WL_CONNECTED;
}

String wifiGetIP() {
  if (_state == WIFI_CONNECTED) {
    return WiFi.localIP().toString();
  }
  if (_state == WIFI_AP_MODE) {
    return WiFi.softAPIP().toString();
  }
  return "0.0.0.0";
}

WifiState wifiGetState() {
  return _state;
}

void wifiStartAP() {
  WiFi.disconnect(true);
  _startAP();
}

void wifiForgetNetwork() {
  nvsClearWifi();
  WiFi.disconnect(true);
  _startAP();
}

// ── Connection Attempt ──────────────────────────────────────────────────────

static bool _tryConnect(const String& ssid, const String& pass, unsigned long timeoutMs) {
  WiFi.disconnect(true);
  delay(100);
  WiFi.begin(ssid.c_str(), pass.c_str());

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(100);
  }

  return WiFi.status() == WL_CONNECTED;
}

// ── AP Mode ─────────────────────────────────────────────────────────────────

static void _startAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_SSID);

  IPAddress apIP = WiFi.softAPIP();
  Serial.print(F("[WIFI] AP started: "));
  Serial.print(WIFI_AP_SSID);
  Serial.print(F(" IP: "));
  Serial.println(apIP);

  // Start DNS server — redirect all domains to AP IP (captive portal)
  _dnsServer.start(53, "*", apIP);

  // Setup web server routes (only once)
  if (!_serverStarted) {
    _setupRoutes();
    _server.begin();
    _serverStarted = true;
    Serial.println(F("[WIFI] Captive portal web server started"));
  }

  _apActive = true;
  _setState(WIFI_AP_MODE);
}

static void _stopAP() {
  _dnsServer.stop();
  _apActive = false;
  WiFi.mode(WIFI_STA);
  Serial.println(F("[WIFI] AP stopped"));
}

// ── Web Server Routes ───────────────────────────────────────────────────────

static void _setupRoutes() {
  // Serve static files from LittleFS (data/ folder)
  _server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  // Network scan endpoint
  _server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = _scanNetworksJson();
    request->send(200, "application/json", json);
  });

  // Connect endpoint
  _server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    // Handled in body handler below
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // Parse JSON body
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data, len);

    if (err) {
      request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
      return;
    }

    String ssid = doc["ssid"] | "";
    String pass = doc["pass"] | "";

    if (ssid.length() == 0) {
      request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"SSID required\"}");
      return;
    }

    // Queue the connection attempt (don't block the async handler)
    _pendingSsid = ssid;
    _pendingPass = pass;
    _pendingConnect = true;
    _pendingConnectDone = false;

    request->send(200, "application/json", "{\"status\":\"connecting\"}");
  });

  // Status endpoint
  _server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["state"] = (int)_state;

    switch (_state) {
      case WIFI_CONNECTED:
        doc["status"] = "connected";
        doc["ip"] = WiFi.localIP().toString();
        doc["ssid"] = WiFi.SSID();
        break;
      case WIFI_CONNECTING:
        doc["status"] = "connecting";
        break;
      case WIFI_AP_MODE:
        doc["status"] = "ap_mode";
        break;
      default:
        doc["status"] = "disconnected";
        break;
    }

    if (_pendingConnectDone) {
      doc["connectResult"] = _pendingConnectResult ? "success" : "failed";
    }

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // Captive portal detection — redirect all unknown requests to /
  _server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/");
  });
}

// ── Network Scan ────────────────────────────────────────────────────────────

static String _scanNetworksJson() {
  int n = WiFi.scanNetworks(false, false, false, 300);  // Active scan, 300ms/channel

  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  for (int i = 0; i < n && i < 20; i++) {  // Cap at 20 networks
    JsonObject net = arr.add<JsonObject>();
    net["ssid"] = WiFi.SSID(i);
    net["rssi"] = WiFi.RSSI(i);
    net["enc"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
  }

  WiFi.scanDelete();

  String result;
  serializeJson(doc, result);
  return result;
}

// ── State Management ────────────────────────────────────────────────────────

static void _setState(WifiState newState) {
  if (newState != _state) {
    _state = newState;
  }
}
