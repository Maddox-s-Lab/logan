#include <SoftSerial_INT0.h>
#include <DigiKeyboard.h>

SoftSerial sSerial(2, 1); // First pin rx (receive) - second pin tx (transmit)


// Write the password of the user once the Master has awakened the PC.
void WritePassword() {
  DigiKeyboard.println("070300mmr");
}
// Press keyboard shortcut to lock the session (Windows + L) 
void WriteLogout() {
  DigiKeyboard.sendKeyStroke(KEY_L, MOD_GUI_LEFT);
}

// Open terminal (Cntrl + Alt + T), write command to suspend
void WriteSuspend() {
  // Open shell on Ubuntu / Linux Mint
  DigiKeyboard.sendKeyStroke(KEY_T, MOD_CONTROL_LEFT | MOD_ALT_LEFT);
  DigiKeyboard.delay(2000);
  // Suspend the pc
  DigiKeyboard.write("systemctl suspend\n");
}

void setup() {
  sSerial.begin(9600);
}

void loop() {
  DigiKeyboard.update();
  if (sSerial.available() > 0) {
    // Read byte sent from ESP32 and casting it to char.
    uint8_t byteFromSerial = sSerial.read();
    char c = (char) byteFromSerial;

    switch (c) {
      case '1': // Unblock session  
        WritePassword();
        break;
      case '2': // Block session
        WriteLogout();
        break;
      case '3': // Suspend PC
        WriteSuspend();
        break;
    }
  }
}