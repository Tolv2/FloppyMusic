#include <pigpio.h> //pigpio library - from https://github.com/joan2937/pigpio
#include <stdio.h>
#include <unistd.h>
#include <midifile.h> //midilib library - from https://github.com/MarquisdeGeek/midilib
#include <midiutil.h> //also midilib

//HARDWARE STUFF
#define LOG_TRUE 0
#define LOG_FALSE 1
#define DIRECTION_SELECT_PIN 27 //configure pins as wired
#define DRIVE_SELECT_PIN 22
#define STEP_PIN 17
#define TIME_MULTIPLIER 1000000
#define BILLION 1000000000

#define BPM 45

//MIDI STUFF
#define INPUT_FILE "input.mid" // replace with file path
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define META 0xff

int NOTE_DURATION = 60000 / BPM / 8;
int BEAT_DURATION = 60000 / BPM;
int BEAT = 768;
int track;
int direction;
float frequency;
float duration;
int watchedFrequency;

pthread_t *pPulserThread;

MIDI_FILE * myFile;
MIDI_MSG midiMessage;
void pulse(int duration){
    struct timespec garbage, _duration = {0, duration};

    gpioWrite(STEP_PIN, LOG_TRUE);
    gpioWrite(DRIVE_SELECT_PIN, LOG_TRUE);

    nanosleep(&_duration, &garbage);

    gpioWrite(DRIVE_SELECT_PIN, LOG_FALSE);
    gpioWrite(STEP_PIN, LOG_FALSE);
}

void *pulserThread(void *garbage){
    for (;;) {
        if (watchedFrequency == 0){
            usleep(1);
        } else {
            pulse( BILLION / (int) watchedFrequency );
            track++;
            if (track == 80) {
                track = 0;
                direction = !direction;
                gpioWrite(DIRECTION_SELECT_PIN, direction);
            }
        }
    }
}

void onNoteOn(int _frequency){
    watchedFrequency = _frequency;
}
void onNoteOff(){
    watchedFrequency = 0;
}

int main() {

    myFile = midiFileOpen(INPUT_FILE);

    gpioInitialise();
    gpioSetMode(DRIVE_SELECT_PIN, PI_OUTPUT);
    gpioSetMode(DIRECTION_SELECT_PIN, PI_OUTPUT);
    gpioSetMode(STEP_PIN, PI_OUTPUT);

    track = 0;
    direction = 0;
    watchedFrequency = 0;
    gpioWrite(DIRECTION_SELECT_PIN, direction);

    for (int i=0; i < 80; i++) pulse(3000000);

    direction = 1;
    gpioWrite(DIRECTION_SELECT_PIN, direction);

    pPulserThread = gpioStartThread(pulserThread, "\0");

    sleep(2);



    printf("File tracks: %d\n", midiReadGetNumTracks(myFile));
    printf("File messages:\n");

    while (midiReadGetNextMessage(myFile, 0, &midiMessage)){
        switch (midiMessage.iType){
            case (NOTE_OFF):
                duration = (int) ((float) midiMessage.dt / (float) BEAT * (float) BEAT_DURATION);

                printf("Sleeping for %f ms and turning off noise.\n", duration);

                usleep(duration * 1000);
                onNoteOff();
                break;
            case (NOTE_ON):
                duration = (int) ((float) midiMessage.dt / (float) BEAT * (float) BEAT_DURATION);
                frequency = muGetFreqFromNote(midiMessage.MsgData.NoteOn.iNote);

                printf("Sleeping for %f ms and turning on noise at %f Hz.\n", duration, frequency);

                usleep(duration * 1000);
                onNoteOn((int) frequency);
                break;
            case (META):
                //printf("Meta event at delta time %d (impossible)\n", midiMessage.dt);
                break;
            }
    }

    return 0;

}
