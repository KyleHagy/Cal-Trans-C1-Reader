/***********************************************************************
  FILENAME :        C1Reader_vB6.0_ESP32s_Bluetooth.ino          DESIGN REF: C1 Reader vB6.0

  DESCRIPTION :
        ESP32s saves pin data from Max13362 to SD Card over SPI and
        timestamps changes of pin state with RTC over I2C.
        Addition:
        Sends incoming data over bluetooth to device wirelessly
        

  PUBLIC FUNCTIONS :
        void    initializeBooleanArray(bool boolArray[])
        void    retriveMAX13362Data(byte chipArray[], const int pinNum)
        int*    Bin2Bit(unsigned char byte1, unsigned char byte2, unsigned char byte3,  unsigned char byte4)
        String  printChipFunction(int* chipNumArray, bool chipBoolArray[], int ChipNum)
        void    timeStamp(unsigned long time_get)
        void    printTime()
        void    setup()
        void    loop()

  NOTES :
       ESP32s has to be turned on at the exact time as the other devices over SPI/I2C when testing
       printChipFunction() 1 or 0 should be changed to respective testing or field use based on active low or active high
       Libraries need further installation on new computer
       When in the field, comment out serial/serial.print()
       Text file on SD Card will be erased if power is turn off then back on
       To get setup with receiving bluetooth data please refer to the "ReadMe.txt"


  AUTHOR :    Kyle Hagy            START DATE :    15 Mar 20

  CHANGES :

  VERSION    DATE           WHO                 DETAIL
    3.1     31Aug20      Kyle Hagy        Implemented Bluetooth

*/

#include <SD.h>                  //RTC Library for SD Card
#include <SPI.h>                 //SPI Library for MAX13362 & SD Card
#include <Time.h>                //Time Library
#include <SparkFunDS1307RTC.h>   //RTC Library
#include <Wire.h>                //I2C Library
#include "BluetoothSerial.h"     //Bluetooth Library


#define CS1 25 //MAX13362 chip select - Chip 1
#define CS2 26 //MAX13362 chip select - Chip 2
#define CS3 27 //MAX13362 chip select - Chip 3
#define CS4 14 //MAX13362 chip select - Chip 4
#define CS5 12 //MAX13362 chip select - Chip 5

#define CSSDCard 13  //SD Card chip select


// Bluetooth Serial object
BluetoothSerial SerialBT;


//Used for SD Card and Dates for RTC
//File SDCardFile;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define PRINT_USA_DATE //Specifies the date format in which RTC print the month and day


//For initializing() & Bin2Bit()
byte maximChip_1_Array [4];
byte maximChip_2_Array [4];
byte maximChip_3_Array [4];
byte maximChip_4_Array [4];
byte maximChip_5_Array [4];

//For initializeBooleanArray() & printChipFunction()
//Meant to decide wheather a pin was off or on and prevent floating pins
bool boolArrayChip1[24];
bool boolArrayChip2[24];
bool boolArrayChip3[24];
bool boolArrayChip4[24];
bool boolArrayChip5[24];

//=======================================================================
//       INITALIZE ARRAY FOR STATE OF PIN IN printChipFunction()
//=======================================================================
//NOTE: SPI continuously sends data and receives the state of the pins.
//To stop useless data and the same state of the pin from being recorded,
//the data is only recorded if the previous state of the pin has changed.
void initializeBooleanArray(bool boolArray[])
{
  for (int y = 0; y < 24; y++)
  {
    boolArray[y] = false;
  }
}

//=======================================================================
//     STORES BYTE VAVLUES OF MAX13362 REGISTERS IN ARRAY USING SPI
//=======================================================================
void retriveMAX13362Data(byte chipArray[], const int pinNum)
{
  digitalWrite(pinNum, LOW);
  chipArray [0] = SPI.transfer(0x00);
  chipArray [1] = SPI.transfer(0x00);
  chipArray [2] = SPI.transfer(0x00);
  chipArray [3] = SPI.transfer(0x00);
  digitalWrite(pinNum, HIGH);
}

//=======================================================================
//  STRIPS EACH CHIP FOR BINARY VALUES OF 4 REGISTERS & RETURNS ARRAY
//=======================================================================
int* Bin2Bit(unsigned char byte1, unsigned char byte2, unsigned char byte3,  unsigned char byte4)
{
  static int longbits[24];

  int w = 0;
  int x = 0;
  int y = 0;
  int z = 0;

  // Extract the bits
  for (int i = 4; i > -1; --i) {
    // Mask each bit in the byte and store it
    longbits[w] = (byte1 & (1 << i)) != 0;
    w++;
  }
  for (int i = 7; i > -1; --i) {
    // Mask each bit in the byte and store it
    longbits[x + 5] = (byte2  & (1 << i)) != 0;
    x++;
  }
  for (int i = 7; i > -1; --i) {
    // Mask each bit in the byte and store it
    longbits[y + 13] = (byte3  & (1 << i)) != 0;
    y++;
  }
  for (int i = 7; i > 4; --i) {
    // Mask each bit in the byte and store it
    longbits[z + 21] = (byte4  & (1 << i)) != 0;
    z++;
  }
  return longbits;
}

