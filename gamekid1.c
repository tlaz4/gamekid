#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "memory.h"
#include "gpu.h"
#include "gui.h"
#include "interrupts.h"
#include "joypad.h"
#include "timer.h"

void powerUpSequence(){
	initPc();

	loadRom();
	copyBiosToRom();

	while(checkPc(0x100)){
		unsigned char thisLen = cpu();
		gpu(thisLen);
	}

	loadRom();
}


// to compile run: gcc -Wall -std=c99 -o gamekid gamekid1.c cpu.c memory.c
// gcc -Wall -std=c99 -o gamekid gamekid1.c cpu.c memory.c gpu.c gui.c interrupts.c -lmingw32 -lSDL2main -lSDL2

// test roms passed 04, 05, 06
int main(int argc, char* args[]){
	int quit;

	if(argc > 1){
		unsigned short breakPoint = (unsigned short)strtol(args[1], NULL, 0);
		setBreakpoint(breakPoint);
	}

	createWindow();
	powerUpSequence();
	//initRegisters();
	
	printf("boot finished\n");

	quit = 0;

	writeByte(0xff00, 0xff);
	// cpu should return the last instruction cycle time to the gpu
	while(!quit){
		quit = getInput();
		unsigned char thisLen = cpu();
		gpu(thisLen);
		//updateTimer(thisLen);
		interrupt();
	}
		
	return 1;
	
}