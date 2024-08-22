#ifndef FM_MIDI_H
#define FM_MIDI_H

#include <stdint.h>

typedef uint32_t FM_MIDI_message_t;

#define FM_MIDI_MessageStatus(x) ((x) & 0xFF)
#define FM_MIDI_MessageData1(x) ((x >> 8) & 0xFF)
#define FM_MIDI_MessageData2(x) ((x >> 16) & 0xFF)
#define FM_MIDI_MessageChannel(x) ((x) & 0xF)
#define FM_MIDI_MessageIsNoteOn(x) (FM_MIDI_MessageStatus(x) & 0xF0) == 0x90
#define FM_MIDI_MessageIsNoteOff(x) (FM_MIDI_MessageStatus(x) & 0xF0) == 0x80

extern const int FM_MIDI_NoteMap[128];


// TODO: add methods for assignig floppies to channels other than the default registry order
int FM_MIDI_HandleMessage(FM_MIDI_message_t message);

#endif