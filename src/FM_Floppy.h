#ifndef FM_FLOPPY_H
#define FM_FLOPPY_H


#include <stdbool.h>
#include <pthread.h>
#include <time.h>


#define FM_STEP_DURATION 3000000 //nanoseconds
#define FM_STEP_RATIO 3000000
#define FM_FLOPPY_TRACKS_35_IN 80

#define FM_INDEX_ZERO (FM_index_t) 0
#define FM_INDEX_NOTFOUND (FM_index_t) -1 //hopefully nobody has 4.2 billion floppy drives

typedef unsigned int FM_index_t;

typedef struct FM_FloppyInfo {
    int directionSelectGPIOPin;
    int driveSelectGPIOPin;
    int stepGPIOPin;
    int floppyTracks;
    bool logicalTrue;
    void (*writeFunc)(int, bool);
} FM_FloppyInfo;


typedef struct FM_Floppy {
    const FM_FloppyInfo* floppyInfo;
    FM_index_t index;

    pthread_t* pulserThread;
    bool direction;
    int track;
    struct timespec* sleepPeriod;
    bool running, available;
} FM_Floppy;

void FM_FloppyClear(FM_Floppy* floppy);

void* FM_FloppyLoop(void* _floppy);

int FM_RegisterFloppy(FM_Floppy* floppy);

int FM_UnregisterFloppy(FM_Floppy* floppy);

int FM_RegisterFloppyFromInfo(FM_Floppy* floppy, const FM_FloppyInfo* info);

#endif