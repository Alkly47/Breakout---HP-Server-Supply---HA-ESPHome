/*!
 * @file KCORES_CSPS.h
 *
 * This is a library for the CSPS PMBUS
 * Written by AlphaArea, Modified by Alkly47
 *
 * GPL license, all text here must be included in any redistribution.
 *
 */

#ifndef KCORES_CSPS_H
#define KCORES_CSPS_H

#include <Arduino.h>
#include <Wire.h>

class CSPS
{
public:
  CSPS(byte CSPS_addr);
  CSPS(byte CSPS_addr, byte ROM_addr);
  CSPS(byte CSPS_addr, byte ROM_addr, bool ENABLE_CHECKSUM);

  String getROM(byte addr, byte len);

  //Spare Part No
  String getSPN()
  {
    return getROM(0x12, 0x0A);
  };

  //Manufacture Date
  String getMFG()
  {
    return getROM(0x1D, 0x08);
  };

  //Manufacturer
  String getMFR()
  {
    return getROM(0x2C, 0x05);
  };

  //Power Name
  String getName()
  {
    return getROM(0x32, 0x1A);
  };

  //Option Kit No
  String getOKN()
  {
    return getROM(0x4D, 0x0A);
  };

  //CT Date Codes
  String getCT()
  {
    return getROM(0x5B, 0x0E);
  };

  float getInputVoltage()
  {
    return (float)readCSPSword(0x08) / 32;
  };

  float getInputCurrent()
  {
    return (float)readCSPSword(0x0A) / 64;
  };

  float getInputPower()
  {
    return (float)readCSPSword(0x0C);
  };

  float getOutputVoltage()
  {
    return (float)readCSPSword(0x0E) / 254.5;
  };

  float getOutputCurrent()
  {
    return (float)readCSPSword(0x10) / 64;
  };

  float getOutputPower()
  {
    return (float)readCSPSword(0x12);
  };

  float getTemp1()
  {
    return (float)readCSPSword(0x1A) / 64;
  };

  float getTemp2()
  {
    return (float)readCSPSword(0x1C) / 64;
  };

  uint16_t getFanRPM()
  {
    return readCSPSword(0x1E);
  };

  uint32_t getRunTime()
  {
    return readCSPSword(0x30) / 2;
    // Strangely, this register maxes out with a 16-bit integer (65535) and counts exactly 2 ticks per second, hence the divide by 2.
    // Therefore, this register is kind of pointless: 32767.5 seconds max, a little more than 9 hours, then the number simply stops changing....
  };

  uint32_t getPeakAmpsOut()
  {
    return readCSPSword(0x36) / 64;
  };

  void setFanRPM(uint16_t rpm)
  {
    writeCSPSword(0x40, rpm);
  };

private:
  bool _CSPS_READ_CHECKSUM;
  byte _CSPS_addr;
  byte _ROM_addr;
  byte readROM(byte dataAddr);

  uint32_t readCSPSword(byte dataAddr);
  void writeCSPSword(byte dataAddr, unsigned int data);
};

#endif
