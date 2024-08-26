#ifndef FM_FLOPPY_H
#define FM_FLOPPY_H


#include <stdbool.h>
#include <pthread.h>
#include <time.h>


#define FM_STEP_DURATION 3000000 //nanoseconds
#define FM_STEP_RATIO 3000000
#define FM_INDEX_ZERO (FM_index_t) 0
#define FM_INDEX_NOTFOUND (FM_index_t) -1 //hopefully nobody has 4.2 billion floppy drives

typedef unsigned int FM_index_t;
/**
 * A struct representing a physical floppy device and how to interface with it.
 * 
 * This struct contains data about the pinout of this specific floppy, as well as how to write to its pins. You're meant to create and fill one of these for each floppy that you have.
 */
typedef struct FM_FloppyInfo {
    int directionSelectGPIOPin; ///< Number of local pin that the floppies DIRECTION SELECT pin (pin 18) is connected to. 
    int driveSelectGPIOPin; ///< Number of local pin that the floppies active DRIVE SELECT (B/A) pin (pin 12/14, depending on the cable twist) is connected to. 
    int stepGPIOPin; ///<  Number of local pin that the floppies STEP pin (pin 20) is connected to. 
    int floppyTracks; ///< Number of tracks on the floppy drive. I. e. how many tracks does it take to hit the end of the drive. For most drives, this is 80. If unsure, it is safe to pass a lower value, but passing a larger value may result in an undesirable screeching sound after hitting one end of the drive.
    bool logicalTrue; ///< Logic level that the floppy considers TRUE. For most setups this is false (as floppies consider a LOW pull to mean TRUE). Flip this if your floppies remain on after being done playing a note. 
    void (*writeFunc)(int, bool); ///< User-provided write function. This function must set the pin to the value, and it must do it quickly (it's called 4 times every period of the sound). @param [in] pin the pin to be set @param [in] value the value to be set.
} FM_FloppyInfo;

/**
 * A struct representing a floppy device managed by the library. You're not meant to fill this manually, it is done by FM_RegisterFloppyFromInfo.
 */
typedef struct FM_Floppy {
    const FM_FloppyInfo* floppyInfo; ///< FM_FloppyInfo struct representing the physical floppy device.
    FM_index_t index; ///< Used internally. Index in FM_Registry that points to this instance of FM_Floppy.

    pthread_t* pulserThread; ///< Used internally. The POSIX thread belonging to this FM_Floppy. The thread of a floppy is an infinite loop that will play a note if the sleepPeriod member of the FM_Floppy struct indicates a period (if the tv_nsec member isn't zero). It exits when the running member of FM_Floppy is set to false.
    bool direction; ///< Used internally. This keeps track of the direction of the stepper motor, so that hitting the end of the drive can be avoided.
    int track; ///< Used internally. This keeps track of which track on the drive the head is currently over. It is not meant to be reliable, just roughly accurate.
    struct timespec* sleepPeriod; ///< Used internally. The thread belonging to this floppy drive will observe this struct and expect the periods of the sounds to be written here. Only the tv_nsec member is considerered.
    bool running; ///< Used internally. Used to control the thread, which will exit when this is set to false
    bool available; ///< Used internally. Used to indicate that this floppy is currently not playing a note.
} FM_Floppy;

/**
 * Used internally. Reset track, direction and step values and set all pins to logical false. 
 * 
 * @param floppy pointer to FM_Floppy which to clear.
 */
void FM_FloppyClear(FM_Floppy* floppy);

/**
 * Used internally. Main loop of the floppies thread. One instance of this function is spawned for every registered floppy. It observers the sleepPeriod member of the FM_Floppy struct and handles them accordingy.
 * 
 * @param _floppy pointer to FM_Floppy struct that the function belogns to. Declared void* because pthread needs it. Safe to cast. 
 */
void* FM_FloppyLoop(void* _floppy);

/**
 * Used internally, but can be used externally. Registers the floppy for the library to use as a playing device. Clears the pins, fills out the rest of the struct, and spawns and starts its thread.
 * 
 * @param floppy the FM_Floppy object which is to be registered.
 */
int FM_RegisterFloppy(FM_Floppy* floppy);

/**
 * Unregisters the floppy, stopping its thread and removing it from the registry, making it unavailable for playback.
 * 
 * @param floppy the FM_Floppy object which is to be unregistered.
 */
int FM_UnregisterFloppy(FM_Floppy* floppy);

/**
 * Helper function that assigns the FM_FloppyInfo object to the FM_Floppy object, and then registers it.
 * 
 * @param floppy the FM_Floppy object which is to be filled and registered.
 * @param info the already filled out FM_FloppyInfo struct with the data of the physical floppy. 
 */
int FM_RegisterFloppyFromInfo(FM_Floppy* floppy, const FM_FloppyInfo* info);

#endif