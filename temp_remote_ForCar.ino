// nrf24_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_NRF24 driver to control a NRF24 radio.
// It is designed to work with the other example nrf24_reliable_datagram_server
// Tested on Uno with Sparkfun WRL-00691 NRF24L01 module
// Tested on Teensy with Sparkfun WRL-00691 NRF24L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>


#define Joystick_x_pin A0
#define Joystick_y_pin A1

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_NRF24 driver;
// RH_NRF24 driver(8, 7);   // For RFM73 on Anarduino Mini

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

int Xc,Yc;
uint8_t motorControl[3]={0,0,0};


uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
}



void loop()
{
  Serial.println("Reading joystick values");

  //reading values of joystick
  Xc=map(analogRead(Joystick_x_pin),0,1023,0,255);
  Yc=map(analogRead(Joystick_y_pin),0,1023,255,0);
  if(Xc<130 && Xc>122 && Yc<134 && Yc>126)
  {
    motorControl[0]=0;
    motorControl[1]=0;
    motorControl[2]=0;
  }

  else if(Yc>=134 && Xc>=122 && Xc<=130)
  {
    motorControl[0]=map(Yc,126,255,0,255);
    motorControl[1]=motorControl[0];
    motorControl[2]=0;
    
  }
  else if(Yc<=126 && Xc>=122 && Xc<=130)
  {
    motorControl[0]=map(Yc,126,0,0,255);
    motorControl[1]=motorControl[0];
    motorControl[2]=1;
  }
  else if(Xc<122)
  { 
    
    motorControl[0]=map(Xc,0,122,0,255);
    motorControl[1]=255;
    if(Yc>=130)
    {
      motorControl[2]=0;
    }

    else if(Yc<130)
    {
      motorControl[2]=1;
    }      
    
  }
  else if(Xc>130)
  {
    motorControl[0]=255;
    motorControl[1]=map(Xc,130,255,255,0);
    if(Yc>=130)
    {
      motorControl[2]=0;
    }

    else if(Yc<130)
    {
      motorControl[2]=1;
    }      
  }
  

  


   //dispay at serial monitor

   Serial.println("----------");
   Serial.print("Xc:");
   Serial.println(Xc);
   Serial.print("Yc:"); 
   Serial.println(Yc);
   Serial.print("Speed 1:");
   Serial.println(motorControl[0]);
   Serial.print("speed 2:");
   Serial.println(motorControl[1]);
   Serial.print("Direction:");
   Serial.println(motorControl[2]);
  // Send a message to manager_server
  if (manager.sendtoWait(motorControl, sizeof(motorControl), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is nrf24_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  delay(50);
}
