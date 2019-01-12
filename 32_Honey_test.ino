/*
 ********************************************************************************************* 
 * Alan Campbell 12/22/2018
 * 
 * HiLetgo ESP-WROOM-32 ESP32 ESP-32S Development Board
 * 
 * Arduino IDE 1.8.8
 * 
 Communication protocol Honeywell HPM Particle Sensor
Serial port baudrate: 9600; Parity: None; Stop Bits: 1; packet length is fixed at 32 bytes.
Byte
0      Start Character 1  0x42(fixed bit)
1      Start Character 2 0x4d(fixed bit)
2,3    Frame Length 16-byte  Frame Length = 2*9+2 (data+check bit)
4,5    Data 1, 16-byte concentration of PM1.0, ug/m3
6,7    Data 2, 16-byte concentration of PM2.5, ug/m3
8,9    Data 3, 16-byte concentration of PM10.0, ug/m3
10,11  Data 4, 16-byte Internal test data
12,13  Data 5, 16-byte Internal test data
14,15  Data 6, 16-byte Internal test data
16,17  Data 7, 16-byte the number of particulate of diameter above 0.3um in 0.1 liters of air
18,19  Data 8, 16-byte the number of particulate of diameter above 0.5um in 0.1 liters of air
20,21  Data 9, 16-byte the number of particulate of diameter above 1.0um in 0.1 liters of air
22,23  Data 10, 16-byte  the number of particulate of diameter above 2.5um in 0.1 liters of air
24,25  Data 11, 16-byte  the number of particulate of diameter above 5.0um in 0.1 liters of air
26,27  Data 12, 16-byte  the number of particulate of diameter above 10.0um in 0.1 liters of air
28,29  Data 13, 16-byte  Internal test data
30,31  Check Bit for Data Sum, 16-byte Check Bit = Start Character 1 + Start Character 2 + ...all data
 *********************************************************************************************
 */

#include <HardwareSerial.h>
HardwareSerial mySerial(1);
 
byte readpart[] = {0x68, 0x01, 0x04, 0x93};  //Listed command to read particle data
byte readstart[] = {0x68, 0x01, 0x04, 0x96}; //Listed command to start particle measurement
byte response[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //32 byte array to store returned data
int pm1 =  0; //PM1.0,  ug/m3
int pm2 =  0; //PM2.5,  ug/m3
int pm10 = 0; //PM10.0, ug/m3

 
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600); //Opens the main serial port to communicate with the computer
  mySerial.begin(9600, SERIAL_8N1, 16, 17); //Rx,Tx Opens the virtual serial port with a baud of 9600
  delay(2000);                                                 //Wait for serial to have started
  Serial.println("Sending particle start reading code");       //Update user that autosend code sent
  mySerial.write(readstart,8);                                 //Request sensor starts particle reading
  delay(6000);                                                 //Allow 6 seconds for sensor response time
  Serial.println("Sensor started");                            //Confirm end of setup code
}
 
void loop()
{ sendRequest(readpart);                                       //Send the read code request
  delay(1000);                                             
  //for(int i = 0; i < 32; i++){                                  //Print out the data obtaine
  //  Serial.print(response[i]);
  //  Serial.print(", ");
  //}
  pm1 = 255*response[4] + response[5];
  pm2 = 255*response[6] + response[7];
  pm10 = 255*response[8] + response[9];   
  Serial.print(pm1);
  Serial.print(", ");
  Serial.print(pm2);
  Serial.print(", ");
  Serial.print(pm10);
  Serial.println(" ");
}
 
//Sub command that runs inside the main loop
void sendRequest(byte packet[])
{
  while(!mySerial.available())                                  //keep sending request until we start to get a response
{ //mySerial.write(readpart,4);                                 //send command for a reading
    delay(50);
}
  int timeout=0;                                                //set a timeout counter
  while(mySerial.available() < 32)                              //Hold in this loop until we get a 32 bit response
   { timeout++; 
   if(timeout > 10)                                             //wait 10ms to check if we're happy, else reset and wait again
     { while(mySerial.available())                                             
        mySerial.read();                                        //read everything out the buffer to empty it                   
        break;                                                  //exit and try again
     }
    delay(50);
   }
  for (int i=0; i < 32; i++)
{ response[i] = mySerial.read();                                //Read data out the buffer into into the response array
}}
