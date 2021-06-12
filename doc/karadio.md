# KaRadio and KaRadio32

Both project are web-radio implementation. First one is based on ESP8266
while the second is based on ESP-32.

Both projects have the same UART-based interface, so that Ampcontrol can
communicate with webradio

## Ka-Radio

Clone https://github.com/karawin/Ka-Radio

There is `ESP8266-Firmware` directory in the project with some useful
instructions how to build and flash the project. In `bin` subfolders
there are precompiled binaries.

Erasing ESP32:
```
esptool.py --baud 115200 --port /dev/ttyUSB0 erase_flash
```

Flash Ka-Radio:
```
esptool.py --baud 115200 --port /dev/ttyUSB0 write_flash \
    0x0000000 "./bin/boot_v1.6.bin" \
    0x0001000 "./bin/upgrade/user1.4096.new.4.bin" \
    0x0081000 "./bin/upgrade/user2.4096.new.4.bin" \
    0x03fc000 "./bin/esp_init_data_default.bin" \
    0x03fe000 "./bin/blank.bin" \
    -fs 4MB
```

## Ka-Radio32

Clone https://github.com/karawin/Ka-Radio32
