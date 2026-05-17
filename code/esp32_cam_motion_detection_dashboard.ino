#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// ===============================
// WIFI SETTINGS
// ===============================
const char* ssid = "YOUR WIFI";
const char* password = "YOUR WIFI PASSWORD";

// Web server runs on port 80
WebServer server(80);

// ===============================
// AI-THINKER ESP32-CAM PIN SETUP
// ===============================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===============================
// MOTION DETECTION SETTINGS
// ===============================

// Lower number = more sensitive
// Higher number = less sensitive
int motionThreshold = 20;

// How often motion is checked
int checkDelay = 1000; // 1 second

// Stores previous camera frame
uint8_t *previousFrame = NULL;
size_t previousFrameLength = 0;

// Dashboard values
int differenceScore = 0;
bool motionDetected = false;
int motionEventCount = 0;
String lastStatus = "Starting security monitor...";
unsigned long lastMotionTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("ESP32-CAM Software Motion Detection");
  Serial.println("Security Monitor Starting...");
  Serial.println("====================================");

  setupCamera();
  setupWiFi();
  setupWebServer();

  Serial.println("Security monitor is now active.");
}

void loop() {
  server.handleClient();
  checkForMotion();
  delay(checkDelay);
}

// ===============================
// CAMERA SETUP
// ===============================
void setupCamera() {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;

  // Grayscale makes frame comparison easier
  config.pixel_format = PIXFORMAT_GRAYSCALE;

  // Small frame size uses less memory
  config.frame_size = FRAMESIZE_QQVGA; // 160x120

  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    lastStatus = "Camera initialization failed.";
    return;
  }

  Serial.println("Camera initialized successfully.");
  lastStatus = "Camera initialized successfully.";
}

// ===============================
// WIFI SETUP
// ===============================
void setupWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected successfully.");
    Serial.print("Dashboard IP Address: ");
    Serial.println(WiFi.localIP());

    lastStatus = "Wi-Fi connected. Security monitor active.";
  } else {
    Serial.println("Wi-Fi connection failed.");
    lastStatus = "Wi-Fi connection failed. Motion detection still running on Serial Monitor.";
  }
}

// ===============================
// WEB SERVER SETUP
// ===============================
void setupWebServer() {
  server.on("/", handleDashboard);
  server.on("/status", handleStatus);

  server.begin();

  Serial.println("Web dashboard started.");
}

// ===============================
// MOTION DETECTION LOGIC
// ===============================
void checkForMotion() {
  camera_fb_t *currentFrame = esp_camera_fb_get();

  if (!currentFrame) {
    Serial.println("Failed to capture frame.");
    lastStatus = "Failed to capture camera frame.";
    return;
  }

  // First frame becomes the baseline
  if (previousFrame == NULL) {
    previousFrameLength = currentFrame->len;
    previousFrame = (uint8_t *)malloc(previousFrameLength);

    if (previousFrame == NULL) {
      Serial.println("Failed to allocate memory for previous frame.");
      lastStatus = "Memory allocation failed.";
      esp_camera_fb_return(currentFrame);
      return;
    }

    memcpy(previousFrame, currentFrame->buf, previousFrameLength);

    Serial.println("Baseline image captured.");
    lastStatus = "Baseline image captured. Monitoring for motion.";

    esp_camera_fb_return(currentFrame);
    return;
  }

  // Compare current frame to previous frame
  differenceScore = compareFrames(previousFrame, currentFrame->buf, previousFrameLength);

  Serial.print("Difference Score: ");
  Serial.println(differenceScore);

  if (differenceScore > motionThreshold) {
    motionDetected = true;
    motionEventCount++;
    lastMotionTime = millis();

    Serial.println("[ALERT] MOTION DETECTED!");
    Serial.println("Possible unauthorized physical access or movement detected.");

    lastStatus = "ALERT: Motion detected. Possible unauthorized physical access.";
  } else {
    motionDetected = false;

    Serial.println("No significant motion detected.");
    lastStatus = "No significant motion detected. Area appears clear.";
  }

  // Update baseline frame
  memcpy(previousFrame, currentFrame->buf, previousFrameLength);

  esp_camera_fb_return(currentFrame);
}

