/**
 * Firmware for SPI MUXXER V1.0
 * 
 * This is designed for the Arduino environment, using these settings:
 * 
 * Board: Generic ESP8266 module
 * Upload speed: 921600
 * CPU Frequency: 160mhz
 * Flash mode: DIO
 * Reset Method: nodemcu
 * 
 * See defs.h for the pin usage.
 * 
 */

#include "defs.h"
#include "vars.h"
#include "funcs.h"

// JUST SOME TEST CODE AT THE MOMENT! 
// "Active device" is on port 8, and no firmware to change as yet. Will have web interface!
void setup() {
  setPins();

  // For testing, just set all ports on. This should really be set via a UI
  portState=0xff;

  // Port 8 is the current active port, which is the target for reset and programming. This should really be set via a UI
  activeDevice=7;

  // Just check LEDs and ports all work by "scanning" them across.
  uint8_t s=1;
  while(s>0){
    setPorts(s);
    s<<=1;
    delay(50);
  }

  // Set ports up, as above
  setPorts(portState);
}

void loop() {

  // If reset button pressed, wait for release, then reset the current active device
  if(buttonPressed(RESET_BUT)){
    while(buttonPressed(RESET_BUT)){
      delay(1);
    }
    resetDeviceOnPort(activeDevice);
  }

  // If prog button pressed, wait for release, then disable everything except active port, and wait for programming to complete
  if(buttonPressed(PROG_BUT)){
    while(buttonPressed(PROG_BUT)){
      delay(1);
    }
    programDeviceOnPort(activeDevice);
  }

  // twiddle fingers :-)
  delay(100);
  yield();
  
}
