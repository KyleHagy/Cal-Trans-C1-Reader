Setup

1. Install the Arduino IDE at https://www.arduino.cc/en/Main/Software

2. Installing the ESP32 
	a. Go to File->Preferences
	b. Go to "Additional Board Manager URLs" and copy and paste "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"
	   into the box and then hit "OK"              	 
	c. Go to Tools->Board->Boards Manager…
	d. Search for ESP32 and press install button for the “ESP32 by Espressif Systems“
	- More Info: "https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-mac-and-linux-instructions/"

4. Test: Go to Tools->Board and select "NodeMCU-32s" and try uploading a sketch by pressing the arrow button on the top left 
   and make sure that the right "Port" in "Tools" is selected to the micro-USB connected ESP32s.

REMINDER: Hold "IOO" button when console prompts "Connecting........" to upload the sketch until data is written or prints "Hard resetting via RTS pin..." 

5. Download RTC library - "SparkFunDS1307RTC.h" at https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library

6. Go to Sketch->Include Library-> Add .ZIP Library and locate the download library and add it  

7. Download the SD and FS libraries here "https://github.com/espressif/arduino-esp32"

8. Copy and paste SD & FS in the downloaded folder within "libraries" to C:\Program Files(x86)\Arduino\libraries (Replace/delete the SD library already there)





FOR BLUETOOTH

1. Open and flash C1Reader_vB6.0_ESP32s_Bluetooth.ino to the ESP32 the same as above
	NOTE: BluetoothSerial.h" library should already be install, if not please install the library

2. To receive data from external device (Android Only ATM)
 	a. Install “Serial Bluetooth Terminal” from Play Store.
	b. Open the app and then open the side panel and click "Devices." 
	c. While on "Bluetooth Classic" select the device named "C1_Reader" and pair to this device. 
	d. You should get a "connected" message in the "terminal" and at this point you should be ready to receive incoming data.
