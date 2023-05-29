#
# Swamp - cooperative multitasking operating system
# Copyright (c) 2016 rksdna
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Project files

TARGET ?= app
SCRIPT = ch32v003.ld

HEX = $(TARGET).hex
BIN = $(TARGET).bin
ELF = $(TARGET).elf
MAP = $(TARGET).map
LST = $(TARGET).lst
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

# Tools and flags

PREFIX = riscv64-unknown-elf-

CC = $(PREFIX)gcc
LD = $(PREFIX)gcc
OC = $(PREFIX)objcopy
OD = $(PREFIX)objdump
SZ = $(PREFIX)size
BS = minichlink
RM = rm -f

FLAGS = -march=rv32ec -mabi=ilp32e -ffreestanding -nostdinc -nostdlib -nostartfiles
CFLAGS = $(FLAGS) -Wall -Os -g -MD $(INC) $(DEF)
LFLAGS = $(FLAGS) -T $(SCRIPT) -Wl,-Map=$(MAP)

# Targets

.PHONY: all clean install

all: $(HEX) $(LST)

$(HEX): $(ELF)
	@echo "Creating $(HEX)..."
	$(OC) -j .text -j .data -O ihex $< $@

$(BIN): $(ELF)
	@echo "Creating $(HEX)..."
	$(OC) -j .text -j .data -O binary $< $@

$(LST): $(ELF)
	@echo "Creating $(LST)..."
	$(OD) -S -d -g $(ELF) > $(LST)
	$(SZ) $<

$(ELF): $(OBJ)
	@echo "Linking $(ELF)..."
	$(LD) $(LFLAGS)  -o $@ $^ -lgcc

%.o: %.c
	@ echo "Compiling $@..."
	$(CC) -c $(CFLAGS) -o $@ $<

install: $(BIN)
	@echo "Installing $(BIN)..."
	$(BS) -w $(BIN) flash -b

return: $(BIN)
	@echo "Installing $(BIN)..."
	$(BS) -w blink.bin flash -b

clean:
	@echo "Cleaning..."
	$(RM) $(OBJ) $(DEP) $(ELF) $(MAP) $(HEX) $(BIN) $(LST)

-include $(DEP)
