#include "memory.h"
#include "joypad.h"

// we will need more arrays to cover more of the rom data
// and to access the properly

// will need to change array so rom bank is switchable
unsigned char rom[0x8000]; // cartridge rom
unsigned char vram[0x2000]; // video ram
unsigned char sram[0x2000]; // cartridge ram 
unsigned char wram[0x2000]; // working ram
unsigned char oam[0xa0]; // object ram (sprites)
unsigned char io[0x80]; // io ram and hardware
unsigned char hram[0x7f]; // 
unsigned char intio[0x01];

unsigned char bootRom[256] = 
			{0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
            0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
            0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
            0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
            0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
            0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
            0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
            0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
            0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
            0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
            0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
            0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
            0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
            0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,
            0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x00, 0x00, 0x23, 0x7D, 0xFE, 0x34, 0x20,
            0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x00, 0x00, 0x3E, 0x01, 0xE0, 0x50};
//unsigned char hram[0x80];
//int powerUp = 0;

void loadRom(){
	FILE *cart;

	cart = fopen("roms/Dr. Mario (W) (V1.1).gb", "rb");
	fread(rom, 0x8000, 1, cart);
}

void copyBiosToRom(){
	memcpy(rom, bootRom, 0x100);
}


unsigned char readByte(unsigned short address){
	if(address <= 0x7fff){
		return rom[address];
	}else if(address >= 0x8000 && address <= 0x9fff){
		return vram[address - 0x8000];
	}else if(address >= 0xA000 && address <= 0xbfff){
		return sram[address - 0xA000];
	}else if(address >= 0xc000 && address <= 0xdfff){
		return wram[address - 0xc000];
	}else if(address >= 0xfe00 && address <= 0xfe9f){
		return oam[address - 0xfe00];
	}else if(address >= 0xff00 && address <= 0xff7f){
		if(address == 0xff00){
			return getJoyState();
		}else{
			return io[address - 0xff00];
		}

	}else if(address >= 0xff80 && address <= 0xfffe){
		return hram[address - 0xff80];
	}else if(address > 0xfffe){
		return intio[address - 0xffff];
	}else{
		return 0;
	}
}

unsigned short readShort(unsigned short address){
	return readByte(address) | (readByte(address + 1) << 8);
}

void writeByte(unsigned short address, unsigned char byte){
	//rom[address] = byte;
	if(address <= 0x7fff){
		return;
	}else if(address >= 0x8000 && address <= 0x9fff){
		vram[address - 0x8000] = byte;
		//if(address == 0x9800){
			//printf("wrote byte: %x\n", byte);
		//}
	}else if(address >= 0xA000 && address <= 0xbfff){
		sram[address - 0xA000] = byte;

	}else if(address >= 0xc000 && address <= 0xdfff){
		wram[address - 0xc000] = byte;
		//printf("hi");
		//printf("wrote: %x", byte);
	}else if(address >= 0xfe00 && address <= 0xfe9f){
		oam[address - 0xfe00] = byte;

	}else if(address >= 0xff00 && address <= 0xff7f){
		if(address == 0xff46){
			DMAtransfer(byte);
		}else if(address == 0xff00){
			setBit(byte);
		}else{
			io[address - 0xff00] = byte;
		}

	}else if(address >= 0xff80 && address <= 0xfffe){
		hram[address - 0xff80] = byte;

	}else if(address > 0xfffe){
		intio[address - 0xffff] = byte;
	}
}

void writeShort(unsigned short address, unsigned short val){
	//printf("write");
	writeByte(address, (unsigned char)(val & 0x00ff));
	writeByte(address + 1, (unsigned char)((val & 0xff00) >> 8));
}

void DMAtransfer(unsigned char source){
	unsigned short address = source << 8;

	for(int i = 0; i < 0xa0; i++){
		writeByte(0xfe00 + i, readByte(address + i));
	}
}