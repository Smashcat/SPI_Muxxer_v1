
// Current state of the ISP/SPI ports. Bits 0-7 = ports 1-8. High=active
uint8_t portState=0;
uint8_t activeDevice=0; // current active device - ranges from 0-7 for ports 1-8
