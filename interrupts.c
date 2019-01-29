#include "cpu.h"
#include "memory.h"

//interrupt master enable
unsigned int IME = 0;

void setIME(){
	IME = 1;
}

void resetIME(){
	IME = 0;
}

// get interrupt flags and enable registers
unsigned char getIF(){
	return readByte(0xff0f);
}

unsigned char getIE(){
	return readByte(0xffff);
}

void interrupt(){
	if(IME){
		unsigned char IF = getIF();
		unsigned char IE = getIE();

		// we have a vertical blanking interrupt that we must act upon
		if((IF & 0x01) && (IE & 0x01)){
			//printf("vblank interrupt\n");
			writeByte(0xff0f, getIF() & 0xfe);
			resetIME();
			writeShortToStack(getPc());
			setPc(0x0040);
			// possibly increment pc cycle?
		}if((IF & 0x04) && (IE & 0x04)){
			writeByte(0xff0f, getIF() & 0xef);
			resetIME();
			writeShortToStack(getPc());
			setPc(0x0050);
		}if((IF & 0x10) && (IE & 0x10)){
			writeByte(0xff0f, getIF() & 0xef);
			resetIME();
			writeShortToStack(getPc());
			setPc(0x0060);
		}
	}
}