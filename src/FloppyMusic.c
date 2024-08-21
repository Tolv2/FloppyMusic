#include "FloppyMusic.h"

#include <math.h>
#include <stdlib.h>


FM_Floppy** FM_Registry;
FM_index_t FM_RegisteredFloppies, FM_MaxFloppies;
bool FM_Initialized = false;
int* FM_MIDIMap;

int FM_Init(FM_index_t maxFloppies){
    FM_MaxFloppies = maxFloppies;
    FM_Registry = (FM_Floppy**) malloc(maxFloppies * sizeof(FM_Floppy));
    FM_MIDIMap = (int*) malloc(maxFloppies * sizeof(int));
    if(FM_Registry == NULL || FM_MIDIMap == NULL) return -1;
    for(FM_index_t i=0; i<maxFloppies; i++){
        FM_Registry[i] = NULL;
    }
    FM_RegisteredFloppies = 0;


    //Build map of MIDI notes to sound periods to avoid doing it on the fly
    float period;
    for(int i=0; i<128; i++){
        period = 1000000000/(440.0 * pow(2, (float) (i-69)/12));
        FM_MIDIMap[i] = (int) period;
    }

    return 0;
}

int FM_Deinit(){
    if(!FM_Initialized) return -127;
    for(FM_index_t i=0; i<FM_MaxFloppies; i++){
        if (FM_Registry[i] != NULL) FM_FloppyClear(FM_Registry[i]);
        free(FM_Registry[i]);
    }
    free(FM_Registry);
    FM_Initialized = false;

    return 0;
}

FM_index_t FM_GetFirstFreeIndex(){
    if(!FM_Initialized) return -127;


    for(FM_index_t i = FM_INDEX_ZERO; i<FM_MaxFloppies; i++){
        if(FM_Registry[i] == NULL) return i;
    }
    return FM_INDEX_NOTFOUND;
}

FM_Floppy* FM_GetFirstNonBusy(){
    if(!FM_Initialized) return NULL;
    
    for(FM_index_t i=0; i<FM_MaxFloppies; i++){
        if(FM_Registry[i] != NULL && FM_Registry[i]->available && FM_Registry[i]->running) return FM_Registry[i];
    }
    return NULL;
}

FM_Floppy* FM_GetCurrentlyPlayingSound(int periodNanoseconds){
    if(!FM_Initialized) return NULL;

    for(FM_index_t i=0; i<FM_MaxFloppies; i++){
        if(FM_Registry[i] != NULL && FM_Registry[i]->sleepPeriod->tv_nsec == periodNanoseconds) return FM_Registry[i];
    }
    return NULL;
}

int FM_StartPlayingSound(int periodNanoseconds){
    if(!FM_Initialized) return -127;

    if(FM_GetCurrentlyPlayingSound(periodNanoseconds) != NULL) return -1;
    
    FM_Floppy* floppy = FM_GetFirstNonBusy();
    if(floppy == NULL) return -2;

    floppy->sleepPeriod->tv_nsec = periodNanoseconds;
    
    return 0;
}

int FM_StopPlayingSound(int periodNanoseconds){
    if(!FM_Initialized) return -127;

    FM_Floppy* floppy = FM_GetCurrentlyPlayingSound(periodNanoseconds);
    if(floppy == NULL) return -1;

    floppy->sleepPeriod->tv_nsec = 0;
    return 0;
}

int FM_StartPlayingMIDINote(int MIDINote){
    if(!FM_Initialized) return -127;
    return FM_StartPlayingSound(FM_MIDIMap[MIDINote]);
}

int FM_StopPlayingMIDINote(int MIDINote){
    if(!FM_Initialized) return -127;
    return FM_StopPlayingSound(FM_MIDIMap[MIDINote]);
}
