#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32Ping.h> //https://github.com/marian-craciunescu/ESP32Ping

#include <WebServer.h>
#include <ESPmDNS.h>

#include <WiFiUdp.h>
WiFiUDP UDP;

#include <WakeOnLan.h>
WakeOnLan WOL(UDP); // Pass WiFiUDP class

#include <HardwareSerial.h>
HardwareSerial MySerial(1);

// Identifier and password to connect to network
const char* ssid = "GL-DSH"; //Insert real values
const char* password = "goodlife"; //Insert real values


WebServer server(80);

const int led = 13;

IPAddress clientIP (192, 168, 123, 20);
boolean clientPresent = false;
unsigned long lastPingT;
uint8_t failedPingC = 0;



void handleWakeUp() {
  server.send(200, "text/plain", "OK");
  Serial.println("Handling Wake Up");
  WOL.sendMagicPacket("00:23:24:CD:8A:A6");
  delay(1000);
  MySerial.write('3');
  clientPresent = true;
  lastPingT = millis();
}
void handleLogin() {
  server.send(200, "text/plain", "OK");
  Serial.println("Handling Login");
  MySerial.write('1');
}

void handleLogout() {
  server.send(200, "text/plain", "OK");
  Serial.println("Handling Logout");
  MySerial.write('2');
}

void setup(void) {
  MySerial.begin(9600);
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
  server.on("/login", handleLogin);
  server.on("/logout", handleLogout);

  server.onNotFound([](){
        server.send(404, "text/plain", "Action not found");

  });

  lastPingT = millis();

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  if (clientPresent) {
    uint64_t actualTime = millis();
    if ((actualTime - lastPingT) == 5000){
      //Do a ping
      if (Ping.ping(clientIP, 1)) {
        failedPingC = 0;
      }
      else { //Ping failed
        failedPingC ++;
        clientPresent = failedPingC > 1 ? false : true;
        if (!clientPresent) MySerial.write('4'); //Send shutdown order to Digispark
        Serial.println("Client went out");
      }
    }
  }
  else {
    uint64_t actualTime = millis();
    if ((actualTime - lastPingT) == 10000){
      //Do a ping
      if (Ping.ping(clientIP, 1)) {
        failedPingC = 0;
        clientPresent = true;
        Serial.println("Client is now present");
      }
    }
  }
  
  delay(2);//allow the cpu to switch to other tasks
}
