DISPLAY = KS0108
DISPVAR = 8BIT_B

APROC_LIST = TDA7439 TDA731X PT232X
TUNER_LIST =
FEATURE_LIST =

TARGET = ampcontrol_f103_$(shell echo $(DISPLAY)_$(DISPVAR) | tr A-Z a-z)

C_SOURCES = main.c

C_SOURCES += actions.c
C_SOURCES += eemul.c
C_SOURCES += fft.c
C_SOURCES += functions.c
C_SOURCES += handlers.c
C_SOURCES += i2c.c
C_SOURCES += input.c
C_SOURCES += pins.c
C_SOURCES += rtc.c
C_SOURCES += screen.c
C_SOURCES += spectrum.c
C_SOURCES += timers.c

C_DEFS = -DUSE_FULL_LL_DRIVER -DSTM32F103xB

# Display source files
FONTS_SRC = $(wildcard display/fonts/font*.c)
ICONS_SRC = $(wildcard display/icon*.c)
C_SOURCES += $(FONTS_SRC)
C_SOURCES += $(ICONS_SRC)

ifeq "$(DISPLAY)" "KS0108"
  C_SOURCES += display/gm128x64/ks0108.c
  C_SOURCES += display/gm128x64/gm128x64.c
else ifeq "$(DISPLAY)" "ST7920"
  C_SOURCES += display/gm128x64/st7920.c
  C_SOURCES += display/gm128x64/gm128x64.c
else ifeq "$(DISPLAY)" "SSD1306"
  C_SOURCES += display/gm128x64/ssd1306.c
  C_SOURCES += display/gm128x64/gm128x64.c
else ifeq "$(DISPLAY)" "ILI9163"
  C_SOURCES += display/gc160x128/ili9163.c
  C_SOURCES += display/gc160x128/gc160x128.c
else ifeq "$(DISPLAY)" "ST7735"
  C_SOURCES += display/gc160x128/st7735.c
  C_SOURCES += display/gc160x128/gc160x128.c
else ifeq "$(DISPLAY)" "LS020"
  C_SOURCES += display/gc176x132/ls020.c
  C_SOURCES += display/gc176x132/gc176x132.c
else ifeq "$(DISPLAY)" "LPH9157"
  C_SOURCES += display/gc176x132/lph9157.c
  C_SOURCES += display/gc176x132/gc176x132.c
else ifeq "$(DISPLAY)" "SSD1286A"
  C_SOURCES += display/gc176x132/ssd1286a.c
  C_SOURCES += display/gc176x132/gc176x132.c
else ifeq "$(DISPLAY)" "HX8340"
  C_SOURCES += display/gc220x176/hx8340.c
  C_SOURCES += display/gc220x176/gc220x176.c
else ifeq "$(DISPLAY)" "ILI9320"
  C_SOURCES += display/gc320x240/ili9320.c
  C_SOURCES += display/gc320x240/gc320x240.c
else ifeq "$(DISPLAY)" "ILI9341"
  C_SOURCES += display/gc320x240/ili9341.c
  C_SOURCES += display/gc320x240/gc320x240.c
else ifeq "$(DISPLAY)" "S6D0139"
  C_SOURCES += display/gc320x240/s6d0139.c
  C_SOURCES += display/gc320x240/gc320x240.c
else ifeq "$(DISPLAY)" "SPFD5408"
  C_SOURCES += display/gc320x240/spfd5408.c
  C_SOURCES += display/gc320x240/gc320x240.c
else ifeq "$(DISPLAY)" "MC2PA8201"
  C_SOURCES += display/gc320x240/mc2pa8201.c
  C_SOURCES += display/gc320x240/gc320x240.c
else ifeq "$(DISPLAY)" "ILI9327"
  C_SOURCES += display/gc400x240/ili9327.c
  C_SOURCES += display/gc400x240/gc400x240.c
else ifeq "$(DISPLAY)" "ST7793"
  C_SOURCES += display/gc400x240/st7793.c
  C_SOURCES += display/gc400x240/gc400x240.c
endif
C_SOURCES += display.c
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)

# Audio source files
ifeq "$(findstring TDA7439, $(APROC_LIST))" "TDA7439"
  C_SOURCES += audio/tda7439.c
endif
C_SOURCES += audio/audio.c
C_DEFS += $(addprefix -D_, $(APROC_LIST))

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
	@rm -rf $(BUILD_DIR)

.PHONY: flash
flash: $(BIN)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_flash $(BIN)" -c shutdown

.PHONY: erase
erase:
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_erase" -c shutdown

# Other dependencies
-include $(OBJECTS:.o=.d)
