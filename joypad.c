// joyreg 1 is direction inputs, reg2 is button inputs
#include "memory.h"

unsigned char joyRegs[2] = {0xff, 0xff};
unsigned int bit = 0;

void writeToJoypad(unsigned char value){
	//key press right
	if(value == 'r'){
		joyRegs[0] &= 0x0e;
		writeByte(0xff0f, 0x10);
	}
	//key up right
	else if(value == 'e'){
		joyRegs[0] |= 0x01;
	}
	// key down of left
	else if(value == 'l'){
		joyRegs[0] &= 0x0d;
		writeByte(0xff0f, 0x10);
	}
	// key up of left
	else if(value == 'k'){
		joyRegs[0] |= 0x02;
	}
	// key down of up
	else if(value == 'u'){
		joyRegs[0] &= 0x0b;
		writeByte(0xff0f, 0x10);
	}
	// key up of up
	else if(value == 'y'){
		joyRegs[0] |= 0x04;
	}
	// key down of down
	else if(value == 'd'){
		joyRegs[0] &= 0x07;
		writeByte(0xff0f, 0x10);
	}
	// key up of down
	else if(value == 's'){
		joyRegs[0] |= 0x08;
	}
	// key down of start
	else if(value == 'n'){
		joyRegs[1] &= 0x07;
		writeByte(0xff0f, 0x10);
	}
	// key up of start
	else if(value == 'b'){
		joyRegs[1] |= 0x08;
	}

	// key press of a
	else if(value == 'a'){
		joyRegs[1] &= 0x0e;
		writeByte(0xff0f, 0x10);
	}

	// key up a 
	else if(value == '@'){
		joyRegs[1] |= 0x01;
	}
}

unsigned char getJoyState(){
	return joyRegs[bit];
}

// set bit to see what buttons the program is interested in
void setBit(unsigned char value){
	if(!(value & 0x10)){
		bit = 0;
	}else if(!(value & 0x20)){
		bit = 1;
	}
}