// ===============================
// FRAME COMPARISON FUNCTION
// ===============================
int compareFrames(uint8_t *frame1, uint8_t *frame2, size_t length) {
  long totalDifference = 0;

  // Compare every 10th pixel to save processing power
  for (size_t i = 0; i < length; i += 10) {
    int pixelDifference = abs(frame1[i] - frame2[i]);
    totalDifference += pixelDifference;
  }

  int averageDifference = totalDifference / (length / 10);

  return averageDifference;
}

// ===============================
// WEB DASHBOARD PAGE
// ===============================
void handleDashboard() {
  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='3'>";
  html += "<title>ESP32-CAM Security Monitor</title>";

  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #111; color: #eee; text-align: center; padding: 20px; }";
  html += ".container { max-width: 700px; margin: auto; background: #1c1c1c; padding: 25px; border-radius: 12px; }";
  html += "h1 { color: #00d4ff; }";
  html += ".status { font-size: 24px; font-weight: bold; padding: 15px; border-radius: 8px; margin: 20px 0; }";

  if (motionDetected) {
    html += ".statusBox { background-color: #8b0000; color: white; }";
  } else {
    html += ".statusBox { background-color: #006400; color: white; }";
  }

  html += ".data { background: #292929; margin: 10px 0; padding: 12px; border-radius: 8px; font-size: 18px; }";
  html += ".small { color: #aaa; font-size: 14px; margin-top: 20px; }";
  html += "</style>";

  html += "</head>";
  html += "<body>";
  html += "<div class='container'>";

  html += "<h1>ESP32-CAM Security Monitor</h1>";
  html += "<p>Software-Based Motion Detection Dashboard</p>";

  html += "<div class='status statusBox'>";

  if (motionDetected) {
    html += "MOTION DETECTED";
  } else {
    html += "NO MOTION DETECTED";
  }

  html += "</div>";

  html += "<div class='data'><strong>Status:</strong><br>" + lastStatus + "</div>";
  html += "<div class='data'><strong>Difference Score:</strong><br>" + String(differenceScore) + "</div>";
  html += "<div class='data'><strong>Motion Threshold:</strong><br>" + String(motionThreshold) + "</div>";
  html += "<div class='data'><strong>Motion Events Count:</strong><br>" + String(motionEventCount) + "</div>";
  html += "<div class='data'><strong>Device IP Address:</strong><br>" + WiFi.localIP().toString() + "</div>";
  html += "<div class='data'><strong>Uptime:</strong><br>" + String(millis() / 1000) + " seconds</div>";

  if (lastMotionTime > 0) {
    html += "<div class='data'><strong>Last Motion Event:</strong><br>";
    html += String((millis() - lastMotionTime) / 1000);
    html += " seconds ago</div>";
  } else {
    html += "<div class='data'><strong>Last Motion Event:</strong><br>No motion recorded yet</div>";
  }

  html += "<p class='small'>This page refreshes automatically every 3 seconds.</p>";
  html += "<p class='small'>Cybersecurity Function: Physical intrusion detection and monitoring using an IoT device.</p>";

  html += "</div>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

// ===============================
// SIMPLE STATUS ENDPOINT
// ===============================
void handleStatus() {
  String json = "{";
  json += "\"motionDetected\":";
  json += motionDetected ? "true" : "false";
  json += ",";
  json += "\"differenceScore\":";
  json += String(differenceScore);
  json += ",";
  json += "\"motionThreshold\":";
  json += String(motionThreshold);
  json += ",";
  json += "\"motionEventCount\":";
  json += String(motionEventCount);
  json += ",";
  json += "\"status\":\"";
  json += lastStatus;
  json += "\"";
  json += "}";

  server.send(200, "application/json", json);
}
