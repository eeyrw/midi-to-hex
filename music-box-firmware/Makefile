#
# Simple Makefile for programming Atmel AVR MCUs using avra and avrdude
#
# Assemble with 'make', flash hexfile to microcontroller with 'make flash'.
#
# Configuration:
#
# MCU     -> name of microcontroller to program (see 'avrdude -p ?' for a list)
# TARGET  -> target board/programmer to use (see 'avrdude -c ?' for a list)
# DEVICE  -> linux device file refering to the interface your programmer is plugged in to
# INCPATH -> path to the AVR include files
# SRCFILE -> single assembler file that contains the source
#

MCU = atmega328p
TARGET = arduino
PORT = /dev/cu.wchusbserial1410
BAUD = 57600
INCPATH = .
SRCFILE = mg_m328p.asm

$(SRCFILE).hex: $(SRCFILE)
	avra -l $(SRCFILE).lst -I $(INCPATH) $(SRCFILE)
	cp mg_m328p.hex ../hex-file/mg_m328p.hex

flash: $(SRCFILE).hex
	avrdude -c $(TARGET) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(SRCFILE).hex:i

showfuses:
	avrdude -c $(TARGET) -p $(MCU) -P $(PORT) -b $(BAUD) -v 2>&1 |  grep "fuse reads" | tail -n2

clean:
	rm -f $(SRCFILE).hex $(SRCFILE).eep.hex $(SRCFILE).lst $(SRCFILE).obj $(SRCFILE).cof
