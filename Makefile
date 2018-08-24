DISPLAY = ILI9320
DISPVAR = 8BIT

APROC_LIST =
TUNER_LIST =
FEATURE_LIST =

TARGET = ampcontrol_f103_$(shell echo $(DISPLAY)_$(DISPVAR) | tr A-Z a-z)

C_SOURCES = main.c

C_SOURCES += actions.c
C_SOURCES += fft.c
C_SOURCES += functions.c
C_SOURCES += handlers.c
C_SOURCES += i2c.c
C_SOURCES += input.c
C_SOURCES += lcd.c
C_SOURCES += pins.c
C_SOURCES += rtc.c
C_SOURCES += screen.c
C_SOURCES += spectrum.c
C_SOURCES += timers.c

C_DEFS = -DUSE_FULL_LL_DRIVER -DSTM32F103xB

# Display source files
FONTS_SRC = $(wildcard display/font*.c)
ICONS_SRC = $(wildcard display/icon*.c)
C_SOURCES += $(FONTS_SRC)
C_SOURCES += $(ICONS_SRC)

ifeq "$(DISPLAY)" "KS0108"
  C_SOURCES += display/ks0108.c
  C_SOURCES += display/gm128x64.c
else ifeq "$(DISPLAY)" "ST7920"
  C_SOURCES += display/st7920.c
  C_SOURCES += display/gm128x64.c
else ifeq "$(DISPLAY)" "SSD1306"
  C_SOURCES += display/ssd1306.c
  C_SOURCES += display/gm128x64.c
else ifeq "$(DISPLAY)" "LS020"
  C_SOURCES += display/ls020.c
  C_SOURCES += display/gc176x132.c
else ifeq "$(DISPLAY)" "LPH9157"
  C_SOURCES += display/lph9157.c
  C_SOURCES += display/gc176x132.c
else ifeq "$(DISPLAY)" "SSD1286A"
  C_SOURCES += display/ssd1286a.c
  C_SOURCES += display/gc176x132.c
else ifeq "$(DISPLAY)" "ILI9320"
  C_SOURCES += display/ili9320.c
  C_SOURCES += display/gc320x240.c
else ifeq "$(DISPLAY)" "ILI9341"
  C_SOURCES += display/ili9341.c
  C_SOURCES += display/gc320x240.c
else ifeq "$(DISPLAY)" "S6D0139"
  C_SOURCES += display/s6d0139.c
  C_SOURCES += display/gc320x240.c
else ifeq "$(DISPLAY)" "SPFD5408"
  C_SOURCES += display/spfd5408.c
  C_SOURCES += display/gc320x240.c
endif
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_$(DISPVAR)

C_SOURCES += \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_i2c.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
  drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c \
  system/system_stm32f1xx.c

C_INCLUDES = \
  -Idrivers/STM32F1xx_HAL_Driver/Inc \
  -Idrivers/CMSIS/Device/ST/STM32F1xx/Include \
  -Idrivers/CMSIS/Include

AS_DEFS =

ASM_SOURCES = \
system/startup_stm32f103xb.s

# Build directory
BUILD_DIR = build

DEBUG = 1

# Compiler
FPU =
FLOAT-ABI =
MCU = -mcpu=cortex-m3 -mthumb $(FPU) $(FLOAT-ABI)
OPT = -Os -fshort-enums -ffunction-sections -fdata-sections
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
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
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
	rm -rf $(BUILD_DIR)

.PHONY: flash
flash: $(BIN)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_flash $(BIN)" -c shutdown

# Other dependencies
-include $(OBJECTS:.o=.d)
