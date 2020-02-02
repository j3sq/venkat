.SUFFIXES:

DEVICE = atmega328p
MCU = atmega328p
AVRDUDE_DEVICE = m328p
DEVICE ?= atmega168
MCU ?= atmega168
AVRDUDE_DEVICE ?= m168

# If test goal then set ARCH to PC
ifeq ("$(MAKECMDGOALS)","test")
  ARCH := PC
else
  ARCH := AVR_3PI
endif

# Verbose control
Q = @

# Variables for target build
CFLAGS=-g -Wall -mcall-prologues -MMD -mmcu=$(MCU) $(DEVICE_SPECIFIC_CFLAGS) -D$(ARCH) -Os
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax

# Compiler flags for simple test environment
CFLAGS_TEST=-O0 -g3 -Wall -c -MMD -fmessage-length=0 -D$(ARCH)

# Programming tool
PORT ?= /dev/ttyACM0
AVRDUDE=avrdude

# Build file name
TARGET=volvorobot

# List files to be included in target build. List .o files.
OBJ := main.o
OBJ += orienteering.o
OBJ += nodesLog.o
OBJ += follow_line.o
OBJ += debug_print.o
OBJ += markers.o
OBJ += utils.o
OBJ += tsm.o

# Files for test build
TEST_OBJ += test.testobj
TEST_OBJ += orienteering.testobj
TEST_OBJ += nodesLog.testobj
TEST_OBJ += follow_line.testobj
TEST_OBJ += debug_print.testobj
TEST_OBJ += markers.testobj
TEST_OBJ += utils.testobj
TEST_OBJ += readTestData.testobj

# Dependancy handling
-include $(patsubst %.o,%.d,$(OBJ))

# Rules for target
.SECONDARY:
all: $(TARGET).hex

clean:
	rm -f *.o *.testobj *.hex *.elf *.hex *.aout *.lst *.d

%.hex: %.elf 
	$(Q)$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.o: %.c
	$(Q)echo 'Compiling $(notdir $<)'
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

%.elf: $(OBJ)
	$(Q)echo 'Linking $@'
	$(Q)$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

program: $(TARGET).hex
	$(Q)$(AVRDUDE) -p $(AVRDUDE_DEVICE) -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex

# Rules for testing in PC environment	
test: $(TARGET).aout
	./$(TARGET).aout

%.testobj: %.c
	$(Q)echo 'Compiling $(notdir $<) for test'	
	$(Q)gcc -O0 -g3 $(CFLAGS_TEST) $< -o $@
	
$(TARGET).aout: $(TEST_OBJ)
	$(Q)echo 'Linking $@ for test'
	$(Q)gcc -o $@ $^
