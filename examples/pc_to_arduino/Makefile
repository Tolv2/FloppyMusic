SOURCES = pc_server.c
FLOPPY_LIBRARY_DIR = ../../src/
CC = gcc

COMPILER_FLAGS = -Wall -g -I$(FLOPPY_LIBRARY_DIR)
LINKER_FLAGS = -lserialport -lportmidi

PROJECT_NAME = pc_server

all: $(SOURCES)
	$(CC) $(COMPILER_FLAGS) $(SOURCES) $(FLOPPY_LIBRARY_DIR)libfloppymusic.a $(LINKER_FLAGS) -o $(PROJECT_NAME)
clean:
	rm $(PROJECT_NAME)

.PHONY: clean all