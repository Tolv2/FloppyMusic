#include "FM_MIDI.h"
#include "FloppyMusic.h"


// Build map of MIDI notes ahead of time to avoid doing lots of float division and exponentiation at runtime

const int FM_MIDI_NoteMap[128]  = {122312208, 115447336, 108967800, 102851896, 97079248, 91630632, 86487792, 81633592, 77051872, 72727272, 68645400, 64792640, 61156104, 57723668, 54483900, 51425948, 48539624, 45815316, 43243896, 40816796, 38525936, 36363636, 34322704, 32396316, 30578052, 28861838, 27241946, 25712974, 24269816, 22907654, 21621948, 20408402, 19262964, 18181818, 17161352, 16198158, 15289026, 14430919, 13620973, 12856487, 12134908, 11453827, 10810974, 10204201, 9631482, 9090909, 8580675, 8099079, 7644513, 7215459, 6810487, 6428243, 6067453, 5726914, 5405487, 5102100, 4815741, 4545454, 4290338, 4049539, 3822256, 3607729, 3405243, 3214121, 3033727, 2863457, 2702743, 2551050, 2407870, 2272727, 2145169, 2024769, 1911128, 1803864, 1702621, 1607060, 1516863, 1431728, 1351371, 1275525, 1203935, 1136363, 1072584, 1012384, 955564, 901932, 851310, 803530, 758431, 715864, 675685, 637762, 601967, 568181, 536292, 506192, 477782, 450966, 425655, 401765, 379215, 357932, 337842, 318881, 300983, 284090, 268146, 253096, 238891, 225483, 212827, 200882, 189607, 178966, 168921, 159440, 150491, 142045, 134073, 126548, 119445, 112741, 106413, 100441, 94803, 89483, 84460, 79720};


int FM_MIDI_HandleMessage(FM_MIDI_message_t message){
    if(!FM_Initialized) return -127;

    if(FM_MIDI_MessageIsNoteOn(message)){
        int channel = FM_MIDI_MessageChannel(message);
        if(channel >= FM_MaxFloppies) return -1;
        else {
            //printf("[LIBRARY] Turning on note %d on floppy %d.\n", FM_MIDI_MessageData1(message), FM_MIDI_MessageChannel(message));
            FM_Floppy* floppy = FM_Registry[channel];

            floppy->sleepPeriod->tv_nsec = FM_MIDI_NoteMap[FM_MIDI_MessageData1(message)];
            floppy->available = false;

            return 0;

        }
    } else if (FM_MIDI_MessageIsNoteOff(message)){
        int channel = FM_MIDI_MessageChannel(message);
        if(channel >= FM_MaxFloppies) return -1;
        else {
            //printf("\n[LIBRARY] Turning off note %d on floppy %d.\n", FM_MIDI_MessageData1(message), FM_MIDI_MessageChannel(message));
            FM_Floppy* floppy = FM_Registry[channel];

            floppy->sleepPeriod->tv_nsec = 0;
            floppy->available = true;
            return 0;
        }
    }
}
