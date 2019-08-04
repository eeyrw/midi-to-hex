cd ./music-box-firmware
make
cd ..
./midi-to-hex --device  m328p -m $1
avrdude -c arduino -p atmega328p -P /dev/tty.usbmodem0001 -b 57600 -U flash:w:target.hex:i -V