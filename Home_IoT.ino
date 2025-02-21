#include <WiFi.h>
#include <WiFiClientSecure.h> // Required for MQTT over SSL
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "The_Rise";
const char* password = "OfficialRise@#998";

// MQTT broker details
const char* mqtt_server = "7c5205f9eebc472087ef39b2f7e42387.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // SSL secured port
const char* mqtt_user = "iot_water";
const char* mqtt_password = "IoTwater@25";

// MQTT topics
const char* sensor_topic = "home/sensor"; // Publish sensor data here
const char* led_topic = "home/led";       // Subscribe to this topic for LED control

// Corrected Pins for ESP32-S3
const int potPin = 34;  // ESP32-S3 ADC pin (Valid choices: 32, 33, 34, 35, 36, 39)
const int ledPin = 27;   // GPIO for LED output

// Secure WiFi and MQTT clients
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Callback function to handle messages from MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  if (String(topic) == led_topic) {
    if (message == "ON") {
      digitalWrite(ledPin, HIGH);
      Serial.println("ON");
    } else if (message == "OFF") {
      digitalWrite(ledPin, LOW);
      Serial.println("OFF");
    }
  }
}

void setup() {
  // Setup serial monitor
  Serial.begin(115200);

  // Setup WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Secure MQTT Configuration
  espClient.setInsecure(); // Bypass SSL certificate verification (Not recommended for production)
  
  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("Test_Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT!");
      client.subscribe(led_topic);
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Setup pins
  pinMode(ledPin, OUTPUT);
}

void loop() {
  client.loop(); // Keep MQTT connection alive

  // Read potentiometer value and publish it
  int sensorValue = analogRead(potPin);
  Serial.println(sensorValue);
  String sensorString = String(sensorValue);
  client.publish(sensor_topic, sensorString.c_str());

  delay(5000); // Publish every second
}
