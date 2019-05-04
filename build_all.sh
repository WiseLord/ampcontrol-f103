#!/bin/sh

MAKEOPTS="-j4 -s"

compile_display()
{
  local DISPLAY=${1}
  local DISPVARS=${2}

  for DISPVAR in ${DISPVARS}
  do
  echo -e "\nCompiling display ${DISPLAY} ${DISPVAR}"
    make clean_disp
    make ${MAKEOPTS} DISPLAY=${DISPLAY} DISPVAR=${DISPVAR}
  done
}

make clean

# 160x128 color displays
compile_display "S6D0144"   "8BIT"
compile_display "ILI9163"   "8BIT"
compile_display "ST7735"    "8BIT"

# 176x132 color displays
compile_display "L2F50126"  "SPI"
compile_display "LPH9157"   "SPI"
compile_display "LS020"     "SPI"
compile_display "SSD1286A"  "SPI"

# 220x176 color displays
compile_display "HX8340"    "8BIT"
compile_display "ILI9225"   "8BIT SPI"
compile_display "LGDP4524"  "8BIT"
compile_display "S6D0164"   "8BIT"

# 320x240 color displays
compile_display "HX8347A"   "8BIT"
compile_display "HX8347D"   "8BIT"
compile_display "ILI9320"   "8BIT"
compile_display "ILI9341"   "8BIT SPI"
compile_display "MC2PA8201" "8BIT"
compile_display "S6D0129"   "8BIT"
compile_display "S6D0139"   "8BIT"
compile_display "SPFD5408"  "8BIT"
compile_display "SSD1289"   "8BIT"
compile_display "SSD2119"   "8BIT"

# 400x240 color displays
compile_display "ILI9327"   "8BIT"
compile_display "S6D04D1"   "8BIT"
compile_display "ST7793"    "8BIT"

# 480x320 color displays
compile_display "ILI9481"   "8BIT"
compile_display "ILI9486"   "8BIT"
compile_display "R61581"    "8BIT"
compile_display "RM68140"   "8BIT"

make clean
