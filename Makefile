PROJECT = ampcontrol_f103

DISPLAY = ILI9341
DISPVAR = SPI

TARGET = $(PROJECT)_$(shell echo $(DISPLAY)_$(DISPVAR) | tr A-Z a-z)

APROC_LIST = TDA7439 TDA731X PT232X
TUNER_LIST = RDA580X SI470X TEA5767
FEATURE_LIST =

C_SOURCES = main.c

C_SOURCES += actions.c
C_SOURCES += eemap.c
C_SOURCES += eemul.c
C_SOURCES += fft.c
C_SOURCES += handlers.c
C_SOURCES += i2c.c
C_SOURCES += input.c
C_SOURCES += menu.c
C_SOURCES += pins.c
C_SOURCES += rc.c
C_SOURCES += rtc.c
C_SOURCES += screen.c
C_SOURCES += spectrum.c
C_SOURCES += swtimers.c
C_SOURCES += timers.c
C_SOURCES += tr/labels.c
C_SOURCES += tr/labels_by.c
C_SOURCES += tr/labels_ru.c
C_SOURCES += tr/labels_tr.c
C_SOURCES += tr/labels_ua.c

C_DEFS = -DUSE_FULL_LL_DRIVER -DSTM32F103xB

# Display source files
C_SOURCES += $(wildcard display/fonts/font*.c)
C_SOURCES += $(wildcard display/icons/icon*.c)

ifeq "$(DISPLAY)" "ILI9163"
  C_SOURCES += display/gc160x128/ili9163.c
else ifeq "$(DISPLAY)" "ST7735"
  C_SOURCES += display/gc160x128/st7735.c
else ifeq "$(DISPLAY)" "L2F50126"
  C_SOURCES += display/gc176x132/l2f50126.c
else ifeq "$(DISPLAY)" "LPH9157"
  C_SOURCES += display/gc176x132/lph9157.c
else ifeq "$(DISPLAY)" "LS020"
  C_SOURCES += display/gc176x132/ls020.c
else ifeq "$(DISPLAY)" "SSD1286A"
  C_SOURCES += display/gc176x132/ssd1286a.c
else ifeq "$(DISPLAY)" "HX8340"
  C_SOURCES += display/gc220x176/hx8340.c
else ifeq "$(DISPLAY)" "ILI9225"
  C_SOURCES += display/gc220x176/ili9225.c
else ifeq "$(DISPLAY)" "ILI9320"
  C_SOURCES += display/gc320x240/ili9320.c
else ifeq "$(DISPLAY)" "ILI9341"
  C_SOURCES += display/gc320x240/ili9341.c
else ifeq "$(DISPLAY)" "MC2PA8201"
  C_SOURCES += display/gc320x240/mc2pa8201.c
else ifeq "$(DISPLAY)" "S6D0139"
  C_SOURCES += display/gc320x240/s6d0139.c
else ifeq "$(DISPLAY)" "SPFD5408"
  C_SOURCES += display/gc320x240/spfd5408.c
else ifeq "$(DISPLAY)" "ILI9327"
  C_SOURCES += display/gc400x240/ili9327.c
else ifeq "$(DISPLAY)" "ST7793"
  C_SOURCES += display/gc400x240/st7793.c
else ifeq "$(DISPLAY)" "ILI9481"
  C_SOURCES += display/gc480x320/ili9481.c
else ifeq "$(DISPLAY)" "ILI9486"
  C_SOURCES += display/gc480x320/ili9486.c
else ifeq "$(DISPLAY)" "R61581"
  C_SOURCES += display/gc480x320/r61581.c
endif
C_SOURCES += display/dispdrv.c
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)
C_DEFS += -D_DISP_LO_BYTE

C_SOURCES += canvas/canvas.c
C_SOURCES += canvas/layout.c
C_SOURCES += canvas/lt160x128.c
C_SOURCES += canvas/lt176x132.c
C_SOURCES += canvas/lt220x176.c
C_SOURCES += canvas/lt320x240.c
C_SOURCES += canvas/lt400x240.c
C_SOURCES += canvas/lt480x320.c

# Audio source files
ifeq "$(findstring TDA7439, $(APROC_LIST))" "TDA7439"
  C_SOURCES += audio/tda7439.c
