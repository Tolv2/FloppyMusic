#ifndef FM_MIDI_H
#define FM_MIDI_H

#include <stdint.h>

typedef uint32_t FM_MIDI_message_t;

#define FM_MIDI_MessageStatus(x) ((x) & 0xFF) ///< A macro that reads the Status field of a MIDI message. This contains data about whether the message is a NoteOn or NoteOff, as well as the channel it's on.
#define FM_MIDI_MessageData1(x) ((x >> 8) & 0xFF) ///< A macro that reads the Data 1 field of a MIDI message. This is usually simply the encoded note. 
#define FM_MIDI_MessageData2(x) ((x >> 16) & 0xFF) ///< A macro that reads the Data 2 field of a MIDI message. This is usually the velocity field.
#define FM_MIDI_MessageChannel(x) ((x) & 0xF) ///< A macro that reads the channel field of a MIDI status message.
#define FM_MIDI_MessageIsNoteOn(x) (FM_MIDI_MessageStatus(x) & 0xF0) == 0x90 ///< A macro that determines whether a MIDI message is a NoteOn message.
#define FM_MIDI_MessageIsNoteOff(x) (FM_MIDI_MessageStatus(x) & 0xF0) == 0x80 ///< A macro that determines whether a MIDI message is a NoteOff message.


/**
 * Used internally, but safe to be used externally. A constant read-only hard-coded array that contains the periods of notes in nanoseconds at the corresponding index. For example, to get the period of MIDI note 50, use FM_MIDI_NoteMap[50]
 */
extern const int FM_MIDI_NoteMap[128];


// TODO: add methods for assignig floppies to channels other than the default registry order

/**
 * Handle a MIDI message in MIDI managment mode. MIDI managment means that, when a note message is recieved, it will always play on the floppy with the same index as the MIDI channel it came from. This limits you to 16 floppies per instance of the program, but probably sounds much better musically unless you have identical (or similar enough) floppies.
 * 
 * @param message the message to handle. Intepreted as a 32 bit word. Most likely safe to cast raw messages from other libraries (e. g. PortMidi).
 */
int FM_MIDI_HandleMessage(FM_MIDI_message_t message);

#endif