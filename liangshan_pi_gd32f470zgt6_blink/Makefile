######################################
# target
######################################
TARGET = gd32f470zgt6


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization for size
OPT = -Os


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_adc.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_can.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_crc.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_ctc.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_dac.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_dbg.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_dci.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_dma.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_enet.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_exmc.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_exti.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_fmc.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_fwdgt.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_gpio.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_i2c.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_ipa.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_iref.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_misc.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_pmu.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_rcu.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_rtc.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_sdio.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_spi.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_syscfg.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_timer.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_tli.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_trng.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_usart.c \
GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_wwdgt.c \
GD32F4xx_Firmware_Library/CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c \
led/gd32f4xx_it.c \
led/main.c \
led/systick.c

# ASM sources
ASM_SOURCES = GD32F4xx_Firmware_Library/startup_gd32f450_470.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_STDPERIPH_DRIVER \
-DGD32F470


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-Iled \
-IGD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Include \
-IGD32F4xx_Firmware_Library/CMSIS/Include \
-IGD32F4xx_Firmware_Library/CMSIS/GD/GD32F4xx/Include/ \
-IGD32F4xx_Firmware_Library/CMSIS 

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = GD32F4xx_Firmware_Library/gd32f470zgt6.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -u_printf_float -specs=nosys.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# program
#######################################
program:
	openocd -f /usr/share/openocd/scripts/interface/cmsis-dap.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program build/gd32f470zgt6.bin 0x08000000 verify reset exit"

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
