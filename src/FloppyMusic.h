#ifndef FM_MAIN_H
#define FM_MAIN_H

#include "FM_Floppy.h"

extern FM_Floppy** FM_Registry;
extern FM_index_t FM_RegisteredFloppies, FM_MaxFloppies;
extern bool FM_Initialized;
extern int* FM_MIDIMap;


int FM_Init(FM_index_t FM_MaxFloppies);

int FM_Deinit();

FM_index_t FM_GetFirstFreeIndex();

FM_Floppy* FM_GetFirstNonBusy();

FM_Floppy* FM_GetCurrentlyPlayingSound(int periodNanoseconds);

int FM_StartPlayingSound(int periodNanoseconds);

int FM_StopPlayingSound(int periodNanoseconds);

int FM_StartPlayingMIDINote(int MIDINote);

int FM_StopPlayingMIDINote(int MIDINote);


#endif