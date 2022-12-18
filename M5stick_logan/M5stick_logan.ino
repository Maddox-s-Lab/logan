/**
 * LOGAN Sketch for M5StickC
 * This sketch is intended for conecting to a local AP and 
 * 
 * 
 * Author: Maddox's LAB
 * WEB: www.kiwirange.com
 */

/****** DEFINITIONS *******/
#define DEBUG 1

#define PROXIMITY_CODE 'A'


/****** MACROS *******/
#ifndef waituntil
  #define waituntil(expression) while(!(expression))
#endif //waituntil

#ifndef waituntildo
  #define waituntildo(expression1,expression2) while(!(expression1)) expression2
  //#define waituntildo(expression1,expression2,expression3) while(expression1 != expression2) expression3()
#endif //waituntildo

#define sprint(expression) Serial.print(expression)
#define sprintln(expression) Serial.println(expression)

#if DEBUG == 1
  #define dinit() Serial.begin(115200); Serial.flush(); delay(50);
  #define dprint(expression) Serial.println( expression )
  #define dwrite(expression) Serial.print( expression )
  #define dshow(expression) Serial.print("# "); Serial.print( #expression ); Serial.print( ": " ); Serial.println( expression )
#else
  #define dprint(expression) 
  #define dshow(expression)
  #define dinit()
#endif
  


/****** INCLUDES *******/
//#include <Arduino.h>

// M5Stick-C Hardware Library
#include <M5StickC.h>

// Wifi Libraries
#include <WiFi.h>

// HTTP Client Library
#include <HTTPClient.h>



/****** GLOBAL VARIABLES *******/
//Your IP address or domain name with URL path
const PROGMEM char* serverWakeup = "http://192.168.123.10/wake-up";
const PROGMEM char* serverUnlock = "http://192.168.123.10/login";
const PROGMEM char* serverLock = "http://192.168.123.10/logout";

const PROGMEM char* wifiSSID = "GL-DSH";
const PROGMEM char* wifiPASS = "goodlife";

unsigned long lastPresenceTime = 0;
uint8_t lastPresence = 0;

uint8_t state;

/****** STATIC INSTANCES *******/






/****** LOCAL FUNCTIONS *******/
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}







void sendUnlock() {
  String unlockResponse = ""; //Variable for storing server response
  dprint("send UNLOCK");
  while (unlockResponse != "OK") {
    unlockResponse = httpGETRequest(serverUnlock);
    delay(2000);
  }
}



void sendLock () {
  String lockResponse = ""; //Variable for storing server response
  dprint("send LOCK");
  while (lockResponse != "OK") {
    lockResponse = httpGETRequest(serverLock);
    delay(2000);
  }
}




void setup() {
  // Initialize debug info
  //Serial.begin(115200);
  dinit();
  dprint();

  M5.begin(true, true, false); //Initialize M5 Library with LCD and AXP but w/o Serial
  
  // Setup LCD (80x160) and show Splashscreen for 4s
  M5.Axp.ScreenBreath(8); //Set the bright of the display using the AXP
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(20,24);
  M5.Lcd.print("LOGAN");
  delay(4000); 


  
  
//  M5.Lcd.fillScreen(TFT_BLACK);
//  M5.Lcd.setTextSize(1);
//  M5.Lcd.drawCentreString("WATCH INVADERS",80,15,2);
//  M5.Lcd.drawCentreString("PUSH[A] START TO BEAT!",80,50,1);
//  M5.Lcd.drawCentreString("PUSH[B] ADJUST BY NTP ",80,60,1);
//  M5.Lcd.fillScreen(BLACK);
//  M5.Lcd.setCursor(0,4);
//  M5.Lcd.setTextSize(1);
//  M5.Lcd.println(" Adjust by NTP");
//  M5.Lcd.println(" Wifi Conecting...");
//  M5.Lcd.println("")

  // Wait for Serial initialization (max 4 seconds)
  
  dprint(F("Login Over Gadgets Around Network"));
  dprint(F("---------------------------------"));
  dprint();

  
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(8,8);
  M5.Lcd.println("Conecting");
  M5.Lcd.setCursor(8,30);
  M5.Lcd.print("Wifi... ");

  dwrite(F("Initializing wifi..."));
  WiFi.persistent(false); // Don't save WiFi configuration in flash
  WiFi.mode(WIFI_STA); // Set wifi mode as Station (client)
  WiFi.begin(wifiSSID, wifiPASS);

  //Wait until wifi is connected. Pause 1sec between retry
  waituntildo(WiFi.status() == WL_CONNECTED, delay(1000));  //{sprint("."); delay(1000);}
  dprint(F("DONE"));

  dprint();
  dshow(WiFi.SSID());
  dshow(WiFi.localIP());

  // Notify wifi connection through LCD (1s)
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.print("OK");
  delay(1000);

  


  //Send wake-up signal
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  //M5.Lcd.setTextSize(2);
  //M5.Lcd.setCursor(8,8);
  M5.Lcd.drawCentreString("Sending", 80, 16, 1);
  //M5.Lcd.setCursor(8,30);
  M5.Lcd.drawCentreString("Wake-Up", 80, 38, 1);

  String wakeupResponse = ""; //Variable for storing server response
  dprint("send WAKEUP");
  while (wakeupResponse != "OK") {
    wakeupResponse = httpGETRequest(serverWakeup);
    delay(2000);
  }
  
  
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN);
  //M5.Lcd.setTextSize(2);
  //M5.Lcd.setCursor(8,8);
  M5.Lcd.drawCentreString("Sended", 80, 16, 1);
  //M5.Lcd.setCursor(8,30);
  M5.Lcd.drawCentreString("Wake-Up", 80, 38, 1);
  M5.Lcd.setTextColor(WHITE);
  delay(1000);


  pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
  state = 0;

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.drawCentreString("LOCKED!!", 80, 40, 1);
}

void loop() {

  uint8_t val = digitalRead(M5_BUTTON_HOME);
  
  
  if (val == LOW) {
    if (state == 0){
      sendUnlock();
      state = 1;
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.drawCentreString("UNLOCKED", 80, 40, 1);
    }
    else {
      sendLock();
      state = 0;
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextColor(RED);
      M5.Lcd.drawCentreString("LOCKED!!", 80, 40, 1);
    }
    
    delay(1000);
    
  }
 
}
