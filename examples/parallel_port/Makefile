SOURCES = parallel_port.c
FLOPPY_LIBRARY_DIR = ../../src/
CC = gcc

COMPILER_FLAGS = -Wall -g -I$(FLOPPY_LIBRARY_DIR)
LINKER_FLAGS = -lportmidi

PROJECT_NAME = parallel_port

all: $(SOURCES)
	$(CC) $(COMPILER_FLAGS) $(SOURCES) $(FLOPPY_LIBRARY_DIR)libfloppymusic.a $(LINKER_FLAGS) -o $(PROJECT_NAME)
clean:
	rm $(PROJECT_NAME)

.PHONY: clean all