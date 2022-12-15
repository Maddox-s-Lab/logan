#include <WiFi.h>
#include <WiFiClient.h>

#include <WebServer.h>
#include <ESPmDNS.h>

#include <WiFiUdp.h>
WiFiUDP UDP;

#include <WakeOnLan.h>
WakeOnLan WOL(UDP); // Pass WiFiUDP class

#include <HardwareSerial.h>
HardwareSerial MySerial(1);

// Identifier and password to connect to network
const char* ssid = ""; //Insert real values
const char* password = ""; //Insert real values


WebServer server(80);

const int led = 13;

void handleWakeUp() {
  Serial.println("Handling Wake Up");
  WOL.sendMagicPacket("00:23:24:CD:8A:A6");
  delay(1000);
  MySerial.write("0");
}

void handleSuspend() {
  MySerial.write("1");
}

void setup(void) {
  MySerial.begin(9600);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

//  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/wake-up", handleWakeUp);
  server.on("/suspend", handleSuspend);

  server.onNotFound([](){
        server.send(404, "text/plain", "Action not found");

  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}
