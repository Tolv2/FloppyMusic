#include <pigpio.h> //pigpio library - from https://github.com/joan2937/pigpio
#include <stdio.h>
#include <unistd.h>
#include <portmidi.h> //portmidi libary - from https://github.com/PortMidi/PortMidi
#include <signal.h>
#include <math.h>


#define DIRECTION_SELECT_PIN 27
#define DRIVE_SELECT_PIN 22
#define STEP_PIN 17

#define HEADLESS_MODE 1

int track, direction, myError, running, note, lastNote, lastInputDevice;
long status, data1, data2;
float frequency, duration;
PmDeviceID deviceNo;
PortMidiStream *device;
pthread_t *pPulserThread;
const PmDeviceInfo *pDeviceInfo;
PmEvent pEvent[1];
PmMessage message;
void *_garbage;

float freqFromNote(int _note){
    return 440.0 * pow(2, (float) (_note-69)/12);
}

void pulse(float _duration){
    struct timespec garbage, _duration_t = {0, (int) _duration};

    gpioWrite(STEP_PIN, 0);
    gpioWrite(DRIVE_SELECT_PIN, 0);

    nanosleep(&_duration_t, &garbage);

    gpioWrite(DRIVE_SELECT_PIN, 1);
    gpioWrite(STEP_PIN, 1);
}

void *pulserThread(void *garbage){
    for (;;) {
        if ((int) frequency == 0){
            usleep(1);
        } else {
            pulse(1000000000/frequency);
            track++;
            if (track == 80) {
                track = 0;
                direction = !direction;
                gpioWrite(DIRECTION_SELECT_PIN, direction);
            }
        }
    }
}

void onNoteOn(float _frequency){
    frequency = _frequency;
}
void onNoteOff(){
    frequency = 0;
}

void preQuit(){
    gpioTerminate();
    Pm_Terminate();
}

void onSigInt(int garbage){
    printf("Recieved CTRL-C. Quitting;\n");
    running = 0;
}

int main(){
    signal(11, onSigInt);
    gpioInitialise();
    gpioSetMode(DRIVE_SELECT_PIN, PI_OUTPUT);
    gpioSetMode(DIRECTION_SELECT_PIN, PI_OUTPUT);
    gpioSetMode(STEP_PIN, PI_OUTPUT);

    track = 0;
    direction = 0;
    frequency = 0;
    gpioWrite(DIRECTION_SELECT_PIN, direction);

    for (int i=0; i < 80; i++) pulse(3000000);

    direction = 1;
    gpioWrite(DIRECTION_SELECT_PIN, direction);
    pPulserThread = gpioStartThread(pulserThread, "\0");

    Pm_Initialize();

    for (int i=0; i<Pm_CountDevices(); i++){
        pDeviceInfo = Pm_GetDeviceInfo(i);
        if (pDeviceInfo->input) printf("Input device number %d info:\nName: \"%s\"\n\n", i, pDeviceInfo->name);
        lastInputDevice = i;
    }
    if (!(HEADLESS_MODE)){
        printf("Select device number?>");
        scanf("%d", &deviceNo);
    } else {
        deviceNo = lastInputDevice;
    }
    printf("Selected device: %d. Opening as input.\n", deviceNo);

    myError = Pm_OpenInput(&device, deviceNo, NULL, 20000, NULL, NULL);
    if (myError) {
        printf("Error no %d opening device; Quitting.\n", myError);
        preQuit();
        return -1;
    }

    Pm_SetFilter(device, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);
    printf("Set filter to note messages only.\n");

    while (Pm_Poll(device)) {
        Pm_Read(device, pEvent, 1);
    }

    running = 1;
    while (running){
        if (Pm_Poll(device)){
            myError = Pm_Read(device, pEvent, 1);
            if(myError < 0) printf("Error reading message: %d\n", myError);
            else if (myError == 0) printf("Somehown read no message again.\n");
            else {
                status = Pm_MessageStatus(pEvent[0].message);
                data1 = Pm_MessageData1(pEvent[0].message);
                data2 = Pm_MessageData2(pEvent[0].message);
                printf("Read message: %2lx %2lx %2lx\n", status, data1, data2);

                if (data2 == 0 && lastNote == (int) data1){
                    printf("Should now turn off.\n");
                    onNoteOff();
                } else if (data2 != 0) {
                    note = (int) data1;
                    printf("Should now turn on note %d\n", note);
                    onNoteOn(freqFromNote(note));
                    lastNote = note;
                }
            }
        }
    }

    preQuit();
    return 0;
}