//=======================================================================
//   TIME STAMPS DATA FOR printChipFunction() - Not being used right now
//=======================================================================
void timeStamp(unsigned long time_get)
{
  File SDCardFile = SD.open("/C1.txt", FILE_APPEND);
  int number_of_digits = 0;
  int n = time_get;
  do {
    ++number_of_digits;
    n /= 10;
  } while (n);

  for (int j = 0; j < 10 - number_of_digits; j++) {
    SDCardFile.print("0");
    Serial.print("0");
  }
  SDCardFile.println(time_get);
  Serial.println(time_get);
  SDCardFile.close();
}

//=======================================================================
//                 PRINT PIN AND CHIP THAT WAS SELECTED
//=======================================================================
String printChipFunction(int* chipNumArray, bool chipBoolArray[], int ChipNum)
{
  //Prints (CHIP #, PIN #, STATE, TIMESTAMP)  EX: 1 01 0 0000051235 -- Chip 1, Pin 1, Pin is ON, Time in milliseconds

  File dataFile1 = SD.open("/C1.txt", FILE_APPEND);

  String BTString;

  for (int z = 0; z < 24; z++)
  {

    if ((*(chipNumArray + z)) == 0 && chipBoolArray[z] == false) //Print pin turned on
    {
      chipBoolArray[z] = true;
      Serial.print(ChipNum);
      dataFile1.print(ChipNum);
      BTString += String(ChipNum);
      Serial.print(" ");
      dataFile1.print(" ");
      BTString += " ";
      if (z < 6)
      {
        Serial.print("0");
        dataFile1.print("0");
        BTString += "0";
        Serial.print(z + 4);
        dataFile1.print(z + 4);
        BTString += String(z + 4);

      }
      else
      {
        dataFile1.print(z + 4);
        Serial.print(z + 4);
        BTString += String(z + 4);
      }
      dataFile1.print(" 0 ");  // Change to 1 for Active Low (Field Use)
      Serial.print(" 0 ");
      BTString += " 0 ";
      //==========================
      // Add time in milliseconds to end of string
      int number_of_digits = 0;
      int n = millis();
      do {
        ++number_of_digits;
        n /= 10;
      } while (n);

      for (int i = 0; i < 10 - number_of_digits; i++) {
        dataFile1.print("0");
        Serial.print("0");
        BTString += "0";
      }
      dataFile1.println(millis());
      Serial.println(millis());
      BTString += String(millis());
      //=========================
    }
    if ((*(chipNumArray + z)) == 1 && chipBoolArray[z] == true) //Print pin turned off
    {
      chipBoolArray[z] = false;
      Serial.print(ChipNum);
      dataFile1.print(ChipNum);
      BTString += String(ChipNum);
      Serial.print(" ");
      dataFile1.print(" ");
      BTString += " ";
      if (z < 6)
      {
        dataFile1.print("0");
        dataFile1.print(z + 4);
        Serial.print("0");
        Serial.print(z + 4);
        BTString += "0";
        BTString += String(z + 4);
      }
      else
      {
        dataFile1.print(z + 4);
        Serial.print(z + 4);
        BTString += String(z + 4);
      }
      dataFile1.print(" 1 "); // Change to 0 for Active High (Field Use)
      Serial.print(" 1 ");
      BTString += " 1 ";
      //==========================
      // Add time in milliseconds to end of string
      int number_of_digits2 = 0;
      int n2 = millis();
      do {
        ++number_of_digits2;
        n2 /= 10;
      } while (n2);

      for (int j = 0; j < 10 - number_of_digits2; j++) {
        dataFile1.print("0");
        Serial.print("0");
        BTString += "0";
      }
      dataFile1.println(millis());
      Serial.println(millis());
      BTString += String(millis());
      //=========================
    }
  }
  dataFile1.close();
  return BTString;
}

//=======================================================================
//     (RTC) TIME STAMP FOR DATE OF DATA RECORDED AT TOP OF FILE
//=======================================================================
void printTime()
{
  rtc.update();
  File dataFile2 = SD.open("/C1.txt", FILE_WRITE);

  dataFile2.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    dataFile2.print('0'); // Print leading '0' for minute
  dataFile2.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    dataFile2.print('0'); // Print leading '0' for second
  dataFile2.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) dataFile2.print(" PM"); // Returns true if PM
    else dataFile2.print(" AM");
  }

  dataFile2.print(" | ");

  // Few options for printing the day, pick one:
  dataFile2.print(rtc.dayStr()); // Print day string
  //SDCardFile.print(rtc.dayC()); // Print day character
  //SDCardFile.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  dataFile2.print(" - ");
#ifdef PRINT_USA_DATE
  dataFile2.print(String(rtc.month()) + "/" +   // Print month
                  String(rtc.date()) + "/");  // Print date
