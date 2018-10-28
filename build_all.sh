#!/bin/sh

MAKEOPTS="-j4 -s"

compile_display()
{
  local DISPLAY=${1}
  local DISPVARS=${2}

  for DISPVAR in ${DISPVARS}
  do
  echo -e "\nCompiling display ${DISPLAY} ${DISPVAR}"
    make ${MAKEOPTS} DISPLAY=${DISPLAY} DISPVAR=${DISPVAR}
    make clean
  done
}

make clean

# Monochrome 128x64 displays
compile_display "KS0108A"   "8BIT"
compile_display "KS0108B"   "8BIT"
compile_display "ST7920"    "8BIT"
compile_display "SSD1306"   "I2C"

# 160x128 color displays
compile_display "ILI9163"   "8BIT"
compile_display "ST7735"    "8BIT"

# 176x132 color displays
compile_display "LS020"     "SPI"
compile_display "LPH9157"   "SPI"
compile_display "SSD1286A"  "SPI"

# 220x176 color displays
compile_display "HX8340"    "8BIT"
compile_display "ILI9225"   "8BIT SPI"

# 320x240 color displays
compile_display "ILI9320"   "8BIT"
compile_display "ILI9341"   "8BIT SPI"
compile_display "S6D0139"   "8BIT"
compile_display "SPFD5408"  "8BIT"
compile_display "MC2PA8201" "8BIT"

# 400x240 color displays
compile_display "ILI9327"   "8BIT"
compile_display "ST7793"    "8BIT"

# 480x320 color displays
compile_display "ILI9481"   "8BIT"
compile_display "R61581"    "8BIT"