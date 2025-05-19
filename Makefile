# Auteur : BELACEL Madani | Author: BELACEL Madani
# Makefile for RPLMQoS_BELACEL project

# Project configuration
PROJECT = rpl-mqos-example
TARGET = sky

# Source files
SRC = src/rpl-mqos-example.c \
      src/rpl-mqos.c \
      src/rpl-mqos-metrics.c

# Include directories
CFLAGS += -I. -Iinclude -include include/sky-conf.h

# Contiki configuration
CONTIKI = $(HOME)/contiki-ng
include $(CONTIKI)/Makefile.include

# Build targets
all: $(PROJECT)

clean:
	rm -f *.sky *.ihex *.elf *.hex *.bin *.sky *.lst *.map *.d *.o

# Test targets
test: mqos-test.csc rpl-mqos-test.csc
	cooja mqos-test.csc
	cooja rpl-mqos-test.csc

.PHONY: all clean test
