PROJECT = ampcontrol_f103

DISPLAY = ILI9341
DISPVAR = SPI

APROC_LIST = TDA7439 TDA731X PT232X TDA7418
TUNER_LIST = RDA580X SI470X TEA5767
FEATURE_LIST =

F10X_MCU = STM32F103xB
#DEBUG_ENABLED = YES

# Lowercase argument
lc = $(shell echo $1 | tr '[:upper:]' '[:lower:]')

TARGET = $(call lc, $(PROJECT)_$(DISPLAY)_$(DISPVAR))


C_DEFS = -DUSE_FULL_LL_DRIVER -D$(F10X_MCU)

ifeq "$(DEBUG_ENABLED)" "YES"
  C_DEFS += -D_DEBUG_ENABLED
endif

DISP_HI_BYTE = NO
DISP_LO_BYTE = YES

ifeq "$(DISP_HI_BYTE)" "YES"
  C_DEFS += -D_DISP_HI_BYTE
endif
ifeq "$(DISP_LO_BYTE)" "YES"
  C_DEFS += -D_DISP_LO_BYTE
endif

C_SOURCES = main.c

C_SOURCES += actions.c
C_SOURCES += debug.c
C_SOURCES += eemap.c
C_SOURCES += eemul.c
C_SOURCES += fft.c
C_SOURCES += handlers.c
C_SOURCES += i2c.c
C_SOURCES += input.c
C_SOURCES += mem.c
C_SOURCES += menu.c
C_SOURCES += pins.c
C_SOURCES += rc.c
C_SOURCES += rtc.c
C_SOURCES += screen.c
C_SOURCES += settings.c
C_SOURCES += spectrum.c
C_SOURCES += swtimers.c
C_SOURCES += timers.c
C_SOURCES += usart.c
C_SOURCES += tr/labels.c
C_SOURCES += $(wildcard tr/labels_*.c)

# Display source files
C_SOURCES += $(wildcard display/fonts/font*.c)
C_SOURCES += $(wildcard display/icons/icon*.c)

ifneq (,$(filter $(DISPLAY), \
  ILI9163   \
  S6D0144   \
  ST7735    \
))
  DISPSIZE = 160x128
endif

ifneq (,$(filter $(DISPLAY), \
  L2F50126  \
  LPH9157   \
  LS020     \
  SSD1286A  \
))
  DISPSIZE = 176x132
endif

ifneq (,$(filter $(DISPLAY), \
  HX8340    \
  ILI9225   \
  LGDP4524  \
  S6D0164   \
))
  DISPSIZE = 220x176
endif

ifneq (,$(filter $(DISPLAY), \
  HX8347A   \
  HX8347D   \
  ILI9320   \
  ILI9341   \
  MC2PA8201 \
  S6D0129   \
  S6D0139   \
  SPFD5408  \
  SSD1289   \
  SSD2119   \
))
  DISPSIZE = 320x240
endif

ifneq (,$(filter $(DISPLAY), \
  ILI9327   \
  S6D04D1   \
  ST7793    \
))
  DISPSIZE = 400x240
endif

ifneq (,$(filter $(DISPLAY), \
  ILI9481   \
  ILI9486   \
  R61581    \
  RM68140   \
))
  DISPSIZE = 480x320
endif

C_SOURCES += display/gc$(DISPSIZE)/$(call lc,$(DISPLAY)).c
C_SOURCES += display/dispdrv.c
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)
C_DEFS += -D_DISP_$(DISPSIZE)

C_SOURCES += gui/canvas.c
C_SOURCES += gui/layout.c
C_SOURCES += gui/lt$(DISPSIZE).c

# Audio source files
C_SOURCES += $(addprefix audio/, $(addsuffix .c, $(call lc, $(APROC_LIST))))
C_SOURCES += audio/audio.c
C_DEFS += $(addprefix -D_, $(APROC_LIST))

# Tuner source files
C_SOURCES += $(addprefix tuner/, $(addsuffix .c, $(call lc, $(TUNER_LIST))))
C_SOURCES += tuner/rds.c
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
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usart.c \
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
  system/startup_$(call lc, $(F10X_MCU)).s

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

clean_disp:
	@rm -f $(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(wildcard display/gc*/*.c)))
	@rm -f $(BUILD_DIR)/dispdrv.o $(BUILD_DIR)/layout.o $(BUILD_DIR)/pins.o

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
