#
# Project Directories
#
WORKING_DIR = $(shell pwd)
COMMON_DIR  = ${WORKING_DIR}/../utils
CORES_DIR   = ${WORKING_DIR}/cores
VARIANT_DIR = ${WORKING_DIR}/mega
BINARY_DIR  = ${WORKING_DIR}

#
# Program settings
#
BIN  = at89sprog

CCC_SRCS = $(shell ls ${CORES_DIR}/*.c 2>/dev/null) \
		   $(shell ls ${COMMON_DIR}/*.c 2>/dev/null)

CXX_SRCS = $(shell ls ${CORES_DIR}/*.cpp 2>/dev/null) \
		   $(shell ls ${WORKING_DIR}/*.cpp 2>/dev/null) \
		   $(shell ls ${COMMON_DIR}/*.cpp 2>/dev/null) 

ASM_SRCS = $(shell ls ${CORES_DIR}/*.S 2>/dev/null) \

CCC_OBJS = $(CCC_SRCS:.c=.o)
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
ASM_OBJS = $(ASM_SRCS:.S=.o)

OBJS = $(CCC_OBJS) $(CXX_OBJS) $(ASM_OBJS)

#
# Compiler toolchains 
#
AVR_COMPILER = avr-
CC  = ${AVR_COMPILER}gcc
CXX = ${AVR_COMPILER}g++
AR  = ${AVR_COMPILER}ar
LD  = ${AVR_COMPILER}ld
OBJCOPY = ${AVR_COMPILER}objcopy
SIZE = ${AVR_COMPILER}size

#
# Compiling configurations
#
MMCU = atmega2560
BAUD = 115200
FCPU = 16000000UL
AVRDUDE_CONF = ${WORKING_DIR}/avrdude.conf

CCC_FLAGS = -c -Os -Wall -Wextra -std=gnu11 -mmcu=${MMCU} -DF_CPU=${FCPU}
CXX_FLAGS = -c -Os -Wall -Wextra -std=gnu++11 -xc++ -mmcu=${MMCU} -DF_CPU=${FCPU}
ASM_FLAGS = -c -xassembler-with-cpp -mmcu=${MMCU} -DF_CPU=${FCPU}
EFLAGS = -Wall -Wextra -Os -Wl,--gc-sections,--relax -mmcu=${MMCU}
HFLAGS = -v -O ihex -R .eeprom -S

IFLAGS = -I ${WORKING_DIR} \
		 -I ${CORES_DIR} \
		 -I ${VARIANT_DIR} \
		 -I ${COMMON_DIR}

${BIN}.hex: ${BIN}.elf
	@echo "========================="
	@echo "||  Building HEX file  ||"
	@echo "========================="
	${OBJCOPY} ${HFLAGS} $< $@
	${SIZE} $@
	@echo "\n"

${BIN}.elf: ${OBJS}
	@echo "========================="
	@echo "||   Making ELF file   ||"
	@echo "========================="
	${CC} ${EFLAGS} $^ -o $@
	@echo "\n"

.S.o:
	$(CC) $(ASM_FLAGS) $(IFLAGS) -c $< -o $@
	@echo "\n"
.cpp.o:
	$(CXX) $(CXX_FLAGS) $(IFLAGS) -c $< -o $@
	@echo "\n"
.c.o:
	$(CC) $(CCC_FLAGS) $(IFLAGS) -c $< -o $@
	@echo "\n"

install: ${BIN}.hex
	@echo "============================================"
	@echo "||   Burning HEX file to ARDUINO device   ||"
	@echo "============================================"
	avrdude -v -c wiring -D \
		-C ${AVRDUDE_CONF} \
		-p ${MMCU} \
		-P ${PORT} -b ${BAUD} \
		-U flash:w:$<:i
	@echo "\n"

clean:
	@echo "============================"
	@echo "||   Cleaning all files   ||"
	@echo "============================"
	rm -f ${CCC_OBJS}
	@echo ""
	rm -f ${CXX_OBJS}
	@echo ""
	rm -f ${ASM_OBJS}
	@echo ""
	rm -f ${BIN}.elf
	@echo ""
	rm -f ${BIN}.hex
	@echo "\n"
