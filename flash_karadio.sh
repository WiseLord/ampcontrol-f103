#!/bin/bash

# KaRadio project and binaries:
# https://github.com/karawin/Ka-Radio

# Erase all
esptool.py --baud 115200 --port /dev/ttyUSB0 erase_flash

# Flash firmware
esptool.py --baud 115200 --port /dev/ttyUSB0 write_flash \
    0x0000000 "./bin/boot_v1.7.bin" \
    0x0001000 "./bin/upgrade/user1.4096.new.4.bin" \
    0x0081000 "./bin/upgrade/user2.4096.new.4.bin" \
    -fs 4MB

# Flash default settings
esptool.py --baud 115200 --port /dev/ttyUSB0 write_flash \
    0x03fe000 "./bin/blank.bin" \
    0x00fe000 "./bin/blank.bin" \
    0x007E000 "./bin/blank.bin" \
    0x03fc000 "./bin/esp_init_data_default.bin" \
    0x00FC000 "./bin/esp_init_data_default.bin" \
    -fs 4MB
