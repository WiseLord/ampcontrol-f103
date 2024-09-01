# Ampcontrol (STM32F103 / F303 version)

Ampcontrol is an control module for your DIY audio amplifier.
It supports various I²C-controlled audioprocessors and FM tuners.
Also, some other third-party devices are supported.

Some videos related to the device can be found at
[Youtube](https://www.youtube.com/playlist?list=PLfv57leyFFd1C61HgnKE3e_OFIXMBtf-D)

## Hardware support

- Graphic color TFT displays based on various controllers:
  - 160x128: ILI9163, S6D0144, ST7735;
  - 176x132: L2F50126, LPH9157, LS020, SSD1286A;
  - 220x176: HX8340, ILI9225, LGDP4524, S6D0164;
  - 320x240: HX8347A, HX8347D, ILI9320, ILI9341, MC2PA8201, S6D0129, S6D0139, SPFD5408, SSD1289, SSD2119;
  - 400x240: ILI9327, S6D04D1, ST7793;
  - 480x320: HX8357C, ILI9481, ILI9486, R61581, RM68140, ST7796S.

- Audio processors with I²C bus:
  - TDA7439, TDA731X, PT232X, TDA7418, TDA7719.

- FM tuners with I²C bus:
  - RDA580X, SI470X, TEA5767.

- Bluetooth modules
  - CSR8645, BK8000L (GPIO-based control via PCF8574x I²C expander);
  - BT201 (USART-based control).

- USB HID
  - Meida player on the PC can be controlled (Play/Pause and so on) by the device.

- KaRadio and MPD support
  - The device can act as "KaRadio plugin" via USART2 to add web radio support via ESP32. Alternatively, 
it can control MPD server running on Linux (for example, Raspberry PI).

More specific info can be found in [documentation](doc) pages.

## Building the code

[GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
is required to build the project.

The project build system is based on [GNU Make](https://www.gnu.org/software/make/).
Linux users must have it already installed or they can easily do it.
For Windows users there is a port exists.
In both cases, both toolchain and make binaries should be added to the system PATH.

The process of building is very simple and doesn't depend on any other software installed.

### Default build (ILI9341 display with SPI wiring):

`make`

### Build for the specific STM32 MCUs, display and wiring:

`make STM32_MCU=F303CB DISPLAY=R61581 DISPVAR=8BIT`

The list of supported display controllers and MCUs can be found in Makefile or build_all.sh script.

## Schematics and wiring

The schematic and PCB files for the device itself in KiCad format can be found 
in [kicad/ampcontrol-f103](kicad/ampcontrol-f103) directory.

## Useful links

* [Firmware releases](https://github.com/WiseLord/ampcontrol-f103/releases)

* [The article (in Russian)](https://radiokot.ru/artfiles/6484/)

* [Forum thread (in Russian)](https://radiokot.ru/forum/viewtopic.php?f=2&t=158605)

* [Youtube playlist](https://www.youtube.com/playlist?list=PLfv57leyFFd1C61HgnKE3e_OFIXMBtf-D)
