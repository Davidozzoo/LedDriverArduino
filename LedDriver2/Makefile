flash:
	avrdude -p m8 -b 115200 -c usbasp -e -u -v -U flash:w:LedDriver2.ino_atmega8_8000000L.hex:i

flash_fuse_int8:
	avrdude -c usbasp -b 9600 -p m8 -u -v -U lfuse:w:0xe4:m -U hfuse:w:0xd9:m

flash_fuse_ext:
	avrdude -c usbasp -b 9600 -p m8 -u -v -U lfuse:w:0x7f:m -U hfuse:w:0xd9:m	
	
