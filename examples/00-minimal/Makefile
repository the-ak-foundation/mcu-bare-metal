##############################################################################
# Makefile - 00-minimal - STM32L151CBT6
##############################################################################

# Utilities define
Print = @echo "~"

# SOC, flash and RAM info
SOC_NAME            = STM32L151CBT6
SOC_FLASH_BYTES     = 131072 # 128KB
SOC_SRAM_BYTES      = 16384  # 16KB
FLASH_BASE_ADDR_VAL = 0x08003000

# Name of build target ex: 00-minimal-application.bin
NAME_MODULE = 00-minimal-application
PROJECT     = $(NAME_MODULE)

GCC_PATH = /home/phuoc/Workspace/Tools/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux/gcc-arm-none-eabi-10.3-2021.10

#|---------------------------------------------------------------------------------------------------|
#| OPTIMIZE LEVELS                                                                                   |
#|------------|----------------------------------|--------------|---------|------------|-------------|
#|   option   | optimization levels              |execution time|code size|memory usage|complile time|
#|------------|----------------------------------|--------------|---------|------------|-------------|
#|   -O0      | compilation time                 |     (+)      |   (+)   |     (-)    |    (-)      |
#|   -Os      | code size                        |              |   (--)  |            |    (++)     |
#|------------|----------------------------------|--------------|---------|------------|-------------|
OPTIMIZE_OPTION = -g3 -O0

# Warning option
WARNING_OPTION = -Wall -Wextra -Werror -Wundef -Wshadow

OBJECTS_DIR = build_$(NAME_MODULE)
TARGET      = $(OBJECTS_DIR)/$(NAME_MODULE).elf
LDFILE      = stm32l151xx.ld

SOURCES = main.c
OBJECTS = $(patsubst %.c, $(OBJECTS_DIR)/%.o, $(SOURCES))

# The command for calling the compiler.
CC       = $(GCC_PATH)/bin/arm-none-eabi-gcc
OBJCOPY  = $(GCC_PATH)/bin/arm-none-eabi-objcopy
OBJDUMP  = $(GCC_PATH)/bin/arm-none-eabi-objdump
ARM_SIZE = $(GCC_PATH)/bin/arm-none-eabi-size

# Set the compiler CPU/FPU options.
CPU = -mthumb -march=armv7-m
FPU = -mfloat-abi=soft

# General flags
GENERAL_FLAGS += \
		$(WARNING_OPTION) \
		$(OPTIMIZE_OPTION) \
		-DSTM32L1XX_MD \
		-DSTM32L151xB

# Compiler flags
COMPILER_FLAGS += \
		$(CPU) \
		$(FPU) \
		-ffunction-sections \
		-fdata-sections \
		-fno-common \
		-fno-builtin \
		-ffreestanding \
		-c

# C compiler flags
CFLAGS += \
		$(GENERAL_FLAGS) \
		$(COMPILER_FLAGS) \
		-std=c99

# Linker flags
LDFLAGS = $(CPU) $(FPU) \
		-T$(LDFILE) \
		-nostartfiles -nostdlib \
		--specs=nano.specs -lc -lgcc \
		-Wl,-Map=$(OBJECTS_DIR)/$(PROJECT).map \
		-Wl,--gc-sections \
		-Wl,--cref

all: create $(TARGET) info

create:
	$(Print) CREATE $(OBJECTS_DIR) folder
	@mkdir -p $(OBJECTS_DIR)

$(TARGET): $(OBJECTS)
	$(Print) LD $@
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(Print) OBJCOPY $(@:.elf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.elf=.bin)
	@$(ARM_SIZE) $(TARGET)

$(OBJECTS_DIR)/%.o: %.c
	$(Print) CC $@
	@$(CC) $(CFLAGS) -o $@ $<

.PHONY: help
help:
	$(Print) "[make]             build firmware"
	$(Print) "[make flash]       burn firmware via ST-Link"
	$(Print) "[make flash dev=X] burn firmware via ak-flash (e.g. dev=/dev/ttyUSB0)"
	$(Print) "[make info]        print SOC, flash and RAM usage"
	$(Print) "[make clean]       clean build project folder"

.PHONY: flash
flash: all
	$(Print) FLASH $(TARGET:.elf=.bin) to target
ifdef dev
	ak-flash $(dev) $(TARGET:.elf=.bin) $(FLASH_BASE_ADDR_VAL)
else
	@st-flash --connect-under-reset --reset write $(TARGET:.elf=.bin) $(FLASH_BASE_ADDR_VAL)
endif

.PHONY: clean
clean:
	$(Print) CLEAN $(OBJECTS_DIR) folder
	@rm -rf $(OBJECTS_DIR)

.PHONY: info
info: $(TARGET)
	@FLASH_USED=`$(ARM_SIZE) -A -d $(TARGET) | awk '$$1==".isr_vector" || $$1==".text" || $$1==".rodata" || $$1==".data" {sum += $$2} END {print sum+0}'`; \
	RAM_USED=`$(ARM_SIZE) -A -d $(TARGET) | awk '$$1==".data" || $$1==".bss" {sum += $$2} END {print sum+0}'`; \
	FLASH_PCT=`awk "BEGIN {printf \"%.2f\", ($$FLASH_USED * 100.0) / $(SOC_FLASH_BYTES)}"`; \
	RAM_PCT=`awk "BEGIN {printf \"%.2f\", ($$RAM_USED * 100.0) / $(SOC_SRAM_BYTES)}"`; \
	echo "MCU name: $(SOC_NAME)"; \
	printf -- " - Flash : %d / %d bytes (%s%%)\n" $$FLASH_USED $(SOC_FLASH_BYTES) $$FLASH_PCT; \
	printf -- " - RAM   : %d / %d bytes (%s%%)\n" $$RAM_USED $(SOC_SRAM_BYTES) $$RAM_PCT