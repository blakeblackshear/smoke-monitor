#include "config.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "ESP8266HTTPUpdateServer.h"
#include "PubSubClient.h"

#include "SmokeMonitor.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);
SmokeMonitor smokeMonitor(D1,D2,D6);

String create_webpage_html()
{
  String html = "<html><h1>Smoke</h2><p>";
  html += smokeMonitor.is_smoke_detected() ? "Yes" : "No";
  html += "</p>";
  html += "<h1>CO</h2><p>";
  html += smokeMonitor.is_co_detected() ? "Yes" : "No";
  html += "</p>";
  html += "<h1>Alarm</h2><p>";
  html += smokeMonitor.is_alarm_on() ? "Yes" : "No";
  html += "</p>";
  html += "<h1>Alarm</h2><p>";
  html += "<form action=\"/alarm/on\" method=\"post\" target=\"dummyframe\">";
  html += "<button type=\"submit\">On</button>";
  html += "</form>";
  html += "<form action=\"/alarm/off\" method=\"post\" target=\"dummyframe\">";
  html += "<button type=\"submit\">Off</button>";
  html += "</form>";
  html += "<iframe width=\"0\" height=\"0\" border=\"0\" name=\"dummyframe\" id=\"dummyframe\"></iframe>";
  html += "</html>";
  return html;
}

void setup_web_server()
{
  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", create_webpage_html());
  });
  server.on("/alarm/on", HTTP_POST, [](){
    smokeMonitor.alarm_on();
    server.send(200,"text/html", "ok");
  });
  server.on("/alarm/off", HTTP_POST, [](){
    smokeMonitor.alarm_off();
    server.send(200,"text/html", "ok");
  });
  server.begin();
}

void send_status() {
  client.publish(SMOKE_STATUS_TOPIC, smokeMonitor.is_smoke_detected() ? "ON" : "OFF", true);
  client.publish(CO_STATUS_TOPIC, smokeMonitor.is_co_detected() ? "ON" : "OFF", true);
  client.publish(ALARM_STATUS_TOPIC, smokeMonitor.is_alarm_on() ? "ON" : "OFF", true);
}

void handle_mqtt_message(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  if (strcmp(topic,ALARM_CONTROL_TOPIC) == 0){
    if (strcmp(message,"ON") == 0) {
      smokeMonitor.alarm_on();
    }
    else if (strcmp(message,"OFF") == 0) {
      smokeMonitor.alarm_off();
    }
  }
}

void reconnect_mqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("smoke_monitor", AVAILABILITY_TOPIC, 1, true, "offline")) {
      Serial.println("connected");
      client.subscribe(ALARM_CONTROL_TOPIC);
      client.publish(AVAILABILITY_TOPIC, "online", true);
      send_status();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // disable AP
  WiFi.mode(WIFI_STA);

  // Setup serial output
  Serial.begin(9600);
  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  setup_web_server();
  httpUpdater.setup(&server);

  Serial.println("Server started");
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Setup MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(handle_mqtt_message);
  reconnect_mqtt();

  smokeMonitor.on_change(send_status);
}

void reboot() // Restarts program from beginning but does not reset the peripherals and registers
{
  Serial.print("rebooting");
  ESP.reset(); 
}

void loop()
{
  server.handleClient();
  smokeMonitor.update();
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();
}