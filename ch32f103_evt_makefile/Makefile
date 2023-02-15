######################################
# target
######################################
TARGET = ch32f


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
C_SOURCES = \
CMSIS/core_cm3.c \
StdPeriphDriver/src/ch32f10x_bkp.c \
StdPeriphDriver/src/ch32f10x_flash.c \
StdPeriphDriver/src/ch32f10x_spi.c \
StdPeriphDriver/src/ch32f10x_usb.c \
StdPeriphDriver/src/ch32f10x_dma.c \
StdPeriphDriver/src/ch32f10x_gpio.c \
StdPeriphDriver/src/ch32f10x_crc.c \
StdPeriphDriver/src/ch32f10x_tim.c \
StdPeriphDriver/src/ch32f10x_rcc.c \
StdPeriphDriver/src/ch32f10x_usart.c \
StdPeriphDriver/src/ch32f10x_rtc.c \
StdPeriphDriver/src/ch32f10x_i2c.c \
StdPeriphDriver/src/ch32f10x_misc.c \
StdPeriphDriver/src/ch32f10x_exti.c \
StdPeriphDriver/src/ch32f10x_adc.c \
StdPeriphDriver/src/ch32f10x_can.c \
StdPeriphDriver/src/ch32f10x_iwdg.c \
StdPeriphDriver/src/ch32f10x_dbgmcu.c \
StdPeriphDriver/src/ch32f10x_wwdg.c \
StdPeriphDriver/src/ch32f10x_usb_host.c \
StdPeriphDriver/src/ch32f10x_dac.c \
StdPeriphDriver/src/ch32f10x_pwr.c \
User/system_ch32f10x.c \
User/Main.c \
User/ch32f10x_it.c \
Debug/debug.c \


# ASM sources
ASM_SOURCES =  \
Startup/startup_ch32f10x.S \


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-

CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

AS_DEFS =

# C defines
C_DEFS = 

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ICMSIS \
-IDebug \
-IStdPeriphDriver/inc \
-IUser

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
LDSCRIPT = Ld/Link.ld 

# libraries
LIBS = -lc -lm -lnosys
LIBDIR = 
LDFLAGS = $(MCU) -specs=nosys.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
#$(LUAOBJECTS) $(OBJECTS)
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
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
