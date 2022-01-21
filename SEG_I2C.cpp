/***************************************************
  This is code HT16K33 through I2C converter
     
 

 ****************************************************/
#include <Arduino.h>
#include <stdint.h>
#include "SEG_I2C.h"

void SEG_I2C::begin(uint8_t addr)
{
   writeData(addr, 0x21);  // HT16K33 ocsillation start 
   delay(10);
}
void SEG_I2C::setBrightness(uint8_t addr, uint8_t br)
{
   if(br > 15) br = 15;
   writeData(addr, HT16K33_BRIGHTNESS_CMD | br);    // br = 0~15; 
   delay(10);  
}
void SEG_I2C::blinkRate(uint8_t addr, uint8_t bl)
{
    if(bl > 3) bl = 0;    // turn off if not sure, 0=OFF, 1=2HZ, 2=1HZ, 3=0.5HZ
    writeData(addr, HT16K33_BLINK_CMD | HT16K33_DISP_ON | (bl<<1));   // bl = HT16K33_BLINK_2HZ
    delay(10);
}
void SEG_I2C::displayOn(uint8_t addr)
{
   writeData(addr, HT16K33_BLINK_CMD | HT16K33_DISP_ON);
   delay(10); 
}
void SEG_I2C::displayOff(uint8_t addr)
{
   writeData(addr, HT16K33_BLINK_CMD | HT16K33_DISP_OFF); 
   delay(10);
}
void SEG_I2C::writeDisplay(uint8_t addr, uint16_t* d0, uint16_t* d1, uint16_t* d2, uint16_t* d3)
{
    uint8_t disp_data[8];
    
    displaybuffer[0] = *d0;
    displaybuffer[1] = *d1;
    displaybuffer[2] = *d2;
    displaybuffer[3] = *d3;

    for(uint8_t i=0; i<4; i++)    // fill zero in all disp data
    {
      disp_data[i*2] = displaybuffer[i]>>8;
      disp_data[(i*2)+1] = displaybuffer[i]%256;
    }
    
    Wire.beginTransmission(addr);
    Wire.write(0x00);
    Wire.write(disp_data,8);
    Wire.endTransmission();
    delay(10);   
}
void SEG_I2C::clear(uint8_t addr)
{
    uint8_t disp_data[8];
    
    for(uint8_t i=0; i<8; i++)    // fill zero in all disp data
      disp_data[i] = 0;  
    
    Wire.beginTransmission(addr);
    Wire.write(0x00);
    Wire.write(disp_data,8);
    Wire.endTransmission();
    delay(10);
}

void SEG_I2C::writeData(uint8_t addr, uint8_t val)
{
    Wire.beginTransmission(addr);
    Wire.write(val);
    Wire.endTransmission();
    delay(10);    
}

void lampControl(uint8_t signal)
{
  switch(signal)
  {
    case 4:   // ready
      digitalWrite(49, HIGH);
      digitalWrite(50, LOW);
      digitalWrite(51, LOW);
      break;
    case 2:   // run
      digitalWrite(49, LOW);
      digitalWrite(50, HIGH);
      digitalWrite(51, LOW);
      break;
    case 1:   // end
      digitalWrite(49, LOW);
      digitalWrite(50, LOW);
      digitalWrite(51, HIGH);
      break;
    case 8:   // off
      digitalWrite(49, LOW);
      digitalWrite(50, LOW);
      digitalWrite(51, LOW);
      break;
    case 10:   // on
      digitalWrite(49, HIGH);
      digitalWrite(50, HIGH);
      digitalWrite(51, HIGH);
      break;
    default:
      break; 
  }
}
