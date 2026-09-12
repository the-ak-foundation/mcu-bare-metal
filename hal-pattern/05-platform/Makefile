NAME_MODULE ?= led_blink
PROJECT_DIR ?= examples/ak_base_kit/gpio/$(NAME_MODULE)
OBJECTS_DIR  = build_$(NAME_MODULE)

MCU ?= stm32l1

ifeq ($(MCU),stm32l1)
MCU_FLAGS = -mcpu=cortex-m3 -mthumb
else ifeq ($(MCU),rp2040)
MCU_FLAGS = -mcpu=cortex-m0plus -mthumb
else
$(error unknown MCU: $(MCU))
endif

GCC_PATH         = $(HOME)/workspace/tools/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux/gcc-arm-none-eabi-10.3-2021.10
PROGRAMER_PATH   = $(HOME)/workspace/tools/STM32CubeProgrammer/bin
OPENOCD_CFG_PATH = /usr/share/openocd/scripts/board/stm32ldiscovery.cfg

APP_START_ADDR = 0x08000000

CROSS   = $(GCC_PATH)/bin/arm-none-eabi-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
SIZE    = $(CROSS)size

CFLAGS  = $(MCU_FLAGS) -std=c99 -Os -g -Wall -Wextra \
          -ffunction-sections -fdata-sections -fsigned-char -fmessage-length=0 \
          -DSTM32L151xB -MMD -MP \
          -Icmsis -Iboard/ak_base_kit \
          -Ihal/inc/api -Ihal/inc/instances \
          -Ihal/src/bsp/mcu/all -Ihal/src/bsp/mcu/stm32l1 \
          -I$(PROJECT_DIR)/hal_cfg/bsp -I$(PROJECT_DIR)/hal_cfg/driver \
          -I$(PROJECT_DIR)/hal_gen

LDFLAGS = $(MCU_FLAGS) -T script/stm32l151cbtx_flash.ld \
          -Wl,--gc-sections -Wl,-Map=$(OBJECTS_DIR)/$(NAME_MODULE).map \
          --specs=nano.specs --specs=nosys.specs
LDLIBS  = -lc -lm -lnosys

ELF = $(OBJECTS_DIR)/$(NAME_MODULE).elf
BIN = $(OBJECTS_DIR)/$(NAME_MODULE).bin

## GDB command for debugging
ARM_GDB = $(GCC_PATH)/bin/arm-none-eabi-gdb-py

CMSIS_SRC = \
    cmsis/system_stm32l1xx.c

BSP_SRC = \
    hal/src/bsp/mcu/all/bsp_common.c \
    hal/src/bsp/mcu/all/bsp_delay.c \
    hal/src/bsp/mcu/stm32l1/bsp_clocks.c

HAL_SRC = \
    hal/src/stm32l1_gpio/stm32l1_gpio.c \
    hal/src/stm32l1_uart/stm32l1_uart.c \
    hal/src/stm32l1_tim/stm32l1_tim.c

BOARD_SRC = \
    board/ak_base_kit/board_init.c \
    board/ak_base_kit/board_leds.c

GEN_SRC = \
    $(PROJECT_DIR)/hal_gen/hal_data.c \
    $(PROJECT_DIR)/hal_gen/pin_data.c

APP_SRC = $(PROJECT_DIR)/src/hal_entry.c

STARTUP_SRC = cmsis/startup_stm32l151xb.s

C_SRC = $(CMSIS_SRC) $(BSP_SRC) $(HAL_SRC) $(BOARD_SRC) $(GEN_SRC) $(APP_SRC)
OBJ   = $(addprefix $(OBJECTS_DIR)/,$(C_SRC:.c=.o) $(STARTUP_SRC:.s=.o))
DEP   = $(OBJ:.o=.d)

all: $(BIN)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(ELF): $(OBJ) | $(OBJECTS_DIR)
	$(CC) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $@
	$(SIZE) $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(OBJECTS_DIR)/%.o: %.c | $(OBJECTS_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECTS_DIR)/%.o: %.s | $(OBJECTS_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

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

-include $(DEP)

.PHONY: all clean flash debug
