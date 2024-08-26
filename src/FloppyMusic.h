#ifndef FM_MAIN_H
#define FM_MAIN_H

#include "FM_Floppy.h"
#include "FM_MIDI.h"
#include "FM_Auto.h"

extern FM_Floppy** FM_Registry; ///< Array that holds pointers to currently registered floppies.
extern FM_index_t FM_RegisteredFloppies; ///< The number of currenctl registered floppies. 
extern FM_index_t FM_MaxFloppies; ///< Maximum floppies allowed to be registered, set at initialization.
extern bool FM_Initialized; ///< Used to check if FM_Initialize has been called.

/**
 * Library initalization function. Allocates necesarry memory for FM_Registry.
 * 
 * @param maxFloppies the number of floppies to allocate for.
 */
int FM_Init(FM_index_t maxFloppies /**< [in]  The number of connected floppies*/);
/**
 * Library deinitialization function. Frees memory from FM_Registry.
 */
int FM_Deinit();

/**
 * Used internally. This function is a simple lookup that returns the index of first null pointer (and, therefore, and empty slot) in FM_Registry.
 */
FM_index_t FM_GetFirstFreeIndex();


#endif