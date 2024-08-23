## Pre-Compiled firmware for easy setup. ##
Flash .bin file of choice to ESP8266 device using ESPHome Web Flasher. (https://web.esphome.io/)

Versions:
 - `hp-server-psu-webserver-api.bin`  Compiled with both Home Assistant and a Web Server component. (Open device IP in web browser to see all exposed sensors/data)
 - `hp-server-psu-no_webserver-api.bin` Compiled with Home Assistant API only, no Web Server component included.
 - `hp-server-psu-webserver-no_api.bin` Compiled with Web Server component only, no Home Assistant API included.

All versions immediately open a HOTSPOT on fresh boot:
 - SSID: "HP Server PSU"
 - Password: "123456789"

Connect to the hotspot and navigate to the captive portal ("Sign into this network") to enter your Wifi Credentials.

Versions that include Home Assistant API become directly discoverable by Home Assistant via the ESPHome Add-On.

YAML Files used during compile are included for inspection, but are not utilized when flashing a BIN file.
If modifications are desired, you need to self-compile.
