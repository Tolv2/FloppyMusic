#include "FloppyMusic.h"

#include <libserialport.h>
#include <portmidi.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_FLOPPIES 2

#define STEP_PIN_FLOPPY1 8 
#define DIRECTION_SELECT_PIN_FLOPPY1 7 // Adjust these according to your physical connections.
#define DRIVE_SELECT_PIN_FLOPPY1 6

#define STEP_PIN_FLOPPY2 5
#define DIRECTION_SELECT_PIN_FLOPPY2 3
#define DRIVE_SELECT_PIN_FLOPPY2 4

#define LOGICAL_TRUE false // This refers to whether the pin is pulled high or low when you want the floppy drive to consider it as 'true'. There should be no reason to change it.
#define FLOPPY_TRACKS 80 // This depends on your drive. 80 is a reasonable assumption, those are the most common.

#define MIDI_MESSAGE_BUF_SIZE 100 // I just made this up, adjust it if you have a very large file and it keeps throwing errors or something.
#define TRANSPOSE_OFFSET 0

#define SERIAL_BAUDRATE 115200


bool midiMode;
PmDeviceID deviceNo;
PortMidiStream* device;
PmEvent* event;
FM_Floppy floppy1, floppy2;
long status, note, velocity;
uint8_t message;
struct sp_port** serialPorts;
struct sp_port* arduinoPort;

// Message length: 1 byte
// Message structure:
// 0bSPPPPPPP; S = status high/low (1 - high, 0 - low); P = pin number
void myWriteFunction(int pin, bool value){
    message = (pin & 0b01111111) | (value << 7);
    sp_blocking_write(arduinoPort, &message, 1, 0);
    sp_drain(arduinoPort);
}

FM_FloppyInfo floppy2info = {
    DIRECTION_SELECT_PIN_FLOPPY1,
    DRIVE_SELECT_PIN_FLOPPY1,
    STEP_PIN_FLOPPY1,
    FLOPPY_TRACKS,
    LOGICAL_TRUE,
    myWriteFunction
},
 floppy1info = {
    DIRECTION_SELECT_PIN_FLOPPY2,
    DRIVE_SELECT_PIN_FLOPPY2,
    STEP_PIN_FLOPPY2,
    FLOPPY_TRACKS,
    LOGICAL_TRUE,
    myWriteFunction
};



