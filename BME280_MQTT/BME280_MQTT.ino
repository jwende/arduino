/***************************************************************************
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>

#define SEALEVELPRESSURE_HPA (1013.25)

#ifndef STASSID
#define STASSID "VDSL"
#define STAPSK "xx"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiClient wifiHdlr;
MqttClient mqttClient(wifiHdlr);

const char brokerAddr[] = "192.168.2.203";     // Use the real broker ip address
String topic = "home/room";
Adafruit_BME280 bme; // I2C

unsigned long delayTime;

void setup() {
  Serial.begin(9600);

  if (! bme.begin(0x76, &Wire)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
  }
  
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("[MQTT]: Connecting to MQTT broker: ");
  Serial.println(brokerAddr);

  // Connect to MQTT broker
  if (!mqttClient.connect(brokerAddr, 1884)) {
    Serial.print("[MQTT]: MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    // The connection to the MQTT broker failed, Arduino is halted.
    while (1);
  }

  Serial.println("[MQTT]: Connection established");
  Serial.println();
    delayTime = 5000;
}


void loop() {
  // call poll() regularly to handle keep alive and library
  mqttClient.poll();
  printValues();
  delay(delayTime);
}


void printValues() {
    StaticJsonDocument<200> doc;
    doc["Temperature"] = bme.readTemperature(); 
    doc["Pressure"] = bme.readPressure() / 100.0F;
    doc["Humidity"] = bme.readHumidity();

    // Publish message
    mqttClient.beginMessage(topic+"/json");
    char output[128];
    serializeJson(doc, output);
    mqttClient.print(output);
    mqttClient.endMessage();
}
