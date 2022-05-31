# ESP32 TouchDown
[![](https://img.shields.io/badge/buy-on%20Tindie-blue)](https://www.tindie.com/products/dustinwattsnl/esp32-touchdown/)
[![](https://img.shields.io/discord/693862273864827012?color=5165f6&label=chat%20on%20Discord)](https://discord.gg/RE3XevS)
[![](https://badgen.net/github/license/DustinWatts/esp32-touchdown)](https://github.com/DustinWatts/esp32-touchdown/blob/main/LICENSE)
[![](https://badgen.net/badge/watch/on%20youtube/ff0000)](https://www.youtube.com/watch?v=sdVtHU2Gz7Y)
[![](https://img.shields.io/twitter/follow/DustinWattsNL)](https://twitter.com/DustinWattsNL)



## Where to buy?

Available from:

- Tindie: -> https://www.tindie.com/products/dustinwattsnl/esp32-touchdown/ (Ships from The Netherlands)

- Eplop Electornics -> https://store.eplop.co.uk/product/esp32touchdown/ (Ships from the UK)

- PCBWay -> https://www.pcbway.com/project/gifts_detail/ESP32_TouchDown.html (Ships from China) 

## FreeTouchDeck

#### Your ESP32 TouchDown comes with FreeTouchDeck pre-installed. Check out this simple start guide: [With FreeTouchDeck pre-installed](https://github.com/DustinWatts/esp32-touchdown/wiki/With-FreeTouchDeck-pre-installed)

![ESP32 TouchDown](/Images/osh.JPG)

![ESP32 TouchDown](http://www.dustinwatts.nl/ESP32-TouchDown/ESP32_TouchDown_features.png)

While working on [FreeTouchDeck](https://github.com/DustinWatts/FreeTouchDeck), I really wanted to use capacitive touch. Unfortunately, most modules I could find were resistive touch and capacitive touch has a much nicer user experience. While I was designing, it made sense to put the ESP32 on the board itself, removing the need for an ESP32 Dev board. I wanted to take full advantage of the ESP32 wireless capabilities (Wifi, BLE) so I included a battery management circuit. ESP32 TouchDown can run off of a LiPo battery and also charge it. Here is a full list of the features:

- ESP32-WROOM-32D
- ILI9488 480*320 TFT screen in 4-wire SPI mode
- FT62x6 Capacitive Touch Controller
- APK2112 3.3V regulator _(On V1.1 of the board an AP7365 is used, see changelog.txt)_
- MCP73831 battery management IC _(On V1.1 of the board a Shouding SD8016 is used, see changelog.txt)_
- CP2102 USB-to-UART bridge
- USB-C connector
- microSD card holder
- Battery voltage divider connected to GPIO35
- Passive buzzer connected to GPIO26
- 12 unused GPIO's broken out
- Stemma / JST-PH I2C connector

ESP32 TouchDown works out of the box (as ESP32 Dev Module) with the Arduino IDE, providing you have installed the ESP32 Arduino Core. If you have not, this is where you can find it: [https://github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32).

## Documentation:

Documentation can be found in the [Wiki](https://github.com/DustinWatts/esp32-touchdown/wiki).

## TFT screen and touch controller

ESP32 TouchDown uses an 480x320 screen with an ILI9488 ([datasheet](https://github.com/DustinWatts/esp32-touchdown/blob/main/Hardware/Datasheets/ILITEK_ILI9488.pdf)) driver. There are numerous drivers out there, but I found TFT_eSPI by Bodmer the most versatile. [https://github.com/Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI). 

Pins used by the TFT screen are:

- GPIO2 -> DC_RS
- GPIO4 -> TFT_RESET
- GPIO15 -> TFT_CS
- GPIO18 -> SPI Clock
- GPIO23 -> SDI (MOSI)

The TFT backlight anode (positive supply) is selectable via a jumper on the back. You can either power it directly from 3.3V or use GPIO32. By default, the positive source is GPIO32. You can this pin and use PWM to control the backlight brightness.

_Note: On V1.1 of the board the jumpers used are with bridged pads. So by default have no solder blob. See changelog.txt_

![TFT Backlight Select](http://www.dustinwatts.nl/ESP32-TouchDown/backlight_select.png)

The touch controller is a FocalTech FT62x6 ([datasheet](https://github.com/DustinWatts/esp32-touchdown/blob/main/Hardware/Datasheets/FT6236-FocalTechSystems.pdf)). The first batch uses an FT6236 and the second batch of boards has the FT6206. Both have the same I2C address of 0X38. I made an Arduino IDE library available here: [https://github.com/DustinWatts/FT6236](https://github.com/DustinWatts/FT6236). This library also supports the FT6202.

Pins used by the FT62x6 are:

- GPIO21 -> I2C data
- GPIO22 -> I2C clock
- GPIO27 -> IRQ

## GPIO Breakout

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

## SD Card

The following pins are used by the SD card holder:

- GPIO 25 --> CD/DAT3
- GPIO 23 --> CMD
- GPIO 18 --> CLK
- GPIO 19 --> DAT0

*CD (card detect) DAT1 and DAT2 are not connected.*

## Uploading code to the ESP32 TouchDown

For FreeTouchDeck installation instructions see: https://github.com/DustinWatts/FreeTouchDeck/wiki/1.-Installation

For other code: The ESP32 TouchDown uses the same configuration as an ESP32 Dev Module. So you can use those board definitions to upload your own code to the ESP32 TouchDown:
![Arduino IDE board settings](http://www.dustinwatts.nl/ESP32-TouchDown/docs/board_settings.png)

## Open Source

Everything about the ESP32 TouchDown is open source. This means I provide you will all the designs I make. In this repository, you will find the KiCad project, including schematic, and board layout. Also, the complete BOM is available to you. I have included the datasheets of all the hardware used.

<div style="margin:0 auto;">
  
  <div> 
    <img src="http://www.dustinwatts.nl/ESP32-TouchDown/license.svg" alt="License" width="200"/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <img src="/Images/oshw-mark-nl000004-transp-900x660.png" alt="OSHW" width="160"/>
  </div>

</div>

## Support Me

If you like what I am doing, there are a number of ways you can support me. 

| Platform | Link|
|:-----:|:-----|
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/twitter_logo.png" alt="Twitter" width="24"/>](https://twitter.com/dustinwattsnl "Follow me on Twitter") | You can follow me on Twitter: [@dustinwattsnl](https://twitter.com/dustinwattsnl "Follow me on Twitter")|
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/youtube_logo.png" alt="YouTube" width="32"/>](https://www.youtube.com/dustinwatts "Subscrive to my YouTube channel") | You can subscribe to my channel on Youtube: [/dustinWatts](https://www.youtube.com/dustinwatts "Subscribe to my YouTube channel") |
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/patreon_logo.png" alt="Patreon" width="32"/>](https://www.patreon.com/dustinwatts) | You can support me by becoming a patron on Patreon: https://www.patreon.com/dustinwatts |
| [<img src="https://github.com/DustinWatts/small_logos/blob/main/paypalme_logo.png" alt="PayPal.me" width="32"/>](https://www.paypal.me/dustinwattsnl) | You can make a one time donation using PayPal.me: https://www.paypal.me/dustinwattsnl |

## Help

For quick access to help you can join my Discord server where I have a dedicated #esp32-touchdown channel. https://discord.gg/RE3XevS
