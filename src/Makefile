SOURCES = FM_Floppy.c FloppyMusic.c FM_Auto.c FM_MIDI.c
CC = gcc
AR = ar

ARCHIVER_FLAGS = rcs
COMPILER_FLAGS = -w -pthread -c -fpic -Wall -g
LINKER_FLAGS =

PROJECT_NAME = floppymusic

STATIC_NAME = lib${PROJECT_NAME}.a
DYNAMIC_NAME = lib${PROJECT_NAME}.so
OBJ_NAMES = $(SOURCES:.c=.o)


all: ${STATIC_NAME} ${DYNAMIC_NAME}
.PHONY: all clean

%.o: %.c
	$(CC) $(COMPILER_FLAGS) -c $< -o $@

${STATIC_NAME}: ${OBJ_NAMES}
	$(AR) $(ARCHIVER_FLAGS) $@ $^

${DYNAMIC_NAME}: ${OBJ_NAMES}
	$(CC) -shared $(LINKER_FLAGS) -o $@ $^

clean:
	rm -f ${OBJ_NAMES} ${DYNAMIC_NAME} ${STATIC_NAME}
