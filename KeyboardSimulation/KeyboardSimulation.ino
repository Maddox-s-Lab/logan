#include <SoftSerial_INT0.h>

#include <DigiKeyboard.h>

SoftSerial sSerial(2, 1); // First pin rx (receive) - second pin tx (transmit)

void WritePassword() {
  // Write the password of the user once the Master has awakened the PC.
  DigiKeyboard.println("070300mmr");
}
void WriteSuspend() {
  // Open shell on Ubuntu / Linux Mint
  DigiKeyboard.sendKeyStroke(KEY_T, MOD_CONTROL_LEFT | MOD_ALT_LEFT);
  DigiKeyboard.delay(2000);
  // Suspend the pc, with super user permissions
  DigiKeyboard.write("sudo systemctl suspend\n");
  DigiKeyboard.delay(500);
  // Write here the sudo password of the pc to suspend
  DigiKeyboard.write("070300mmr\n");
}
void setup() {
  sSerial.begin(9600);
}

void loop() {
  if (sSerial.available() > 0) {
    // Read byte sent from ESP32 and casting it to char.
    uint8_t byteFromSerial = sSerial.read();
    char c = (char) byteFromSerial;

    switch (c) {
      case '0':
        // Unblock session  
        WritePassword();
        break;
      case '1':
        // Block session
        WriteSuspend();
        break;
    }
  }
}