# TFT display

The project supports Graphic color TFT displays based on various controllers:

- 160x128: ILI9163, S6D0144, ST7735
- 176x132: L2F50126, LPH9157, LS020, SSD1286A
- 220x176: HX8340, ILI9225, LGDP4524, S6D0164
- 320x240: HX8347A, HX8347D, ILI9320, ILI9341, MC2PA8201, S6D0129, S6D0139, SPFD5408, SSD1289, SSD2119
- 400x240: ILI9327, S6D04D1, ST7793
- 480x320: HX8357C, ILI9481, ILI9486, R61581, RM68140, ST7796S

## 8-bit display connection:

D0-D7 (data bus): PB0-PB7
RD (read pin): pull to 3.3V
WR: (write pin): PB15
RS: (register select): PB14
CS: (chip select): PB12
RST: (reset): pull to 3.3V
BCKL: (backlight): PC13

## SPI display connection
CS: (chip select): PB12
RST: (reset): pull to 3.3V
DC: (data/command): PB14
SDI: (slave data input): PB15 (hardware SPI2 MOSI)
SCK: (slave clock): PB13 (hardware SPI2 SCK)
LED: (backlight): PC13
SDO: (slave data ooutput): pull to 3.3V

## Audiocontrollers and FM tuners

Most of the external chips are connected via I²C bus (hardware SPI1)

SDA: PB9
SCL: PB8

Si4703 tuner with 32768kHz quartz also needs reset pin connection:

RST: PA15

## MUTE and STBY control

When entering standby mode, it's useful to disable powering some parts of the whole projects
(like audio amplifier, audioprocessor, etc). So, only ampcontrol control module itself can
be left powered with lower display brighness. STBY pin can be used to control a relay or
another swtitch doing this.

Also, switching between inputs, powering on/off can produce audible clicks in acoustic system.
MUTE pin can be used to mute an amplifier at these moments.

STBY: PB10
MUTE: PB11

## Remote control

Standard TSOP IR receiver is used to get control commands from IR remote.

IR: PA8

## BT201 bluetooth module

USART1 used for communications with the module. So, playback is fully controlled as well as
an information about current song from USB or SDCARD is retreived

TX: PA9
RX: PA10

## Bluetooth modules like BK8000L or CSR8645

These modules don't have known interface to communicate with MCUs. But they have
GPIOs to connect buttons for such functions as PLAY/PAUSE and so on.

To control such kind of bluetooth modules, it's possible to use PCF8574 I²C bus expander
to emulate buttons presses to execute corresponding functions. It's connected to the project
via I²C line:

SDA: PB9
SCL: PB8

Then, the correct I²C address (depends on PCF8574 address input pins conditions)
should be selected in ampcontrol settings.

PCF5874 is connected to the BT module in this way (see i2cexp.h):

NEXT_TRACK: P5
PREV_TRACK: P4
NEXT_INPUT: P3
PLAY_PAUSE: P2

To emulate button press, PCF8574 sends short (200ms) positive impulse on the pin.

## KaRadio or MPD

The device can act as a "plugin" for KaRadio/KaRadio32 projects. The control and all
communications are done via USART2 connection

TX: PA2
RX: PA3

Also, instead of KaRadio it's possible to control MPD (Music Player Daemon). The board with
MPD (e.g. Raspberry Pi or alternative) need to be prepared and it should include mpd-uart.py 
script running. This script makes implements a communication between Ampcontrol's USART2 and
the board's MPD daemon. Python3 packages pyserial and python-mpd2 need to be installed.

## USB

When connectetd to the PC via hardware USB, the project acts as "USB keyboard" and sends
multimedia keys to the PC when a button on front panel or remote control is pressed. Thus,
playback in active multimedia player on PC supporting media keys can be controlled.
