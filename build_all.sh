#!/bin/bash

cd src/

MAKEOPTS="-j$(nproc) -s"
ret=0

STM32_MCU_LIST="F103CB F303CC F303CB"

compile_display()
{
  local STM32_MCU=${1}
  local DISPLAY=${2}
  local DISPVARS=${3}

  for DISPVAR in ${DISPVARS}
  do
    make clean
    echo -e "Compiling display ${DISPLAY} ${DISPVAR}"
    make ${MAKEOPTS} STM32_MCU=${STM32_MCU} DISPLAY=${DISPLAY} DISPVAR=${DISPVAR} \
      || ret=$((ret + 1))
  done
}

for STM32_MCU in ${STM32_MCU_LIST}
do
  echo -e "========== Compiling for ${STM32_MCU} =========="

  # 160x128 color displays
  compile_display ${STM32_MCU} "ILI9163"   "8BIT SPI"
  compile_display ${STM32_MCU} "S6D0144"   "8BIT"
  compile_display ${STM32_MCU} "ST7735"    "8BIT SPI"

  # 176x132 color displays
  compile_display ${STM32_MCU} "L2F50126"  "SPI"
  compile_display ${STM32_MCU} "LPH9157"   "SPI"
  compile_display ${STM32_MCU} "LS020"     "SPI"
  compile_display ${STM32_MCU} "SSD1286A"  "SPI"

  # 220x176 color displays
  compile_display ${STM32_MCU} "HX8340"    "8BIT"
  compile_display ${STM32_MCU} "ILI9225"   "8BIT SPI"
  compile_display ${STM32_MCU} "LGDP4524"  "8BIT"
  compile_display ${STM32_MCU} "S6D0164"   "8BIT"

  # 320x240 color displays
  compile_display ${STM32_MCU} "HX8347A"   "8BIT"
  compile_display ${STM32_MCU} "HX8347D"   "8BIT"
  compile_display ${STM32_MCU} "ILI9320"   "8BIT"
  compile_display ${STM32_MCU} "ILI9341"   "8BIT SPI"
  compile_display ${STM32_MCU} "MC2PA8201" "8BIT"
  compile_display ${STM32_MCU} "S6D0129"   "8BIT"
  compile_display ${STM32_MCU} "S6D0139"   "8BIT"
  compile_display ${STM32_MCU} "SPFD5408"  "8BIT"
  compile_display ${STM32_MCU} "SSD1289"   "8BIT"
  compile_display ${STM32_MCU} "SSD2119"   "8BIT"

  # 400x240 color displays
  compile_display ${STM32_MCU} "ILI9327"   "8BIT"
  compile_display ${STM32_MCU} "S6D04D1"   "8BIT"
  compile_display ${STM32_MCU} "ST7793"    "8BIT"

  # 480x320 color displays
  compile_display ${STM32_MCU} "HX8357C"   "8BIT"
  compile_display ${STM32_MCU} "ILI9481"   "8BIT"
  compile_display ${STM32_MCU} "ILI9486"   "8BIT"
  compile_display ${STM32_MCU} "R61581"    "8BIT"
  compile_display ${STM32_MCU} "RM68140"   "8BIT"
  compile_display ${STM32_MCU} "ST7796S"   "8BIT"
done

make clean

exit $ret
