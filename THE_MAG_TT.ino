/****************************************************
 * 슬레이브 어드레스는 E0. E2로 잡힘
 * 오실레이션 시작 0X21
 * BLINK|DISPLAY_ON 0X81
 * BRIGHTNESS 0XE?,  ?: 0~15사이값
 * 테스트 전문
 * 

 ****************************************************/
#include <Arduino.h>
#include <stdint.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "SEG_I2C.h"

static const uint16_t numbertable[] = {
  0xF801, /* 0 */
  0x3000, /* 1 */
  0xD802, /* 2 */
  0x7802, /* 3 */
  0x3003, /* 4 */
  0x6803, /* 5 */
  0xE803, /* 6 */
  0x3801, /* 7 */
  0xF803, /* 8 */
  0x7803, /* 9 */
  0x8803, /* F [10]*/
  0xB803, /* A [11]*/
  0x3000, /* I [12]*/
  0xC001, /* L [13]*/
  0x0004, /* . [14]*/
  0x0002, /* - [15]*/
  0x0000  /*   [16]*/
};

SEG_I2C segment_disp;


void setup() 
{
  // IO port init
  pinMode(38, OUTPUT);    // RDY DO npn+24V
  pinMode(39, OUTPUT);    // MEAS DO npn+24V
  pinMode(40, OUTPUT);    // END DO npn+24V
  pinMode(41, OUTPUT);    // Blank
  pinMode(42, OUTPUT);    // Blank
  pinMode(43, OUTPUT);    // Blank
  pinMode(44, OUTPUT);    // Blank
  pinMode(45, OUTPUT);    // Blank
  pinMode(46, OUTPUT);    // Blank
  pinMode(47, OUTPUT);    // Blank
  pinMode(48, OUTPUT);    // Blank
  pinMode(49, OUTPUT);    // RDY lamp
  pinMode(50, OUTPUT);    // RUN lamp
  pinMode(51, OUTPUT);    // END lamp

  pinMode(22, INPUT);
  pinMode(23, INPUT);
  pinMode(24, INPUT);
  pinMode(25, INPUT);
  pinMode(26, INPUT);
  pinMode(27, INPUT);
  pinMode(28, INPUT);
  pinMode(29, INPUT);
  pinMode(30, INPUT);
  pinMode(31, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(36, INPUT);
  pinMode(37, INPUT);
  delay(10);

  // I2C port init
  Wire.begin();             // Master mode
  Wire.setClock(100000);    // 4stage:~200KHz, At 1stage 400K is possible but doesn't work at 4stage
  
  // Serial port init
  Serial.begin(9600);       // uploading port
  Serial.setTimeout(100);   // wait time for serial data
  delay(10);
  Serial3.begin(9600);     // RS485 input port
  Serial3.setTimeout(100);  // wait time for serial data
  delay(100);

  lampControl(ALL_ON);    
  segment_disp.begin(HT16K33_ADDRESS_1);
  delay(50);
  segment_disp.setBrightness(HT16K33_ADDRESS_1, 9);
  delay(50);
  segment_disp.clear(HT16K33_ADDRESS_1);
  delay(50);
  segment_disp.displayOn(HT16K33_ADDRESS_1);
  delay(50);
  uint8_t disp_test[8]= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  Wire.beginTransmission(HT16K33_ADDRESS_1);
  Wire.write(0x00);
  Wire.write(disp_test,8);
  Wire.endTransmission();
  delay(500);
  lampControl(ALL_OFF);
  
  // init value display 0
  uint8_t init_display[8]= {0xF8,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
  Wire.beginTransmission(HT16K33_ADDRESS_1);
  Wire.write(0x00);
  Wire.write(init_display,8);
  Wire.endTransmission();
  delay(10);
}

void loop() 
{
  uint16_t d[4];    // data buffer array for display function
  uint8_t rx_buffer[3];  // serial data buffer 
  uint16_t temp;    // convert serial data to decimal 
  //uint16_t exp16[4] = {1, 16, 256, 4096};   // multiply constant to make decimal
  uint16_t exp10[4] = {1, 10, 100, 1000};   // multiply constant to make decimal
  uint8_t dump;   // to throw away countinuous trash data
  
/***** Temp display *****/
  if(Serial3.available()>0)
    delay(200);   // Wait for 200ms from first byte in to check 3 byte input at once 
  
  if(Serial3.available()==3)   // data length 3bytes
  {
    for(uint8_t i=0; i<3; i++)
    {
      rx_buffer[i] = Serial3.read();  
    }
/***** temperature display *****/
    temp = 0;      
    switch(rx_buffer[0])
    {
      case 0x01:    // rx_buffer[0] is end code
        digitalWrite(38, LOW);    
        digitalWrite(39, LOW);
        digitalWrite(40, HIGH);
        digitalWrite(49, LOW);  // The_mag Lamp control
        digitalWrite(50, LOW);
        digitalWrite(51, HIGH);
        temp = ((rx_buffer[1]/16)*1000)+((rx_buffer[1]%16)*100)+((rx_buffer[2]/16)*10)+((rx_buffer[2]%16)*1);
        for(uint8_t i=0;i<4;i++)                    // fill the array with real disp value
        {
          d[i] = numbertable[(temp/exp10[3-i])];  
          temp = temp % exp10[3-i];                 // remain existing number             
        }
        for(uint8_t i=0;i<3;i++)                    // make blank when zero comes first till 10
        {
          if(d[i]==numbertable[0])     
            d[i] = numbertable[16];                 // numbertable[16] is blank 
          else
            break;
        }       
        segment_disp.writeDisplay(HT16K33_ADDRESS_1, &d[3], &d[2], &d[1], &d[0]); 
        delay(50);
        break;

      case 0x04:   // rx_buffer[0] is measure code
        digitalWrite(38, LOW);    
        digitalWrite(39, HIGH);
        digitalWrite(40, LOW);
        digitalWrite(49, LOW);
        digitalWrite(50, HIGH);
        digitalWrite(51, LOW);
        break;
      case 0x03:   // rx_buffer[0] is ready code
        digitalWrite(38, HIGH);    
        digitalWrite(39, LOW);
        digitalWrite(40, LOW);
        digitalWrite(49, HIGH);
        digitalWrite(50, LOW);
        digitalWrite(51, LOW);
        break;
      case 0x08:   // rx_buffer[0] is all off code
        digitalWrite(38, LOW);    
        digitalWrite(39, LOW);
        digitalWrite(40, LOW);
        digitalWrite(49, LOW);
        digitalWrite(50, LOW);
        digitalWrite(51, LOW);
        break;    
    }
  }
  else
  {
    for(uint8_t i=0; i<Serial3.available(); i++)    // if trash data in continuously, throw away
    {
      dump = Serial3.read();  
    } 
  } 
  delay(10);      
}
  
