include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE ?= arm-linux-gnueabi-

OBJDUMP = $(CROSS_COMPILE)objdump

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

# CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -msoft-float -fsingle-precision-constant -Wdouble-promotion -Wfloat-conversion
CFLAGS_ARM9 = -march=armv4t -marm -msoft-float -fsingle-precision-constant -Wdouble-promotion -Wfloat-conversion
# CFLAGS_CUSTOM = -fno-builtin-printf
CFLAGS = $(INC) -Wall -nostdlib  $(CFLAGS_ARM9) $(CFLAGS_CUSTOM) 

LDFLAGS =  -T s3c2440.ld -Map=$@.map --cref --gc-sections

CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
# CFLAGS += -fdata-sections -ffunction-sections
endif

LIBS = mylibc.a libgcc.a

SRC_C = \
	main.c \
	nand.c \
	uart_core.c \
	modmachine.c \
	example.c \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

# SRC_C += lib/libc/string0.c

SRC_S = \
	start.S \

# OBJ = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.S=.o))
OBJ = $(addprefix $(BUILD)/, $(SRC_S:.S=.o) $(SRC_C:.c=.o)) $(PY_CORE_O)

all: $(BUILD)/firmware.bin

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -S $< -O binary $@
	$(Q)$(OBJDUMP) -D -m arm $< > $(BUILD)/firmware.dis

include $(TOP)/py/mkrules.mk
