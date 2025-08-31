# Breakout---HP-Server-Supply---HA-ESPHome

# ESPHome version 2025.2 removed "custom components"
# This repo has been updated to remove the dependency on any "custom component"

Firmware to integrate my breakout board into Home Assistant via ESPHome
- Modified from the KCORES Open Source code found here:
  https://github.com/KCORES/KCORES-CSPS-to-ATX-Converter/tree/main/Software

This yaml creates a PMBus/i2c device compatible with my power supply breakout board and an attached SSD1306 OLED display. Built with Arduino Framework for ESP8266. (Currently untested on any ESP32 variants)

Exposes: (All data comes from PSU except ADS1115)
 - Embedded information about the PSU ("Name", Manufacturer, Date Code, "Spare Part" Number, "Option Kit" Number, and "CT" Barcode)
 - Fan speed reporting and control (Minimum Limit)
 - Intake Air Temperature and Internal PSU Temperature
 - AC Input: Voltage, Current, and Power
 - DC Output: Voltage, Current, Power, and "Peak" Current (A value that resets to zero each time the output cycles "off", or the power supply loses AC power)
 - ADS1115 on the breakout board measuring three separate voltages and the PSU current shunt

Note: Current and Power reporting from the PSU is questionable at low load. Utilizing the data from the ADS115 at low loads is more reliable.

# To Use:
 - Create new device in ESPhome, copy yaml and modify with appropriate api, ota, and wifi creds.
 - Install to device.

**Pre-compiled binary (`.bin`) can be found in the `firmware` folder.


