#include <DigiKeyboard.h>


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
  pinMode(1, OUTPUT);
    //3 parpadeos para indicar que inicia la placa
  for( int i = 0; i < 3; i++) {
    digitalWrite(1, HIGH);
    DigiKeyboard.delay(150);
    digitalWrite(1, LOW);
    DigiKeyboard.delay(150);
  }

}

void loop() {
  
  while(true) {
    // Listen for master ESP32 orders

    // Act
    // WritePassword();
    // WriteSuspend();
  }
}
