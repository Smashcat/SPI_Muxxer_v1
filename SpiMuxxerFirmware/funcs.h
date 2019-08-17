void setPins(void);
void setPorts(uint8_t s);
bool buttonPressed(uint8_t b);
void resetDeviceOnPort(uint8_t d);
void programDeviceOnPort(uint8_t d);

/**
 * Set up default state for pins for device
 */
void setPins(void){
  pinMode(DS,OUTPUT);
  
  // Initially we just passively monitor the SPI bus.
  pinMode(BUS_SCK,INPUT);
  pinMode(BUS_MISO,INPUT);
  pinMode(BUS_MOSI,INPUT);

  // Set ISP bus RESET pin to high impedence, and LOW, so it can be set to output to slam RESET low when required
  pinMode(BUS_RESET,INPUT);
  digitalWrite(BUS_RESET,LOW);
  
  // prog button is pulled down by external resistor, so active high
  pinMode(PROG_BUT,INPUT);

  // MR (shift register master reset) output is pulled up by external resistor, so active low
  pinMode(MR,OUTPUT);
  digitalWrite(MR,HIGH);
  
  // SHCP (shift register clock) output is pulled up by external resistor, so active low
  pinMode(SHCP,OUTPUT);

  // STCP (shift register storage register clock, latches on rising edge)
  pinMode(STCP,OUTPUT);
}

/**
 * Set the SPI/ISP port state. The ports 1-8 relate to the bits 0-7 in uint8_t "s". High=active
 */
void setPorts(uint8_t s){
  digitalWrite(STCP,LOW);
  for(uint8_t n=0;n<8;n++){
    digitalWrite(SHCP,LOW);
    digitalWrite(DS,(s&(1<<(7-n)))==0?LOW:HIGH);
    digitalWrite(SHCP,HIGH);
  }
  digitalWrite(STCP,HIGH);
}

// Returns true if button is pressed - does some debouncing
bool buttonPressed(uint8_t b){
  switch(b){
    case RESET_BUT:
      if(analogRead(RESET_BUT)>500){
        delay(1);
        if(analogRead(RESET_BUT)>500){
          return true;
        }
      }
      return false;
    break;
    case PROG_BUT:
      if(digitalRead(PROG_BUT)==HIGH){
        delay(1);
        if(digitalRead(PROG_BUT)==HIGH){
          return true;
        }
      }
      return false;
    break;
  }
}

// Pauses until button is released, to prevent re-triggering
void waitBut(){
  while(buttonPressed(PROG_BUT) || buttonPressed(RESET_BUT)){
    delay(10);  
  }
  delay(200);
}

// Pulls reset low for 1/2 second for the device on port d (remember, ports are 0-7 internally)
void resetDeviceOnPort(uint8_t d){
  setPorts(1<<d);
  delay(10);
  pinMode(BUS_RESET,OUTPUT);  // Already LOW, so this just slams RESET to GND
  delay(500);
  pinMode(BUS_RESET,INPUT);
  setPorts(portState);
}

// When called, disconnects all but device d, and waits for programming session to complete before re-enabling other ports
// Note that the reset button can be used to cancel this. We use activity on the SCK line to indicate programming activity
void programDeviceOnPort(uint8_t d){
  setPorts(1<<d);
  delay(10);
  // Wait for programming to start...
  while(digitalRead(BUS_SCK)==LOW){
    if(buttonPressed(RESET_BUT)){
      waitBut();
      setPorts(portState);
      return;
    }
    delay(1);
  }

  // Wait until 1 second after activity finished...
  uint16_t timeout=1000;
  while(timeout>0){
    if(digitalRead(BUS_SCK)==HIGH)
      timeout=1000;
    --timeout;
    delay(1);
  }
  
  setPorts(portState);
}
