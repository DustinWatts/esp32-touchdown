# ESP32 TouchDown

![ESP32 TouchDown](http://www.dustinwatts.nl/ESP32-TouchDown/ESP32_TouchDown_2.jpg)

![ESP32 TouchDown](http://www.dustinwatts.nl/ESP32-TouchDown/ESP32_TouchDown_features.png)

The ESP32 TouchDown was born out of a need for a capacitive touch screen module. Most modules I could find where resistive touch
and capacitive touch has a much nicer user experience. While I was designing the module it made sense to put the ESP32 on the board itself, removing the need for an ESP32 Dev board. I wanted to take full advantage of the ESP32 wireless capabilities (Wifi, BLE) so I included a battery management circuit. ESP32 TouchDown can run of a LiPo battery and also charge it. Here is a full list of the features:

- ESP32-WROOM-32D
- ILI9488 480*320 TFT screen in 4-wire SPI mode
- FT6236 Capacitive Touch Controller
- APK2112 3.3V regulator
- MCP73831 battery management IC
- CP2102 USB-to-UART bridge
- USB-C connector
- microSD card holder
- Battery voltage divider connected to GPIO35
- Passive buzzer connected to GPIO26
- 12 unused GPIO's broken out
- Stemma / JST-PH I2C connector

ESP32 TouchDown works out of the box with the Arduino IDE, providing you have installed the ESP32 Arduino Core. If you have not, this is where you can find it: [https://github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32).

# Documentation:

Documentation can be found in the [Wiki](https://github.com/DustinWatts/esp32-touchdown/wiki).

# TFT screen and touch controller

ESP32 TouchDown uses an 480x320 screen with an ILI9488 ([datasheet](https://github.com/DustinWatts/esp32-touchdown/blob/main/Hardware/Datasheets/ILITEK_ILI9488.pdf)) driver. There are numerous drivers out there, but I found TFT_eSPI by Bodmer the most versatile. [https://github.com/Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI). 

Pins used by the TFT screen are:

- GPIO2 -> DC_RS
- GPIO4 -> TFT_RESET
- GPIO15 -> TFT_CS
- GPIO18 -> SPI Clock
- GPIO23 -> SDI (MOSI)

The TFT backlight anode (positive supply) is selectable via a jumper on the back. You can either power it directly from 3.3V or use GPIO32. By default, the positive source is 3.3V. You can change this and use PWM to control the backlight brightness.

![TFT Backlight Select](http://www.dustinwatts.nl/ESP32-TouchDown/backlight_select_tft.png)

The touch controller is a FocalTech FT6236 ([datasheet](https://github.com/DustinWatts/esp32-touchdown/blob/main/Hardware/Datasheets/FT6236-FocalTechSystems.pdf)). The FT6236 uses I2C and has address 0X38. I made an Arduino IDE library available here: [https://github.com/DustinWatts/FT6236](https://github.com/DustinWatts/FT6236).

Pins used by the FT6236 are:

- GPIO21 -> I2C data
- GPIO22 -> I2C clock
- GPIO27 -> IRQ

# GPIO Breakout

The following GPIO's are broken out on the header:

- GPIO 12
- GPIO 13
- GPIO 14
- GPIO 16
- GPIO 17
- SCK (GPIO 18, used as SPI clock)
- SDO (GPIO 19, used as SPI MOSI)
- SDA (GPIO 21, used as I2C data)
- SCL (GPIO 22, used as I2C clock)
- SDI (GPIO 23, used as SPI MISO)
- GPIO 33
- GPIO 34 (note: INPUT only!)

# SD Card

The following pins are used by the SD card holder:

- GPIO 25 --> CD/DAT3
- GPIO 23 --> CMD
- GPIO 18 --> CLK
- GPIO 19 --> DAT0

*CD (card detect) DAT1 and DAT2 are not connected.*

# Open Source

Everything about the ESP32 TouchDown is open source. This means I provide you will all the designs I make. In this repository you will find the Kicad project, including scematic and board layout. Also the complete BOM is available to you. I inculded all the datasheets of all the hardware used.

![License](http://www.dustinwatts.nl/ESP32-TouchDown/license.svg)


