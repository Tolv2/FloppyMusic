# FloppyMusic - Musical Floppy Disk Drive 'library'

This project aims to create a small library that makes it easier to control a given number of FDDs (which I will refer to as floppies) in such a way that they produce a certain note when told to do so. This is done by pulsing the floppies' STEP and DRIVE ENABLE pins logically true (usually LOW) at exactly the right frequency. This library is able to manage multiple floppies at once by spawning a new thread for each one. 

# Dependencies

The library itself is written in pure C and has no dependencies, other than a POSIX thread compliant system and C compiler (-pthreads). However, it integrates very well with MIDI libraries such as [PortMidi](https://github.com/PortMidi/portmidi) or [Midilib](https://github.com/MarquisdeGeek/midilib), because the library does have the capabilities to handle MIDI messages, under certain conditions. See the examples for more details.

The dependencies for the examples are listed in their own READMEs.

# Usage

Generally, the workflow should look like this:

1. Initialize the library with a call to FM_Init
2. Create and fill correct FM_FloppyInfo structs that contain all the nescessary information in order to drive the floppy correctly. This includes mapping all the pins, but also providing a write function that will set a certain pin to true or false with the signature void(int, bool) (see examples)
3. Create (but do not nescesarily fill) FM_Floppy structs that will serve as a handle.
4. Register the floppies to the library by either calling FM_RegisterFloppyFromInfo(), or filling the FM_Floppy struct manually and then calling FM_RegisterFloppy()
5. Choose between 'Automatic' or 'MIDI' managment mode [^1].
    - Under Automatic mode, in the main loop, pass on the *period in nanoseconds(!)* of the sound you wish to play on to the FM_Auto_StartPlayingSound() when you wish to start playing the node and the same value to FM_Auto_StopPlayingSound() when you wish to stop playing it. Note that this library includes a precalculated array that serves as a map of MIDI notes to these periods, where the index of the MIDI note corresponds to the index in the array. It's called FM_MIDI_NoteMap, declared in FM_MIDI.h.
    - Under MIDI mode, simply pass the 32 bit MIDI message word on to FM_MIDI_HandleMessage(), This will handle NoteOn and NoteOff messages for you. Notably, MIDI libraries often let you access this 32 bit word, and you can simply pass it on (for example PortMidi PmMessage messages are directly compatible).
6. After the user chooses to exit, call FM_Deinit() to free allocated resources. 



[^1] This choice isn't enforced, you can mix and match calls to the functions from the different submodules, but this is not reccommended.