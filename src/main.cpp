#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ================= WiFi =================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ================= MQTT =================
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_motion = "othman/home/security/motion";
const char* topic_temp   = "othman/home/environment/temperature";
const char* topic_alert  = "othman/home/emergency/alert";
const char* topic_status = "othman/home/system/status";

// ================= Pins =================
#define PIR_PIN     27
#define DHT_PIN     15
#define BUZZER_PIN  26
#define LED_PIN     25

// ================= Buzzer PWM Settings =================
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ    1000
#define BUZZER_RES     8

#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// ================= Objects =================
WiFiClient espClient;
PubSubClient client(espClient);

// ================= Settings =================
const float TEMP_THRESHOLD = 40.0;   // Alarm ON at 40°C or above

const unsigned long SENSOR_INTERVAL = 2000;
const unsigned long ALERT_COOLDOWN = 10000;
const unsigned long MQTT_RETRY_INTERVAL = 5000;
const unsigned long MOTION_ALARM_DURATION = 5000;

// ================= State Variables =================
unsigned long lastSensorRead = 0;
unsigned long lastMotionAlert = 0;
unsigned long lastTempAlert = 0;
unsigned long lastMqttRetry = 0;
unsigned long motionAlarmUntil = 0;

bool motionAlarmActive = false;
bool tempAlarmActive = false;

// ================= WiFi Setup =================
void setup_wifi() {
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT Reconnect =================
void reconnect_mqtt() {
  unsigned long now = millis();

  if (now - lastMqttRetry < MQTT_RETRY_INTERVAL) {
    return;
  }

  lastMqttRetry = now;

  if (!client.connected()) {
    Serial.print("Connecting to MQTT... ");

    String clientId = "ESP32-SmartHome-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish(topic_status, "Smart Home Emergency System started");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" | MQTT offline, local alarm still works");
    }
  }
}

// ================= Safe MQTT Publish =================
void mqtt_publish(const char* topic, const char* message) {
  if (client.connected()) {
    client.publish(topic, message);
  } else {
    Serial.print("MQTT offline, not sent: ");
    Serial.println(message);
  }
}

// ================= Alarm Control =================
void activate_alarm() {
  digitalWrite(LED_PIN, HIGH);
  ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQ);
}

void deactivate_alarm() {
  digitalWrite(LED_PIN, LOW);
  ledcWriteTone(BUZZER_CHANNEL, 0);
}

// ================= Setup =================
void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // ESP32 PWM setup for passive buzzer
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

  deactivate_alarm();

  dht.begin();
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);

  Serial.println("System Ready...");
}

// ================= Loop =================
void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  } else {
    client.loop();
  }

  unsigned long now = millis();

  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;

    int motionState = digitalRead(PIR_PIN);
    float temperature = dht.readTemperature();

    // ========== Motion Detection ==========
    if (motionState == HIGH) {
      motionAlarmActive = true;
      motionAlarmUntil = now + MOTION_ALARM_DURATION;

      if (now - lastMotionAlert >= ALERT_COOLDOWN) {
        Serial.println("Motion detected!");

        mqtt_publish(topic_motion, "Motion detected");
        mqtt_publish(topic_alert, "Emergency: Intruder motion detected");

        lastMotionAlert = now;
      }
    }

    if (motionAlarmActive && now >= motionAlarmUntil) {
      motionAlarmActive = false;
    }

    // ========== Temperature Detection ==========
    if (!isnan(temperature)) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");

      char tempMessage[50];
      snprintf(tempMessage, sizeof(tempMessage), "Temperature: %.2f C", temperature);
      mqtt_publish(topic_temp, tempMessage);

      if (temperature >= TEMP_THRESHOLD) {
        tempAlarmActive = true;

        if (now - lastTempAlert >= ALERT_COOLDOWN) {
          Serial.println("Overtemperature detected!");

          mqtt_publish(topic_alert, "Emergency: Overtemperature detected");

          lastTempAlert = now;
        }
      } else {
        tempAlarmActive = false;
      }
    } else {
      Serial.println("Failed to read from DHT22 sensor");
    }

    // ========== Local Alarm ==========
    if (motionAlarmActive || tempAlarmActive) {
      activate_alarm();
    } else {
      deactivate_alarm();
    }
  }
}