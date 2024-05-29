#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h> // Change to DHT library instead of DHT11

// Initialize the DHT sensor
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SENSOR_PIN 36 // Analog pin connected to the water level sensor



// Relay pump
const int RELAY_PIN = 35;

const char* ssid = "Deepthyka";
const char* password = "11122002";
const char* mqttServer = "broker.emqx.io"; // MQTT broker address
const int mqttPort = 1883; // MQTT broker port
const char* mqttUser = ""; // MQTT username (if required)
const char* mqttPassword = ""; // MQTT password (if required)

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(RELAY_PIN, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  client.setServer(mqttServer, mqttPort);
  dht.begin(); // Initialize DHT sensor
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    }
    else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  // Read DHT11 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\tHumidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Read water level
  int waterLevel = analogRead(SENSOR_PIN); // Read the analog value from the sensor
  // Map the analog range from 0-4095 to 0-100 (assuming 0 represents empty and 100 represents full)
  int mappedLevel = map(waterLevel, 0, 1023, 0, 100);
  Serial.print("Water Level: ");
  Serial.println(mappedLevel);


  // Control the pump based on water level
  if (mappedLevel <= 500) {
    digitalWrite(RELAY_PIN, HIGH); // Turn on pump
    delay(5000); // Run pump for 5 seconds
  } else {
    digitalWrite(RELAY_PIN, LOW); // Turn off pump
    delay(5000); // Wait for 5 seconds before checking again
  }

  // MQTT
  if (!client.connected()) {
    reconnect();
  }

  String data = String(temperature) + "," + String(humidity) + "," + String(mappedLevel);
  String topic = "Water"; // MQTT topic to publish to
  client.publish(topic.c_str(), data.c_str());

  Serial.println("Published to MQTT: " + data);
  delay(500); // Delay before next loop iteration
}
