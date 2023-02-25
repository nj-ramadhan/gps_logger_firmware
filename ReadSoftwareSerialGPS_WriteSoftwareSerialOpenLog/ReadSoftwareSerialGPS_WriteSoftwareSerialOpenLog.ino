/*
  Software serial multiple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>

SoftwareSerial serialReadGPS(12, 11); // RX, TX
SoftwareSerial serialWriteOpenLog(3, 4); // RX, TX

int resetOpenLog = 5;
int statLED = 13;
float dummyVoltage = 3.50; //This just shows to to write variables to OpenLog

void setup() {
  // Open serial communications and wait for port to open:
  pinMode(statLED, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Device Ready");

  // set the data rate for the GPS SoftwareSerial port
  serialReadGPS.begin(9600);
  // set the data rate for the openLog SoftwareSerial port
  // serialWriteOpenLog.begin(9600); //Open software serial port at 9600bps
  // setupOpenLog(); //Resets logger and waits for the '<' I'm alive character
  // Serial.println("OpenLog online");

}

void loop() { // run over and over
  if (serialReadGPS.available()) {
    Serial.write(serialReadGPS.read());
  }
  if (Serial.available()) {
    serialReadGPS.write(Serial.read());
  }
  /*
  randomSeed(analogRead(A0)); //Use the analog pins for a good seed value
  int fileNumber = random(999); //Select a random file #, 0 to 999
  char fileName[12]; //Max file name length is "12345678.123" (12 characters)
  sprintf(fileName, "log%03d.txt", fileNumber);

  gotoCommandMode(); //Puts OpenLog in command mode
  createFile(fileName); //Creates a new file called log###.txt where ### is random

  Serial.print("Random file created: ");
  Serial.println(fileName);

  //Write something to OpenLog
  serialWriteOpenLog.println("Hi there! How are you today?");
  serialWriteOpenLog.print("Voltage: ");
  serialWriteOpenLog.println(dummyVoltage);
  dummyVoltage++;
  serialWriteOpenLog.print("Voltage: ");
  serialWriteOpenLog.println(dummyVoltage);

  Serial.println("Text written to file");
  Serial.println("Reading file contents:");
  Serial.println();

  //Now let's read back
  gotoCommandMode(); //Puts OpenLog in command mode
  readFile(fileName); //This dumps the contents of a given file to the serial terminal

  //Now let's read back
  readDisk(); //Check the size and stats of the SD card

  Serial.println();
  Serial.println("File read complete");

  //Infinite loop
  Serial.println("Yay!");
  while(1) {
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
  }
  */
}

//Setups up the software serial, resets OpenLog so we know what state it's in, and waits
//for OpenLog to come online and report '<' that it is ready to receive characters to record
void setupOpenLog(void) {
  pinMode(resetOpenLog, OUTPUT);
  serialWriteOpenLog.begin(9600);

  //Reset OpenLog
  digitalWrite(resetOpenLog, LOW);
  delay(100);
  digitalWrite(resetOpenLog, HIGH);

  //Wait for OpenLog to respond with '<' to indicate it is alive and recording to a file
  while(1) {
    if(serialWriteOpenLog.available())
      if(serialWriteOpenLog.read() == '<') break;
  }
}

//This function creates a given file and then opens it in append mode (ready to record characters to the file)
//Then returns to listening mode
void createFile(char *fileName) {

  //Old way
  serialWriteOpenLog.print("new ");
  serialWriteOpenLog.print(fileName);
  serialWriteOpenLog.write(13); //This is \r

  //New way
  //OpenLog.print("new ");
  //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

  //Wait for OpenLog to return to waiting for a command
  while(1) {
    if(serialWriteOpenLog.available())
      if(serialWriteOpenLog.read() == '>') break;
  }

  serialWriteOpenLog.print("append ");
  serialWriteOpenLog.print(fileName);
  serialWriteOpenLog.write(13); //This is \r

  //Wait for OpenLog to indicate file is open and ready for writing
  while(1) {
    if(serialWriteOpenLog.available())
      if(serialWriteOpenLog.read() == '<') break;
  }

  //OpenLog is now waiting for characters and will record them to the new file  
}

