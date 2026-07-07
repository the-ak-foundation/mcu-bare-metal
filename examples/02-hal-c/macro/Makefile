NAME_MODULE		= led_blink
OBJECTS_DIR 	= build_$(NAME_MODULE)

GCC_PATH       = $(HOME)/Workspace/Tools/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux/gcc-arm-none-eabi-10.3-2021.10
PROGRAMER_PATH = $(HOME)/Workspace/Tools/STM32CubeProgrammer/bin
OPENOCD_CFG_PATH	= /usr/share/openocd/scripts/board/stm32ldiscovery.cfg

APP_START_ADDR = 0x08000000

CFLAGS  = -mcpu=cortex-m3 -mthumb -Os -g -Wall -ffreestanding -nostdlib -DAPP_START_ADDR=$(APP_START_ADDR)
LDFLAGS = -T stm32l151xx.ld -nostdlib -Wl,--gc-sections

ELF = $(OBJECTS_DIR)/$(NAME_MODULE).elf
BIN = $(OBJECTS_DIR)/$(NAME_MODULE).bin

## GDB command for debugging
ARM_GDB = $(GCC_PATH)/bin/arm-none-eabi-gdb-py

all: $(BIN)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(ELF): $(NAME_MODULE).c | $(OBJECTS_DIR)
	$(GCC_PATH)/bin/arm-none-eabi-gcc $(CFLAGS) $(LDFLAGS) $< -o $@

$(BIN): $(ELF)
	$(GCC_PATH)/bin/arm-none-eabi-objcopy -O binary $< $@

flash: $(BIN)
	$(PROGRAMER_PATH)/STM32_Programmer.sh -c port=SWD -w $< $(APP_START_ADDR) -rst

debug: $(ELF)
	xterm -e openocd -f $(OPENOCD_CFG_PATH) &
	sleep 1
ifeq ($(gdb),ddd)
	ddd --debugger $(ARM_GDB) --command=stm32l_init.gdb $(ELF)
else
	$(ARM_GDB) -w --command=stm32l_init.gdb $(ELF)
endif

clean:
	rm -rf $(OBJECTS_DIR)

.PHONY: all clean flash debug
