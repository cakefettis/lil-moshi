#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "192.168.1.100";
const int mqtt_port = 1883;

const char* topic_notify  = "mochi/notify";
const char* topic_mood    = "mochi/mood";
const char* topic_command = "mochi/command";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TOUCH_PIN T7
#define LED_PIN 2
#define TOUCH_THRESHOLD 25

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastBlink = 0;
unsigned long blinkInterval = 3000;
bool eyesClosed = false;

String currentMood = "neutral";
String lastMessage = "";

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  setupDisplay();
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (millis() - lastBlink > blinkInterval) {
    blinkEyes();
    lastBlink = millis();
  }

  if (touchRead(TOUCH_PIN) < TOUCH_THRESHOLD) {
    showSurprised();
    delay(700);
    drawFace(currentMood);
  }
}

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting Mochi...");
  display.display();
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("MochiFace")) {
      client.subscribe(topic_notify);
      client.subscribe(topic_mood);
      client.subscribe(topic_command);
      drawFace(currentMood);
    } else delay(3000);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  if (strcmp(topic, topic_notify) == 0) {
    lastMessage = msg;
    showNotification(msg);
  } else if (strcmp(topic, topic_mood) == 0) {
    currentMood = msg;
    drawFace(msg);
  } else if (strcmp(topic, topic_command) == 0) {
    handleCommand(msg);
  }
}

void drawFace(String mood) {
  display.clearDisplay();
  int eyeY = 25;
  int eyeX1 = 45;
  int eyeX2 = 85;

  if (mood == "happy") {
    display.drawArc(64, 48, 15, 15, 20, 160, SSD1306_WHITE);
    display.fillCircle(eyeX1, eyeY, 3, SSD1306_WHITE);
    display.fillCircle(eyeX2, eyeY, 3, SSD1306_WHITE);
  } else if (mood == "sad") {
    display.drawArc(64, 60, 15, 15, 200, 340, SSD1306_WHITE);
    display.fillCircle(eyeX1, eyeY, 3, SSD1306_WHITE);
    display.fillCircle(eyeX2, eyeY, 3, SSD1306_WHITE);
  } else if (mood == "sleepy") {
    display.drawLine(eyeX1 - 5, eyeY, eyeX1 + 5, eyeY, SSD1306_WHITE);
    display.drawLine(eyeX2 - 5, eyeY, eyeX2 + 5, eyeY, SSD1306_WHITE);
  } else if (mood == "angry") {
    display.drawLine(eyeX1 - 5, eyeY - 3, eyeX1 + 3, eyeY + 3, SSD1306_WHITE);
    display.drawLine(eyeX2 - 3, eyeY + 3, eyeX2 + 5, eyeY - 3, SSD1306_WHITE);
    display.drawLine(54, 55, 74, 55, SSD1306_WHITE);
  } else {
    display.fillCircle(eyeX1, eyeY, 2, SSD1306_WHITE);
    display.fillCircle(eyeX2, eyeY, 2, SSD1306_WHITE);
    display.drawLine(54, 55, 74, 55, SSD1306_WHITE);
  }

  display.display();
}

void blinkEyes() {
  eyesClosed = !eyesClosed;
  display.clearDisplay();
  int eyeY = 25;
  int eyeX1 = 45;
  int eyeX2 = 85;

  if (eyesClosed) {
    display.drawLine(eyeX1 - 5, eyeY, eyeX1 + 5, eyeY, SSD1306_WHITE);
    display.drawLine(eyeX2 - 5, eyeY, eyeX2 + 5, eyeY, SSD1306_WHITE);
  } else {
    drawFace(currentMood);
  }
  display.display();
}

void showSurprised() {
  display.clearDisplay();
  display.drawCircle(64, 45, 5, SSD1306_WHITE);
  display.drawCircle(45, 25, 3, SSD1306_WHITE);
  display.drawCircle(85, 25, 3, SSD1306_WHITE);
  display.display();
}

void showNotification(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("New Msg:");
  display.setCursor(0, 12);
  display.println(msg.substring(0, 20));
  display.setCursor(0, 25);
  display.println(msg.substring(20, 40));
  display.display();
  delay(2000);
  drawFace(currentMood);
}

void handleCommand(String cmd) {
  if (cmd == "sleep") currentMood = "sleepy";
  else if (cmd == "wake") currentMood = "neutral";
  else if (cmd == "smile") currentMood = "happy";
  else if (cmd == "frown") currentMood = "sad";
  drawFace(currentMood);
}