#else
  dataFile2.print(String(rtc.date()) + "/" +    // (or) print date
                  String(rtc.month()) + "/"); // Print month
#endif
  dataFile2.println(String(rtc.year()));        // Print year

  dataFile2.close();
}

//=======================================================================
//                               SETUP
//=======================================================================
void setup()
{
  Serial.begin(115200);


  SerialBT.begin("C1_Reader"); //Starting the bluetooth connection and the name of the device

  //Fill the arrays with a bool to help know when pin has changed state
  initializeBooleanArray(boolArrayChip1);
  initializeBooleanArray(boolArrayChip2);
  initializeBooleanArray(boolArrayChip3);
  initializeBooleanArray(boolArrayChip4);
  initializeBooleanArray(boolArrayChip5);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);

  //Set the pin outputs MAX13362 chips
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  pinMode(CS3, OUTPUT);
  pinMode(CS4, OUTPUT);
  pinMode(CS5, OUTPUT);

  //wait for the spi connnection to initialize
  byte tempArray1[4], tempArray2[4] , tempArray3[4], tempArray4[4], tempArray5[4];
  for (int i = 0; i < 100; i++)
  {
    retriveMAX13362Data(tempArray1, CS1);
    retriveMAX13362Data(tempArray2, CS2);
    retriveMAX13362Data(tempArray3, CS3);
    retriveMAX13362Data(tempArray4, CS4);
    retriveMAX13362Data(tempArray5, CS5);
  }
  SPI.end(); //Stop MAX13362 before writing to SD Card

  SD.begin(CSSDCard);
  rtc.begin();

  //Print Date and Time from RTC
  printTime(); ///needs to be after initalizing
  SD.end();
}

//=======================================================================
//                               MAIN LOOP
//=======================================================================
void loop()
{
  //Used to temporary store string of the data from printChipFunction for Bluetooth
  String tempString1;
  String tempString2;
  String tempString3;
  String tempString4;
  String tempString5;

  //Pointers to the arrays of bits from Bin2Bit()
  int* maximChip_1;
  int* maximChip_2;
  int* maximChip_3;
  int* maximChip_4;
  int* maximChip_5;


  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);

  //Set the outputs MAX13362 chips
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  pinMode(CS3, OUTPUT);
  pinMode(CS4, OUTPUT);
  pinMode(CS5, OUTPUT);

  //Grab the SPI data from each MAX13362 chip
  retriveMAX13362Data(maximChip_1_Array, CS1);
  retriveMAX13362Data(maximChip_2_Array, CS2);
  retriveMAX13362Data(maximChip_3_Array, CS3);
  retriveMAX13362Data(maximChip_4_Array, CS4);
  retriveMAX13362Data(maximChip_5_Array, CS5);

  SPI.end(); //Stop MAX13362 before writing to SD Card

  SD.begin(CSSDCard);

  //Get Chip Pin data & Prints Pin to SD card if turned off or on

  //================================= CHIP 1 ======================================
  maximChip_1 = Bin2Bit(maximChip_1_Array [0], maximChip_1_Array [1], maximChip_1_Array [2], maximChip_1_Array [3]);      //Get bits/data from the SPI Transfer and returns that Chip Array

  tempString1 = printChipFunction(maximChip_1, boolArrayChip1, 1); //Print the pin that was set low or high

  if (tempString1 != "")  //Dont send empty string
  {
    SerialBT.println(tempString1);  //Send to bluetooth device
  }

  //================================= CHIP 2 ======================================
  maximChip_2 = Bin2Bit(maximChip_2_Array [0], maximChip_2_Array [1], maximChip_2_Array [2], maximChip_2_Array [3]);

  tempString2 = printChipFunction(maximChip_2, boolArrayChip2, 2);

  if (tempString2 != "")
  {
    SerialBT.println(tempString2);
  }

  //================================= CHIP 3 ======================================
  maximChip_3 = Bin2Bit(maximChip_3_Array [0], maximChip_3_Array [1], maximChip_3_Array [2], maximChip_3_Array [3]);

  tempString3 = printChipFunction(maximChip_3, boolArrayChip3, 3);

  if (tempString2 != "")
  {
    SerialBT.println(tempString3);
  }

  //================================= CHIP 4 ======================================
  maximChip_4 = Bin2Bit(maximChip_4_Array [0], maximChip_4_Array [1], maximChip_4_Array [2], maximChip_4_Array [3]);

  tempString4 = printChipFunction(maximChip_4, boolArrayChip4, 4);

  if (tempString4 != "")
  {
    SerialBT.println(tempString4);
  }

  //================================= CHIP 5 ======================================
  maximChip_5 = Bin2Bit(maximChip_5_Array [0], maximChip_5_Array [1], maximChip_5_Array [2], maximChip_5_Array [3]);

  tempString5 = printChipFunction(maximChip_5, boolArrayChip5, 5);

  if (tempString5 != "")
  {
    SerialBT.println(tempString5);
  }

  SD.end();
}
