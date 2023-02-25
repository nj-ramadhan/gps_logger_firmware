#include <SoftwareSerial.h>

SoftwareSerial serialReadGPS(12, 11); // RX, TX

const char COMM_INIT[] PROGMEM = {
  // Change Baudrate to 115200
  0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00,
  0x00, 0x00, 0xC2, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xBC, 0x5E,
};

const char UBLOX_INIT[] PROGMEM = {
  // Enable Raw Data
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0x02 ,0x15 ,0x01 ,0x01 ,0x00 ,0x01 ,0x01 ,0x00 ,0x2A ,0x56, // enable RXM-RAWX
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0x02 ,0x13 ,0x01 ,0x01 ,0x00 ,0x01 ,0x01 ,0x00 ,0x28 ,0x48, // enable RXM-SFRBX
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0x02 ,0x14 ,0x01 ,0x01 ,0x00 ,0x01 ,0x01 ,0x00 ,0x29 ,0x4F, // enable RXM-MEASX  
  
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0x01 ,0x06 ,0x01 ,0x01 ,0x00 ,0x01 ,0x01 ,0x00 ,0x1A ,0xE5, // enable NAV-SOL
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0x01 ,0x22 ,0x01 ,0x01 ,0x00 ,0x01 ,0x01 ,0x00 ,0x36 ,0xA9, // enable NAV-CLOCK
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0x01 ,0x30 ,0x01 ,0x01 ,0x00 ,0x01 ,0x01 ,0x00 ,0x44 ,0x0B, // enable NAV-SVINFO
  0xB5 ,0x62 ,0x06 ,0x4A ,0x18 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x1F ,0x00 ,0x00 ,0x00 ,0xFF ,0xFF ,0xFF ,0xFF ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x83 ,0xAC,

  // Disable NMEA
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0x23, // disable GxGGA
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x2A, // disable GxGLL
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x02 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,0x31, // disable GxGSA
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x02 ,0x38, // disable GxGSV
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x04 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x03 ,0x3F, // disable GxRMC
  0xB5 ,0x62 ,0x06 ,0x01 ,0x08 ,0x00 ,0xF0 ,0x05 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x04 ,0x46, // disable GxVTG
};

const char RATE_INIT[] PROGMEM = {
  // Rate Setting
  0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0xE8 ,0x03 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x39, // set Rate 1 Hz
};

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Device Ready");

  // set the data rate for the SoftwareSerial port
  Serial.println("Baud 9600");
  serialReadGPS.begin(9600);
  // send Baud Rate Configuration data in UBX protocol
  for(int i = 0; i < sizeof(COMM_INIT); i++) {                        
    serialReadGPS.write(pgm_read_byte(COMM_INIT+i) );
    delay(10); // simulating a 115200 baud.
  }
  delay(1000);

  Serial.println("Configuring init UBX");
  serialReadGPS.begin(115200);
  Serial.println("Baud 115200");
  // send Protocol Configuration data in UBX protocol
  for(int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
    serialReadGPS.write(pgm_read_byte(UBLOX_INIT+i) );
    delay(10); // simulating a 115200 baud.
  }
  delay(1000);

  Serial.println("Setting Rate");
  for(int i = 0; i < sizeof(RATE_INIT); i++) {                        
    serialReadGPS.write(pgm_read_byte(UBLOX_INIT+i) );
    delay(10); // simulating a 115200 baud.
  }
  delay(200);
}

void loop() { // run over and over
  if (serialReadGPS.available()) {
    Serial.write(serialReadGPS.read());
  }
  if (Serial.available()) {
    serialReadGPS.write(Serial.read());
  }
}
