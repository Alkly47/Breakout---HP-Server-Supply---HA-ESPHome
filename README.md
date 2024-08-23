# Breakout---HP-Server-Supply---HA-ESPHome
Firmware to integrate my breakout board into Home Assistant via ESPHome
- Modified from the KCORES Open Source code found here:
  https://github.com/KCORES/KCORES-CSPS-to-ATX-Converter/tree/main/Software

This code creates a PMBus/i2c custom ESPHome component compatible with my breakout board and an attached SSD1306 OLED display. Built with Arduino Framework for ESP8266. (The code as-is doesn't work on ESP32 or it's variants)

Exposes: (All data comes from PSU except ADS1115)
 - Embedded information about the PSU ("Name", Manufacturer, Manufactured Date, "Spare Part" Number, "Option Kit" Number, and "CT" Barcode)
 - Fan speed reporting and control (Minimum Limit)
 - Intake Air Temperature and Internal PSU Temperature
 - AC Input: Voltage, Current, and Power
 - DC Output: Voltage, Current, Power, and "Peak" Current (A value that resets to zero each time the output cycles "off")
 - ADS1115 on the breakout board measuring three separate voltages and the PSU current shunt

Usage:
 - Pull the full repo and copy to ESPHome directory in Home Assistant, maintaining folder structure and file-names (***Important***). 
 - Modify yaml with appropriate api, ota, and wifi creds.
 - Install to device.

 - *During compile, there are warnings that can be ignored. (The result of code being manipulated and forced to work with ESPHome)*
 - *Due to i2c/PMBus limitations, the working bus speed is not quite fast enough to avoid warnings/errors about OLED display "taking too long". These do not affect functionality and can be ignored.*

Note: Current and Power reporting is questionable at low load. Utilizing the data from the ADS115 at low loads is more reliable.