int main(){
    printf("Initialising FloppyMusic...\n");
    if(FM_Init(NUMBER_OF_FLOPPIES) < 0){
        printf("FloppyMusic library initialisation failed. That's embarrasing.\n");
        return -1;
    }
    printf("FloppyMusic Initialisation succeded.\n");

    if(sp_list_ports(&serialPorts) != SP_OK) {
        printf("Serial port listing failed. Are you running with sudo/admin? Is your arduino connected and working?\n");
        FM_Deinit();
        return -2;
    }
    printf("Found serial ports:\n");
    
    for (int i = 0; serialPorts[i] != NULL; i++)
    {
        printf("Port number %d: %s\n", i, sp_get_port_name(serialPorts[i]));
    }
    printf("\nSelect the Arduino's port (usually /dev/ttyUSB0 on Unixes or COM6 on Windows)\n>");
    int portNo;
    scanf("%d", &portNo);
    if(sp_copy_port(serialPorts[portNo], &arduinoPort) != SP_OK) {
        printf("Copying port failed. Are you running with sudo/admin? Is your arduino connected and working?\n");
        FM_Deinit();
        return -2;
    }
    sp_free_port_list(serialPorts);
    if(sp_open(arduinoPort, SP_MODE_WRITE) != SP_OK || sp_set_baudrate(arduinoPort, SERIAL_BAUDRATE) != SP_OK) {
        printf("Opening and configuring port failed. Are you running with sudo/admin? Is your arduino connected and working? Is the port busy (Make sure the Arduino IDE Serial Monitor isn't open)\n");
        FM_Deinit();
        return -2;
    }
    





    if(Pm_Initialize() < 0) {
        printf("PortMidi library initialization failed. Does your system support MIDI? There should be at least a MIDI loopback device listed when running 'aplaymidi -l'\n");
        FM_Deinit();
        return -3;
    }
    printf("PortMidi Initialisation succeded.\n");

    for (int i=0; i<Pm_CountDevices(); i++){
        const PmDeviceInfo* deviceInfo = Pm_GetDeviceInfo(i);
        if (deviceInfo->input) printf("Input device number %d info:\nName: \"%s\"\n\n", i, deviceInfo->name);
    } //TODO: add headless mode, maybe arg from command line, so this section would autoselect the e.g. first device
    printf("Select device number?\n>");
    scanf("%d", &deviceNo);

    printf("Selected device %d. Opening as input.\n", deviceNo);

    if(Pm_OpenInput(&device, deviceNo, NULL, MIDI_MESSAGE_BUF_SIZE, NULL, NULL) < 0) {
        printf("Opening the device failed. Exiting\n");
        FM_Deinit();
        Pm_Terminate();
        return -4;
    }

    //Pm_SetFilter(device, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);
    //printf("Set filter to note messages only.\n");
    
    printf("Clearing backbuffer of MIDI messages.\n");
    while (Pm_Poll(device)) {
        Pm_Read(device, event, 1);
    }

    printf("Creating and registering FM_Floppy objects.\n");
    if(FM_RegisterFloppyFromInfo(&floppy1, &floppy1info) != 0 ||
    FM_RegisterFloppyFromInfo(&floppy2, &floppy2info) != 0) {
        printf("Creating and/or registering FM_Floppy failed. Exiting.\n");
        FM_Deinit();
        Pm_Terminate();
        return -5;
    }





    event = (PmEvent*) malloc(sizeof(PmEvent));

    char choice = 0;
    while (choice != 'M' && choice !='m' && choice != 'A' && choice !='a') {
        printf("\nWould you like to manage notes by [M]idi channel or [A]utomatically (first available)?\nMIDI channel mode works better when piping MIDI files through the loopback port, but Auto tends to work better with live, physical instruments.\n>");
        scanf(" %c", &choice);
    }

    bool running = true;
    switch (choice)
    {
    case 'M':
    case 'm':
        printf("Selected MIDI managment. Starting.\n");
        while (running){
            if(Pm_Poll(device)){
                if(Pm_Read(device, event, 1) <= 0){
                    printf("Failed reading an event. Probably an overflow\n");
                } else{
                    printf("Read message: 0x%.8X\n", event->message);
                    if(FM_MIDI_HandleMessage(event->message) != 0) printf("Handling above message failed. Probably simultaneous notes on one floppy.\n");
                }
            }
        }
        break;
    case 'A':
    case 'a':
        printf("Selected Auto managment. Starting.\n");
        while (running){
            if(Pm_Poll(device)){
                if(Pm_Read(device, event, 1) <= 0){
                    printf("Failed reading an event. Probably an overflow\n");
                } else{
                    status = Pm_MessageStatus(event->message);
                    note = Pm_MessageData1(event->message);
                    velocity = Pm_MessageData2(event->message);
                    printf("Read message: 0x%08X\n", event->message);

                    if (velocity == 0){
                        printf("Should now turn off.\n");
                        FM_Auto_StopPlayingSound(FM_MIDI_NoteMap[(int)note + TRANSPOSE_OFFSET]);
                    } else if (velocity != 0) {
                        printf("Should now turn on note %d\n", (int)note + TRANSPOSE_OFFSET);
                        FM_Auto_StartPlayingSound(FM_MIDI_NoteMap[(int)note + TRANSPOSE_OFFSET]);
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    free(event);

    FM_Deinit();
    Pm_Terminate();
    FM_Deinit();
}