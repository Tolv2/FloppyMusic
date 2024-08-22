#ifndef FM_MAIN_H
#define FM_MAIN_H

#include "FM_Floppy.h"
#include "FM_MIDI.h"
#include "FM_Auto.h"

extern FM_Floppy** FM_Registry;
extern FM_index_t FM_RegisteredFloppies, FM_MaxFloppies;
extern bool FM_Initialized;
int FM_Init(FM_index_t FM_MaxFloppies);

int FM_Deinit();

FM_index_t FM_GetFirstFreeIndex();


#endif