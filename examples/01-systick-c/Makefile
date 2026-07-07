VARIANT     ?= macro
NAME_MODULE = led_blink
SRC_DIR     = $(VARIANT)
OBJECTS_DIR = build_$(NAME_MODULE)_$(VARIANT)

GCC_PATH         = $(HOME)/Workspace/Tools/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux/gcc-arm-none-eabi-10.3-2021.10
PROGRAMER_PATH   = $(HOME)/Workspace/Tools/STM32CubeProgrammer/bin
OPENOCD_CFG_PATH = /usr/share/openocd/scripts/board/stm32ldiscovery.cfg

APP_START_ADDR = 0x08000000

CFLAGS  = -mcpu=cortex-m3 -mthumb -Os -g -Wall -ffreestanding -nostdlib -I$(SRC_DIR) -DAPP_START_ADDR=$(APP_START_ADDR)
LDFLAGS = -T stm32l151xx.ld -nostdlib -Wl,--gc-sections

ELF = $(OBJECTS_DIR)/$(NAME_MODULE).elf
BIN = $(OBJECTS_DIR)/$(NAME_MODULE).bin

ARM_GDB = $(GCC_PATH)/bin/arm-none-eabi-gdb-py

all: $(BIN)

macro:
	$(MAKE) VARIANT=macro all

struct:
	$(MAKE) VARIANT=struct all

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(ELF): $(SRC_DIR)/$(NAME_MODULE).c | $(OBJECTS_DIR)
	$(GCC_PATH)/bin/arm-none-eabi-gcc $(CFLAGS) $(LDFLAGS) $< -o $@

$(BIN): $(ELF)
	$(GCC_PATH)/bin/arm-none-eabi-objcopy -O binary $< $@

flash: $(BIN)
	$(PROGRAMER_PATH)/STM32_Programmer.sh -c port=SWD -w $< $(APP_START_ADDR) -rst

debug: $(ELF)
	xterm -e openocd -f $(OPENOCD_CFG_PATH) &
ifeq ($(gdb),ddd)
	ddd --debugger $(ARM_GDB) --command=stm32l_init.gdb $(ELF)
else
	$(ARM_GDB) -w --command=stm32l_init.gdb $(ELF)
endif

clean:
	rm -rf build_$(NAME_MODULE)_*

.PHONY: all clean flash debug macro struct
