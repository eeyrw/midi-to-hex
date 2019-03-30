cd ./music-box-firmware
make
cd ..
./midi-to-hex --device  m328p -m "./midi-sample/Beethoven_-_Violin_Sonata_No.5_Op.24_Spring_movement_I._Allegro.mid"
avrdude -c arduino -p atmega328p -P /dev/cu.wchusbserial1410 -b 57600 -U flash:w:target.hex:i