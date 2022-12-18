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
HardwareSerial MySerial(1); // UART1 RX:IO9 TX:IO10

// Identifier and password to connect to network
const char* ssid = "GL-DSH"; 
const char* password = "goodlife"; 


WebServer server(80); // Server on port 80

IPAddress clientIP (192, 168, 123, 20); // M5Stick IP address, it is static.

// Variables to handle the presence of the M5Stick
boolean clientPresent = false; 
unsigned long lastPingT;
uint8_t failedPingC = 0;


// Wake up PC by sending WOL datagram.
void handleWakeUp() {
  server.send(200, "text/plain", "OK");
  Serial.println("Handling Wake Up");
  WOL.sendMagicPacket("00:23:24:CD:8A:A6");
  clientPresent = true;
  lastPingT = millis();
}

// Tell Attiny to execute the login function
void handleLogin() {
  server.send(200, "text/plain", "OK");
  Serial.println("Handling Login");
  MySerial.write('1');
}

// Tell Attiny to execute the logout function
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

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Callable endpoints
  server.on("/wake-up", handleWakeUp);
  server.on("/login", handleLogin);
  server.on("/logout", handleLogout);

  server.onNotFound([](){
        server.send(404, "text/plain", "Action not found");

  });
  // Get the milliseconds elapsed
  lastPingT = millis();

  // Initialize server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  // Serve HTTP petitions
  server.handleClient();

  // Check the presence of the M5Stick to decide if the PC should be put to suspension state
  uint64_t actualTime = millis();
  if (clientPresent) { // Client presence
    if ((actualTime - lastPingT) >= 5000){ // If more than 5 seconds elapsed, check presence
      lastPingT = millis();
      if (Ping.ping(clientIP, 1)) { // If Ping to M5Stick is successful, notify it and reset failed Ping counter
        Serial.println("Client is present and PING OK");
        failedPingC = 0;
      }
      else { // Else, increment the counter of failed Pings and suspend PC if 2 consecutive fails occur
        failedPingC ++;
        Serial.printf("Client is present and PING FAILED for %dº time\n\r", failedPingC);
        clientPresent = failedPingC > 1 ? false : true;
        if (!clientPresent){
          MySerial.write('3'); //Send shutdown order to Digispark
          Serial.println("Client went out");
        }
      }
    }
  }
  else { // No presence
    if ((actualTime - lastPingT) >= 10000){ // If more than 10 seconds elapsed, check presence 
      lastPingT = millis();
      if (Ping.ping(clientIP, 1)) { // If Ping to M5Stick is successful, then the client is present
        Serial.print("Client was NOT present and PING OK -> Setting presence to true");
        failedPingC = 0;
        clientPresent = true;
      }
      else { // Else, notify it for debug purposes
        Serial.println("Client is NOT present and PING FAILED");
      }
    }
  }
  delay(2); // Allow the CPU to switch to other tasks
}
