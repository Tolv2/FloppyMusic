#include "FM_Auto.h"
#include "FloppyMusic.h"


FM_Floppy* FM_Auto_GetFirstNonBusy(){

    if(!FM_Initialized) return NULL;
    
    for(FM_index_t i=0; i<FM_MaxFloppies; i++){
        if(FM_Registry[i] != NULL && FM_Registry[i]->available && FM_Registry[i]->running) return FM_Registry[i];
    }
    return NULL;
}


FM_Floppy* FM_Auto_GetCurrentlyPlayingSound(int periodNanoseconds){
    if(!FM_Initialized) return NULL;

    for(FM_index_t i=0; i<FM_MaxFloppies; i++){
        if(FM_Registry[i] != NULL && FM_Registry[i]->sleepPeriod->tv_nsec == periodNanoseconds) return FM_Registry[i];
    }
    return NULL;
}




int FM_Auto_StartPlayingSound(int periodNanoseconds){
    if(!FM_Initialized) return -127;

    if(FM_Auto_GetCurrentlyPlayingSound(periodNanoseconds) != NULL) return -1;
    
    FM_Floppy* floppy = FM_Auto_GetFirstNonBusy();
    if(floppy == NULL) return -2;

    floppy->sleepPeriod->tv_nsec = periodNanoseconds;
    floppy->available = false;
    
    return 0;
}

int FM_Auto_StopPlayingSound(int periodNanoseconds){

    if(!FM_Initialized) return -127;

    FM_Floppy* floppy = FM_Auto_GetCurrentlyPlayingSound(periodNanoseconds);
    if(floppy == NULL) return -1;
    floppy->sleepPeriod->tv_nsec = 0;
    floppy->available = true;
    return 0;
}


