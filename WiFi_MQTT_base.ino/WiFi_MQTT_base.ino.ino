/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

#ifndef STASSID
#define STASSID "VDSL"
#define STAPSK "xx"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiClient wifiHdlr;
MqttClient mqttClient(wifiHdlr);

const char brokerAddr[] = "192.168.2.203";     // Use the real broker ip address
const char topic[] = "/home/room/temperature";

const long intTimer = 5000;               // Timer interval for publishing the message
int currentTemp = 25;                     // Simulated temperature value in °C
unsigned long prevTimer = 0;              // Variable used to keep track of the previous timer value
unsigned long actualTimer = 0;            // Variable used to keep track of the current timer value


void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
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
}

void loop()
{
  // call poll() regularly to handle keep alive and library
  mqttClient.poll();

  actualTimer = millis();

  // Check if the timer is expired
  if (actualTimer - prevTimer >= intTimer) {
    // save the last timestamp a message was sent
    prevTimer = actualTimer;

    // Post some debugging information on serial port
    Serial.print("[MQTT]: Sending message to topic: ");
    Serial.println(topic);
    Serial.println(currentTemp);

    // Publish message
    mqttClient.beginMessage(topic);
    mqttClient.print(currentTemp);
    mqttClient.endMessage();

    Serial.println();
  }
}
