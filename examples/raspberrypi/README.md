# Overview
This example implements the library for 2 floppies on a Raspberry Pi. It listens for MIDI note on/off messages coming from a MIDI device, and then plays them using the floppies as configured. It offers a selection between either automatic or per-channel floppy managment.

# Building
This example depends on [PortMidi](https://github.com/PortMidi/PortMidi) and [pigpio](https://github.com/joan2937/pigpio). From what I can tell, pigpio (and headers) is installed on every RPi by default, and you only need to install the PortMidi headers:

`sudo apt install libportmidi-dev`

Then, clone the repo and build the library, and then this example:

```
git clone https://github.com/Tolv2/FloppyMusic
cd FloppyMusic
make
cd examples/raspberrypi
```
You should now open `read_from_midi_device.c` with your preferred text editor and change the defines to match the pinout of your floppy. You need to connect the floppies STEP, DRIVE SELECT and DIRECTION SELECT pins, and map them out in the defines accordingly. Once you're done, save and compile with `make` You need to run the resulting executable with sudo, i.e. `sudo ./read_from_midi_device`, because Pi GPIO managment requires it (at least without the pigpiod daemon running).


# Usage
After running, you will be (after some text) asked to select a MIDI device to open. If you wish to play MIDI files, select the MIDI Through port, if you have a MIDI instrument, connect it before running the program, and select it now. 

You will then be prompted to choose between MIDI or Automatic managment. 
MIDI managment means that, when a note message is recieved, it will always play on the floppy with the same index as the MIDI channel it came from. This limits you to 16 floppies per instance of the program, but probably sounds much better musically unless you have identical (or similar enough) floppies.
Automatic managment simply plays the note on the first floppy that isn't currently playing another note. This works best with a live instrument, as they usually only send over one channel.

After you enter these two options, the main loop begins. Any MIDI messages are handled with the appropriate functions from the library. If you selected the MIDI Through port earlier, you will now want to open up another terminal, and play a MIDI file into it. A great tool for this is `aplaymidi`, for example `aplaymidi -p 14:0 my_midi.mid` (14:0 is the MIDI port, list available ports with `aplaymidi -l`). If you selected a different MIDI instrument, play it, and you should hear the notes coming from the floppies. 

Control-C to exit, but be aware that sometimes the pigpio library doesn't shut down clearly and will give you errors if you try to start it again. The simplest fix is a reboot.