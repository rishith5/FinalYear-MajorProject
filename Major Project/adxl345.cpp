
adxl345.h
#ifndef ADXL345_H
#define ADXL345_H
#include <Wire.h>
class ADXL345 {
  public:
    void begin();
    void read(int16_t &x, int16_t &y, int16_t &z);
}; #endif
