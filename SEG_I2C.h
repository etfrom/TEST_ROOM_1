#ifndef SEG_I2C_h
#define SEG_I2C_h

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>

#define HT16K33_ADDRESS_1       0x70
#define HT16K33_ADDRESS_2       0x71
#define HT16K33_ADDRESS_3       0x72
#define HT16K33_ADDRESS_4       0x73
#define HT16K33_BLINK_CMD       0x80
#define HT16K33_DISP_ON         0x01
#define HT16K33_DISP_OFF        0x00
#define HT16K33_BLINK_2HZ       0x01
#define HT16K33_BLINK_1HZ       0x02
#define HT16K33_BLINK_0.5HZ     0x03  
#define HT16K33_BRIGHTNESS_CMD  0xE0
 
#define RDY_ON  0x04
#define RUN_ON  0x02
#define END_ON  0x01
#define ALL_OFF  0x08
#define ALL_ON  0x0A


class SEG_I2C
{
  public:
    void begin(uint8_t addr);
    void setBrightness(uint8_t addr, uint8_t br);
    void blinkRate(uint8_t addr, uint8_t bl);
    void displayOn(uint8_t addr);
    void displayOff(uint8_t addr);
    void writeDisplay(uint8_t addr, uint16_t* d0, uint16_t* d1, uint16_t* d2, uint16_t* d3);
    void clear(uint8_t addr);

    uint16_t displaybuffer[4];
    
  private:
    void writeData(uint8_t addr, uint8_t val);             
};

void lampControl(uint8_t signal);

#endif
