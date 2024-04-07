/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#ifndef STASSID
#define STASSID "ESPA"
#define STAPSK "12345678"
#endif

#define ONE_WIRE_BUS 14
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient client;
const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "192.168.4.1";
const uint16_t port = 80;
bool onCooldown = false;
void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network

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

  // Start sensor
  sensors.begin();
}

void loop() {

  sensors.requestTemperatures();
  Serial.print("Celsius temperature: ");
  float temp = sensors.getTempCByIndex(0);
  Serial.println(temp);
  if(temp > 21.0) {
    attemptSendSignal(false);
  }
  else {
    attemptSendSignal(true);
  }
  delay(5000);

  if (onCooldown) {
    delay(5000);
    onCooldown = false;
  }
}
void attemptSendSignal(bool turnOn) {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  if (!client.connect(host, port)) {
    const int time = 5000;
    Serial.print("connection failed...will try again in ");
    Serial.print(time / 1000);
    Serial.println(" seconds.");
    delay(time);
    return;
  }
  Serial.print("Connection successful - sending data to server: ");Serial.println(turnOn? "on":"off");
  if (client.connected()) {
    if (turnOn) {
      client.println("GET /H");
    } else {
      client.println("GET /L");
    }
    onCooldown=true;
  }

  closeConnection();
}
void closeConnection() {
  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}
