#ifndef FM_AUTO_H
#define FM_AUTO_H

#include "FM_Floppy.h"

int FM_Auto_StartPlayingSound(int periodNanoseconds);

int FM_Auto_StopPlayingSound(int periodNanoseconds);

FM_Floppy* FM_Auto_GetFirstNonBusy();

FM_Floppy* FM_Auto_GetCurrentlyPlayingSound(int periodNanoseconds);

#endif