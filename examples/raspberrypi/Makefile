SOURCES = read_from_midi_device.c
FLOPPY_LIBRARY_DIR = ../../src/
CC = gcc

COMPILER_FLAGS = -Wall -g -I$(FLOPPY_LIBRARY_DIR)
LINKER_FLAGS = -lportmidi -lpigpio

PROJECT_NAME = read_from_midi_device

all: $(SOURCES)
	$(CC) $(COMPILER_FLAGS) $(SOURCES) $(FLOPPY_LIBRARY_DIR)libfloppymusic.a $(LINKER_FLAGS) -o $(PROJECT_NAME)
clean:
	rm $(PROJECT_NAME)

.PHONY: clean all