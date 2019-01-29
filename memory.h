#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loadRom();
unsigned char readByte(unsigned short address);
unsigned short readShort(unsigned short address);
void writeByte(unsigned short address, unsigned char byte);
void writeShort(unsigned short address, unsigned short val);
void copyBiosToRom();
void DMAtransfer(unsigned char source);