endif
ifeq "$(findstring TDA731X, $(APROC_LIST))" "TDA731X"
  C_SOURCES += audio/tda731x.c
endif
ifeq "$(findstring PT232X, $(APROC_LIST))" "PT232X"
  C_SOURCES += audio/pt232x.c
endif
C_SOURCES += audio/audio.c
C_DEFS += $(addprefix -D_, $(APROC_LIST))

# Tuner source files
ifeq "$(findstring RDA580X, $(TUNER_LIST))" "RDA580X"
  C_SOURCES += tuner/rda580x.c
endif
ifeq "$(findstring SI470X, $(TUNER_LIST))" "SI470X"
  C_SOURCES += tuner/si470x.c
endif
ifeq "$(findstring TEA5767, $(TUNER_LIST))" "TEA5767"
  C_SOURCES += tuner/tea5767.c
endif
C_SOURCES += tuner/stations.c
C_SOURCES += tuner/tuner.c
C_DEFS += $(addprefix -D_, $(TUNER_LIST))

# USB source files
C_SOURCES += \
  usb/usbd_amp.c \
  usb/usbd_conf.c \
  usb/usbd_desc.c \
  usb/usbhid.c

C_INCLUDES += \
  -Iusb

C_SOURCES += \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_i2c.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c \
  drivers/STM32_USB_Device_Library/Core/Src/usbd_core.c \
  drivers/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
  drivers/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
  system/system_stm32f1xx.c

C_INCLUDES += \
  -Idrivers/STM32F1xx_HAL_Driver/Inc \
  -Idrivers/STM32_USB_Device_Library/Core/Inc \
  -Idrivers/CMSIS/Device/ST/STM32F1xx/Include \
  -Idrivers/CMSIS/Include \
  -Isystem

AS_DEFS +=

ASM_SOURCES += \
  system/startup_stm32f103xb.s

# Build directory
BUILD_DIR = build

DEBUG = 1

# Compiler
FPU =
FLOAT-ABI =
MCU = -mcpu=cortex-m3 -mthumb $(FPU) $(FLOAT-ABI)
OPT = -Os -fshort-enums -ffunction-sections -fdata-sections -ffreestanding
WARN = -Wall -Werror
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(WARN)
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) $(WARN)
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
# Dependency information
CFLAGS += -MMD -MP -MT $(BUILD_DIR)/$(*F).o -MF $(BUILD_DIR)/$(*D)/$(*F).d

LDSCRIPT = system/stm32f103c8tx_flash.ld
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nosys.specs -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
#LDFLAGS = $(WARN) -mmcu=$(MCU) -Wl,--gc-sections -Wl,--relax

# Main definitions
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
AR = $(PREFIX)ar
SZ = $(PREFIX)size

OPENOCD := openocd
OPENOCD_CFG := system/stm32f10x-openocd.cfg

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = flash/$(TARGET).bin

FW_BACKUP = backup/$(TARGET).bin
EE_BACKUP = backup/$(PROJECT)_eeprom.bin

all: $(BIN) size

$(BIN): $(ELF)
	$(CP) -O binary $(ELF) $(BIN)
	$(OD) -h -S $(ELF) > $(BUILD_DIR)/$(TARGET).lss

$(ELF): $(OBJECTS)
	@mkdir -p flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJECTS)

size: $(ELF)
	$(SZ) $(ELF)

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(C_DEFS) -o $@ $<

$(BUILD_DIR)/%.o: %.s Makefile
	@mkdir -p $(dir $@)
	$(AS) -c $(CFLAGS) $(AS_DEFS) -o $@ $<

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: flash
flash: $(BIN)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_flash $(BIN)" -c shutdown

.PHONY: erase
erase:
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_erase" -c shutdown

.PHONY: backup
backup:
	@mkdir -p backup
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_read $(FW_BACKUP) $(EE_BACKUP)" -c shutdown

.PHONY: ee_flash
ee_flash: $(EE_BACKUP)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_ee_flash $(EE_BACKUP)" -c shutdown

.PHONY: ee_erase
ee_erase:
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_ee_erase" -c shutdown

# Other dependencies
-include $(OBJECTS:.o=.d)
