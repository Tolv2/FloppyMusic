# Overview
This example implements the library for 2 floppies on an Arduino-compatible device. It works in a very similar way to the Raspberry Pi example, but delegates the actual pin switching to the Arduino. The pc server/master application listens for MIDI note on/off messages coming from a MIDI device, and then tells the Arduino running the client/slave application which pins to turn on and off at exactly the right time. It offers a selection between either automatic or per-channel floppy managment. It also doesn't work very well because the effective bandwith of this way of communication is very low, and the Arduino (at least the Nano) is underpowered for this sort of thing. Still, it's a good enough example.

# Building
The pc client appliaction relies on [libserialport](https://sigrok.org/wiki/Libserialport) to communicate with the Arduino over the serial interface. Install it (including headers) from your distribution's package manager, or build it yourself. It's very portable, so there should be no problem with Windows/Linux/etc.

Then, clone the repo and build the library, and then this example:

```
git clone https://github.com/Tolv2/FloppyMusic
cd FloppyMusic
make
cd examples/arduino
```
You should now open both `pc_server.c` and `arduino_client.ino` with your preferred text editor and change the defines to match the pinout of your floppies. You need to connect the floppies' STEP, DRIVE SELECT and DIRECTION SELECT pins, and map them out in the defines accordingly. Once you're done, save and compile the PC application with `make`. I leave you to compile and upload the Arduino code yourself, the simplest is probably the Arduino IDE. After the upload, the arduino will await commands forever and it's ready to use.

# Usage
After running, you will be (after some text) asked to select a serial port that the Arduino is connected to. Make sure that you're choosing the correct one (it will be the same port that you wrote the image in in the Arduino IDE).

You will then be asked to select a MIDI device to open. If you wish to play MIDI files, select the MIDI Through port, if you have a MIDI instrument, connect it before running the program, and select it now. 

You will then be prompted to asked between MIDI or Automatic managment. 
- MIDI managment means that, when a note message is recieved, it will always play on the floppy with the same index as the MIDI channel it came from. This limits you to 16 floppies per instance of the program, but probably sounds much better musically unless you have identical (or similar enough) floppies.
- Automatic managment simply plays the note on the first floppy that isn't currently playing another note. This works best with a live instrument, as they usually only send over one channel.

After you enter these two options, the main loop begins. Any MIDI messages are handled with the appropriate functions from the library. If you selected the MIDI Through port earlier, you will now want to open up another terminal, and play a MIDI file into it. A great tool for this is `aplaymidi`, for example `aplaymidi -p 14:0 my_midi.mid` (14:0 is the MIDI port, list available ports with `aplaymidi -l`). If you selected a different MIDI instrument, you can now play it, and you should hear the notes coming from the floppies. 

Control-C to exit, but be aware that sometimes the pigpio library doesn't shut down clearly and will give you errors if you try to start it again. The simplest fix is a reboot.