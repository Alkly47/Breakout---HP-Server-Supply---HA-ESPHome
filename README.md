# Breakout---HP-Server-Supply---HA-ESPHome

Firmware to integrate my breakout board into Home Assistant via ESPHome
- Modified from the KCORES Open Source code found here:
  https://github.com/KCORES/KCORES-CSPS-to-ATX-Converter/tree/main/Software

This yaml creates a PMBus/i2c device compatible with my power supply breakout board and an attached SSD1306 OLED display. Built with Arduino Framework for ESP8266. (Currently untested on any ESP32 variants)

Exposes: (All data comes from PSU except ADS1115)
 - Embedded information about the PSU ("Name", Manufacturer, Date Code, "Spare Part" Number, "Option Kit" Number, and "CT" Barcode)
 - Fan speed reporting and control (Minimum Limit)
 - Intake Air Temperature and Internal PSU Temperature
 - AC Input: Voltage (Volts), Current (Amps), and Power (Watts)
 - DC Output: Voltage (Volts), Current (Amps), Power (Watts), and "Peak" Current (An Amps value that resets to zero each time the output cycles "off", or the power supply loses AC power)
 - ADS1115 on the breakout board measuring three separate voltages and the PSU current shunt.

Note: Current and Power reporting from the PSU is questionable at low load. Utilizing the data from the ADS115 at low loads is more reliable.

# Updates:
 - MIN/MAX (In the filename), Add new software sensors that calculate the minimum and maximum voltages from the other voltage sensors. Intended to be used on the OLED display, so the "Peak Amps" page has been restructured to also display the min/max values and show dynamic timers to indicate the state of the displayed data.
   The min/max values reset when the power supply cycles (turns on or off): reset is immediate when the power supply cycles on, when the power supply cycles off there is a ten minute delay before the min/max values 'expire' (A timer is visible on the OLED display to indicate the time remaining).
   Min/Max timers: 
     - there is a timer on the OLED min/max page that shows when a reset has occurred less than an hour ago
     - there is a timer on the OLED min/max page that shows a ten minute countdown when values are 'frozen' after the power supply cycles off. When the timer reaches Zero, the 'frozen' values are purged.
    
# To Use:
 - Create new device in ESPhome, copy yaml and modify with appropriate api, ota, and wifi creds.
 - Install to device.

**Pre-compiled binary (`.bin`) can be found in the `firmware` folder. Recommended to compile yourself from `yaml` for the latest code improvements and updates


