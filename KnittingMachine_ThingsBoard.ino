#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
Preferences prefs;

const char* token = "";
int runningSeconds = 0;
int downtimeSeconds = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin("Panam Group KB-2", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

Serial.print("WiFi Status: ");
Serial.println(WiFi.status());

Serial.print("IP Address: ");
Serial.println(WiFi.localIP());

Serial.print("WiFi Signal (RSSI): ");
Serial.println(WiFi.RSSI());

Serial.println("WiFi Test Complete!");

prefs.begin("machine", false);
runningSeconds = prefs.getInt("run", 0);
downtimeSeconds = prefs.getInt("down", 0);
}

void loop() {
const char* status = (random(1, 11) <= 8) ? "RUNNING" : "STOPPED";
if (strcmp(status, "RUNNING") == 0) runningSeconds += 5; else downtimeSeconds += 5;
float efficiency = (float)runningSeconds / (runningSeconds + downtimeSeconds) * 100;

  String json = "{\"temperature\":" + String(random(30, 41)) +
              ",\"speed\":" + String(random(100, 151)) +
              ",\"production_count\":" + String(random(500, 1000)) +
              ",\"machine_status\":\"" + String(status) + "\"" +
              ",\"running_seconds\":" + String(runningSeconds) +
              ",\"downtime_seconds\":" + String(downtimeSeconds) +
              ",\"running_efficiency\":" + String(efficiency, 1) + "}";

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, String("https://thingsboard.cloud/api/v1/") + token + "/telemetry");
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(json);
  Serial.println(code);
  http.end();
  prefs.putInt("run", runningSeconds);
  prefs.putInt("down", downtimeSeconds);
  delay(5000);
}