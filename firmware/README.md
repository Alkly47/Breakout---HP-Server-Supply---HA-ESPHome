## Pre-Compiled firmware for easy setup. ##
Flash .bin file to ESP8266 device using ESPHome Web Flasher. (https://web.esphome.io/)

All versions open a Wifi HOTSPOT on fresh boot:
 - SSID: "HP Server PSU"
 - Password: "123456789"

Connect to the wifi hotspot and navigate to the captive portal ("Sign into this network") to enter your Wifi Credentials.

This firmware can allow the device to be directly discoverable by Home Assistant via the ESPHome Add-On.

***These binaries are not updated frequently. For the latest code, utilize the `hp-server-psu.yaml` located in the root of this repo, and compile yourself***
