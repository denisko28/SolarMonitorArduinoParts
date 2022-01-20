#include <SPI.h>
#include <SD.h>
#include <Wire.h>                           // Подключаем библиотеку Wire                           
#include <TimeLib.h>                        // Подключаем библиотеку TimeLib
#include <DS1307RTC.h>
#include <SoftwareSerial.h>

SoftwareSerial stmSerial(7, 8); // RX, TX
File myFile;

void setup() {
  // set the data rate for the SoftwareSerial port
  stmSerial.begin(9600);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");
  pinMode(4, OUTPUT);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  
}

void createReport(char * message){
  tmElements_t tm;                          
 
  if (RTC.read(tm)) {
    int year4digit = tm.Year + 1970;
    char timedatebuf[65];
    sprintf(timedatebuf, "Time: %02d:%02d:%02d   Date: %02d/%02d/%02d  Message: %s \n",tm.Hour, tm.Minute, tm.Second, tm.Day, tm.Month, year4digit, message);
    myFile.print(timedatebuf);
    Serial.print(timedatebuf);
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
}

void readFromSerial(char * buff)
{
  int i=0;
  while(Serial.available() > 0)
  {
      *(buff+i) = Serial.read();
      *(buff+i+1) = '\0'; // Append a null
      i++;
  }
}

void loop() {
  
  if (Serial.available()  > 0)
  {
    delay(100);
    char buff[25];
    readFromSerial(buff);
    
    Serial.println("Opening");
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) 
    {
      Serial.print("Writing to test.txt...");

      createReport(buff);
      
      //      myFile.println("Helo bitch"); 
      myFile.close();
      Serial.println("done.");
    } 
    else 
    {
        Serial.println("error opening test.txt");
    }
  }
}
