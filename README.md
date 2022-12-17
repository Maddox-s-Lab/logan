# LOGAN
Loging Over Gadgets Aroud Network

This is a gadget setup for protecting a computer for non-authorised physical access when you leave your desk.

## Logic
- When your M5Stick enters the Wifi network, it warns the ESP32 server, which send a WOL magic packet to the Computer for starting it up.
- Then, the user can unlock and lock its computer throug his M5Stick wristband by using the middle button.
- When the user leaves the building (and the wifi network), the ESP32 server send a suspend command to the PC.


## Files
There are 3 different Arduino projects for the 3 MCUs board used on the project
 - **KeyboardSimulation** --- Digispark ATtiny85
 - **M5stick_logan** --- M5StickC (ESP32)
 - **ESP32_Server** --- FireBeetle Board-ESP32

## Disclaimer
This is only a demo project and can not be safe enough on a real environment. Use it at your own risk and only for testing purposes.
