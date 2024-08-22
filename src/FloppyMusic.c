#include "FloppyMusic.h"

#include <stdlib.h>


FM_Floppy** FM_Registry;
FM_index_t FM_RegisteredFloppies, FM_MaxFloppies;
bool FM_Initialized = false;
int FM_ManagmentMode;

int FM_Init(FM_index_t maxFloppies){
    if(FM_Initialized) return -127;

    FM_MaxFloppies = maxFloppies;
    FM_Registry = (FM_Floppy**) malloc(maxFloppies * sizeof(FM_Floppy*));
    if(FM_Registry == NULL) return -2;
    for(FM_index_t i=0; i<maxFloppies; i++){
        FM_Registry[i] = NULL;
    }
    FM_RegisteredFloppies = 0;

    FM_Initialized = true;
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
