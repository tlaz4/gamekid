#include "memory.h"
#include "timer.h"

#define CLOCKSPEED 4194304;

void updateTimer(unsigned char clock){
	static short timeClock = 0;
	unsigned char timerControl = readByte(0xff07);

	// check if timer is enabled
	timeClock += clock;
	if((timerControl & 0x04) == 0x04){
		unsigned int timerCounter = clockSelect();

		if(timeClock >= timerCounter){
			// timer will overflow, we must set interrupt and adjust timer with value
			if(readByte(0xff05) == 0xff){
				writeByte(0xff0f, 0x04);
				writeByte(0xff05, readByte(0xff06));
			// else increment timer
			}else{
				incrementClock(0xff05);
			}
		}
	// if clock is > 256, we need to reset the div timer, else we increment it
	}if(timeClock >= 256){
		if(readByte(0xff04) == 0xff){
			writeByte(0xff04, 0x00);
		}else{
			incrementClock(0xff04);
		}
	}
} 

void incrementClock(unsigned short address){
	unsigned char clock = readByte(address); 

	clock += 1;
	writeByte(address, clock);

}

unsigned int clockSelect(){
	unsigned char select = readByte(0xff07) & 0x03;

	if(select == 0){
		return 1024;
	}else if(select == 1){
		return 16;
	}else if(select == 2){
		return 64;
	}else if(select ==3){
		return 256;
	}else{
		return 0;
	}
}