//Reads the contents of a given file and dumps it to the serial terminal
//This function assumes the OpenLog is in command mode
void readFile(char *fileName) {

  //Old way
  serialWriteOpenLog.print("read ");
  serialWriteOpenLog.print(fileName);
  serialWriteOpenLog.write(13); //This is \r

  //New way
  //OpenLog.print("read ");
  //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

  //The OpenLog echos the commands we send it by default so we have 'read log823.txt\r' sitting 
  //in the RX buffer. Let's try to not print this.
  while(1) {
    if(serialWriteOpenLog.available())
      if(serialWriteOpenLog.read() == '\r') break;
  }  

  //This will listen for characters coming from OpenLog and print them to the terminal
  //This relies heavily on the SoftSerial buffer not overrunning. This will probably not work
  //above 38400bps.
  //This loop will stop listening after 1 second of no characters received
  for(int timeOut = 0 ; timeOut < 1000 ; timeOut++) {
    while(serialWriteOpenLog.available()) {
      char tempString[100];
      
      int spot = 0;
      while(serialWriteOpenLog.available()) {
        tempString[spot++] = serialWriteOpenLog.read();
        if(spot > 98) break;
      }
      tempString[spot] = '\0';
      Serial.write(tempString); //Take the string from OpenLog and push it to the Arduino terminal
      timeOut = 0;
    }

    delay(1);
  }

  //This is not perfect. The above loop will print the '.'s from the log file. These are the two escape characters
  //recorded before the third escape character is seen.
  //It will also print the '>' character. This is the OpenLog telling us it is done reading the file.  

  //This function leaves OpenLog in command mode
}

//Check the stats of the SD card via 'disk' command
//This function assumes the OpenLog is in command mode
void readDisk() {

  //Old way
  serialWriteOpenLog.print("disk");
  serialWriteOpenLog.write(13); //This is \r

  //New way
  //OpenLog.print("read ");
  //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

  //The OpenLog echos the commands we send it by default so we have 'disk\r' sitting 
  //in the RX buffer. Let's try to not print this.
  while(1) {
    if(serialWriteOpenLog.available())
      if(serialWriteOpenLog.read() == '\r') break;
  }  

  //This will listen for characters coming from OpenLog and print them to the terminal
  //This relies heavily on the SoftSerial buffer not overrunning. This will probably not work
  //above 38400bps.
  //This loop will stop listening after 1 second of no characters received
  for(int timeOut = 0 ; timeOut < 1000 ; timeOut++) {
    while(serialWriteOpenLog.available()) {
      char tempString[100];
      
      int spot = 0;
      while(serialWriteOpenLog.available()) {
        tempString[spot++] = serialWriteOpenLog.read();
        if(spot > 98) break;
      }
      tempString[spot] = '\0';
      Serial.write(tempString); //Take the string from OpenLog and push it to the Arduino terminal
      timeOut = 0;
    }

    delay(1);
  }

  //This is not perfect. The above loop will print the '.'s from the log file. These are the two escape characters
  //recorded before the third escape character is seen.
  //It will also print the '>' character. This is the OpenLog telling us it is done reading the file.  

  //This function leaves OpenLog in command mode
}

//This function pushes OpenLog into command mode
void gotoCommandMode(void) {
  //Send three control z to enter OpenLog command mode
  //Works with Arduino v1.0
  serialWriteOpenLog.write(26);
  serialWriteOpenLog.write(26);
  serialWriteOpenLog.write(26);

  //Wait for OpenLog to respond with '>' to indicate we are in command mode
  while(1) {
    if(serialWriteOpenLog.available())
      if(serialWriteOpenLog.read() == '>') break;
  }
}

