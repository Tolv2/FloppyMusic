#include "FM_Floppy.h"
#include "FloppyMusic.h"

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

void FM_FloppyClear(FM_Floppy* floppy){
    const FM_FloppyInfo* info = floppy->floppyInfo;

    floppy->direction = !floppy->floppyInfo->logicalTrue;
    floppy->track = 0;
    floppy->sleepPeriod->tv_nsec = 0;

    info->writeFunc(info->directionSelectGPIOPin, !floppy->floppyInfo->logicalTrue);
    info->writeFunc(info->driveSelectGPIOPin, !floppy->floppyInfo->logicalTrue);
    info->writeFunc(info->directionSelectGPIOPin, !floppy->floppyInfo->logicalTrue);
}

void* FM_FloppyLoop(void* _floppy){
    if(!FM_Initialized) return NULL;

    FM_Floppy* floppy = (FM_Floppy*) _floppy; //helpers
    const FM_FloppyInfo* info = floppy->floppyInfo;
    bool logicalFalse = !(info->logicalTrue);
    struct timespec duration = {0};


    FM_FloppyClear(floppy);
    
    duration.tv_nsec = FM_STEP_DURATION;
    for(int i=0; i<info->floppyTracks; i++){
        /*
        pulsing for 3ms is specified to be exactly one step 
        by doing it e.g. 80 times we are guaranteed to be at one end of the drive
        we do not want to try to play notes when at the end of the drive
        */ 
        info->writeFunc(info->stepGPIOPin, info->logicalTrue);
        info->writeFunc(info->driveSelectGPIOPin, info->logicalTrue);

        nanosleep(&duration, NULL);

        info->writeFunc(info->directionSelectGPIOPin, logicalFalse);
        info->writeFunc(info->stepGPIOPin, logicalFalse);
    }

    floppy->running = true;
    while (floppy->running){
        if(floppy->sleepPeriod->tv_nsec == 0) {
            floppy->available = true;
            usleep(1);
        } else {
            floppy->available = false;
            // this is not in a function in order to reduce overhead
            info->writeFunc(info->stepGPIOPin, info->logicalTrue);
            info->writeFunc(info->driveSelectGPIOPin, info->logicalTrue);

            nanosleep(&duration, NULL);

            info->writeFunc(info->directionSelectGPIOPin, logicalFalse);
            info->writeFunc(info->stepGPIOPin, logicalFalse);

            floppy->track += 1 + (duration.tv_nsec / (FM_STEP_RATIO));

            if (floppy->track >= info->floppyTracks){ // if we hit the end of the track, reverse direction
                floppy->track = 0;
                floppy->direction = !floppy->direction;
                info->writeFunc(info->directionSelectGPIOPin, floppy->direction);
            }
        }
    }
    FM_FloppyClear(floppy);
}

int FM_RegisterFloppy(FM_Floppy* floppy){
    if(!FM_Initialized) return -127;

    FM_index_t _index = FM_GetFirstFreeIndex();
    if(_index == FM_INDEX_NOTFOUND) return -1;
    floppy->index = _index;

    pthread_create(floppy->pulserThread, NULL, FM_FloppyLoop, (void*)floppy);
    pthread_detach(*(floppy->pulserThread));

    FM_RegisteredFloppies++;
    return 0;
}

int FM_UnregisterFloppy(FM_Floppy* floppy){
    if(!FM_Initialized) return -127;

    if(!(floppy->running)) return -1;
    if(floppy->index >= FM_MaxFloppies) return -2;
    floppy->running = false;
    FM_Registry[floppy->index] = NULL;

    FM_RegisteredFloppies--;
    return 0;
}