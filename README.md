
# Building midi-to-hex

The midi-to-hex is written with C++ and uses cmake as build tool. The bootloader software inside requires libusb-**1.0**.  Following examples explain the ~~detail~~ building steps in different O.S.

## Windows

 1. Install mingw**64**, libusb-**1.0** and cmake (Do it by yourself...) . 
 2. Open a shell ( I use powershell ) and locate it at the root directory of project and input `cmake -G "MinGW Makefiles" .`  (DO NOT MISS THE **DOT** IN THE END) and press Enter.
 3. If everything goes well... You can start to build and input  `mingw32-make.exe -j5` and press Enter. You may see the ouput from cmake such as `[100%] Built target midi-to-hex` (which means that you have built the midi-to-hex successfully) or a lot of error info... 
## Mac
1. I installed Xcode and brew to get Apple Clang and libusb-1.0 and cmake.
2. Open a terminal emulator and locate it at the root directory of project and input `cmake .`  (DO NOT MISS THE **DOT** IN THE END) and press Enter.
3. If everything goes well... You can start to build and input  `make -j5` and press Enter. You may see the output from cmake such as `[100%] Built target midi-to-hex` (which means that you have built the midi-to-hex successfully) or a lot of error info... 
 ## Linux
 1. Install libusb-1.0  and cmake by apt-get ( maybe other tool )...
 2. Open a terminal emulator and locate it at the root directory of project and input `cmake .`  ( A Linux user definitely never misses the  **DOT**. ) .
 3. If everything goes well... You can start to build and input  `make -j5` and press Enter. You may see the ouput from cmake such as `[100%] Built target midi-to-hex` (which means that you have built the midi-to-hex successfully) or a lot of error info... 

# Usage of midi-to-hex
The midi-to-hex is a command line tool. Here is an example of usage:

    PS F:\Users\yuan\Desktop\midi-to-hex> .\midi-to-hex.exe -m .\midi-sample\twinkle.mid -d --device t167
    Song duration: 8 s
    Note pitch v.s. occur times table:
    +--------------------------------------+
    |  Note pitch| Occur times| Is in range|
    +--------------------------------------+
    |         3:C|           2|         YES|
    |         3:F|           1|         YES|
    |         3:G|           1|         YES|
    |         3:A|           1|         YES|
    |         3:B|           1|         YES|
    |         4:C|           5|         YES|
    |         4:D|           1|         YES|
    |         4:E|           2|         YES|
    |         4:F|           1|         YES|
    |         5:C|           3|         YES|
    |         5:D|           2|         YES|
    |         5:E|           2|         YES|
    |         5:F|           2|         YES|
    |         5:G|           3|         YES|
    |         5:A|           2|         YES|
    |        Fine|           1|          NO|
    +--------------------------------------+
    Highest pitch: 5:A
    Lowest pitch: 3:C
    Transpose suggestion: 0 half note
    Mem size: 46b
    Generate hex file for ATTINY 167.
    Download target.hex to mcu through micronucleus bootloader.
    Please ensure the mcu device is under bootloader mode (usally by re-pluging usb or reseting mcu).
    > Please plug in the device ...
    > Press CTRL+C to terminate the program.
You can plug in your music-box to PC at the moment and something will happen like this:

    > Device is found!
    waiting: 20% complete
    > Device has firmware version 2.2
    > Device signature: 0x1e9487
    > Available space for user applications: 14842 bytes
    > Suggested sleep time between sending pages: 5ms
    > Whole page count: 116  page size: 128
    > Erase function sleep duration: 580ms
    parsing: 40% complete
    parsing: 60% complete
    > Erasing the memory ...
    erasing: 65% complete
    erasing: 70% complete
    erasing: 75% complete
    erasing: 80% complete
    > Starting to upload ...
    writing: 85% complete
    writing: 90% complete
    writing: 95% complete
    writing: 100% complete
    >> Micronucleus done. Thank you!
Then the firmware of music-box has been updated with your midi file.
There are some parameters that should be explain here:

-m or --midi : Specify midi file. This parameter is mandatory.

-t or --transpose : Specify a wanted transpose in half pitch. If you do not specify, the suggestion transpose calculated by midi-to-hex will be applied.

-d or --download: With this parameter means download hex file to music-box is required. 

--device : Specify the mcu device used in your music-box. It only support t85 and t167 by now. The t85 will be default if without this parameter. 

# Great Thanks to:
ELM-Chan : http://elm-chan.org/works/mxb/report.html

> Who put forward the original idea about firmware and hardware of music box.

Stuart Cording : https://github.com/codinghead/Intel-HEX-Class

> Who powered midi-to-hex with ability to access intel hex file.

Jenna Fox & Tim : https://github.com/micronucleus/micronucleus
> Who created a robust bootloader for ATtiny MCU with V-USB.

Craig Stuart Sapp : https://github.com/craigsapp/midifile
> Who powered midi-to-hex with ability to access midi file and he is so warmhearted and answer my question by a vivid example : )
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTgyMzYyNTE3M119
-->