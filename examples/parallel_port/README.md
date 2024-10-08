# Overview
Unbeknownst to many people, the parallel port (sometimes also called the printer port) on older computers can be used as GPIO. This example implements the library for 2 floppies on the parallel port. It listens for MIDI note on/off messages coming from a MIDI device, and then plays them using the floppies as configured. It offers a selection between either automatic or per-channel floppy managment.

# Building
This example depends on [PortMidi](https://github.com/PortMidi/PortMidi). It also only works on Linux (and probably also on other *nix systems). In theory, this could also be achieved on Windows, but with a different write function.

Then, clone the repo and build the library, and then this example:

```
git clone https://github.com/Tolv2/FloppyMusic
cd FloppyMusic
make
cd examples/parallel_port
```
You should now open `parallel_port.c` with your preferred text editor and change the defines to match the pinout of your floppies. You need to connect the floppies' STEP, DRIVE SELECT and DIRECTION SELECT pins, and map them out in the defines accordingly. Note that the pin numbers used in the example refer to the D (or output) pins of the parallel port (so, pin 0 maps to D0, pin 1 to D1 etc., the control pins are unused, and if you wish to use them, you will need to modify the example). Once you're done, save and compile with `make`. You need to run the resulting executable with sudo, i.e. `sudo ./parallel_port`, because IO managment requires it.

# Usage
After running, you will be (after some text) asked to select a MIDI device to open. If you wish to play MIDI files, select the MIDI Through port, if you have a MIDI instrument, connect it before running the program, and select it now. 

You will then be prompted to choose between MIDI or Automatic managment. 
- MIDI managment means that, when a note message is recieved, it will always play on the floppy with the same index as the MIDI channel it came from. This limits you to 16 floppies per instance of the program, but probably sounds much better musically unless you have identical (or similar enough) floppies.
- Automatic managment simply plays the note on the first floppy that isn't currently playing another note. This works best with a live instrument, as they usually only send over one channel.

After you enter these two options, the main loop begins. Any MIDI messages are handled with the appropriate functions from the library. If you selected the MIDI Through port earlier, you will now want to open up another terminal, and play a MIDI file into it. A great tool for this is `aplaymidi`, for example `aplaymidi -p 14:0 my_midi.mid` (14:0 is the MIDI port, list available ports with `aplaymidi -l`). If you selected a different MIDI instrument, you can now play it, and you should hear the notes coming from the floppies. 

Control-C to exit.
