#ifndef FM_AUTO_H
#define FM_AUTO_H

#include "FM_Floppy.h"

/**
 * Start playing a sound using Automatic managment mode. Automatic managment simply plays the note on the first floppy that isn't currently playing another note. This works best with a live instrument, as they usually only send over one channel.
 * 
 * @param periodNanoseconds the period of the sound to play. Note that you must pass the exact same value back to FM_Auto_StopPlayingSound to stop playing the sound started by this call. Also note that a map of MIDI notes to periods is available in the library, and can be safely used here.
 * \sa FM_MIDI_NoteMap 
 */
int FM_Auto_StartPlayingSound(int periodNanoseconds);

/**
 * Stop playing a sound previously started by a call to FM_AutoStartPlayingSound.
 * 
 * @param periodNanoseconds the period of the sound to stop playing. Note that you must pass the exact same value as you did to FM_Auto_StartPlayingSound.
 */
int FM_Auto_StopPlayingSound(int periodNanoseconds);

/**
 * Used internally. Finds the first FM_Floppy that isn't currently busy playing a sound.
 */
FM_Floppy* FM_Auto_GetFirstNonBusy();

/**
 * Used internally. Finds the first floppy that is currently playing the sound passed on.
 * 
 * @param periodNanoseconds the period of the sound to look for.
 */
FM_Floppy* FM_Auto_GetCurrentlyPlayingSound(int periodNanoseconds);

#endif