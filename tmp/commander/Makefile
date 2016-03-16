#
# Program settings
#
TARGET = at89sprog
WORKING_DIR = $(shell pwd)
COMMON_DIR  = ${WORKING_DIR}/../utils
BINARY_DIR  = ${WORKING_DIR}

SRCS = $(shell ls ${WORKING_DIR}/*.c 2>/dev/null) \
	   $(shell ls ${COMMON_DIR}/*.c 2>/dev/null) \
	   $(shell ls ${COMMON_DIR}/*.cpp 2>/dev/null) \

TMP_OBJS = $(SRCS:.c=.o)
OBJS = $(TMP_OBJS:.cpp=.o)

#
# Compiler configuration
#
CC = g++
AR = ar
LD = ld

#
# OS tools
#
RM = rm -rf

#
# Compiling configurations
#
LIBS   = -lpthread
CFLAGS = -Wall -Wextra -Os -g
LFLAGS =
IFLAGS = -I${WORKING_DIR} -I${COMMON_DIR}

.c.o:
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@
	@echo "\n"

.cpp.o:
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@
	@echo "\n"

$(TARGET): $(OBJS)
	@echo "\n===> Build $@: "
	$(CC) $(OBJS) -o ${BINARY_DIR}/$@ $(LFLAGS) $(LIBS)

.PHONY: all clean

all: $(TARGET)
	@echo "--- Make all ---"

clean:
	@echo "\n--- Clean all ---"
	$(RM) $(TARGET)
	$(RM) $(OBJS)
	@echo "-----------------\n"
