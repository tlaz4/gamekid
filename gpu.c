#include <unistd.h>

// gpu cycle goes:
// oam search (search for sprites visible in that line)
// draw background and sprites for the background
// have time to hblank and draw next line
// do this 144 times, then enter vblank to reset to line 0

#include "cpu.h"
#include "memory.h"
#include "gui.h"
#include "gpu.h"

int LCDScreen[160][144];
Sprite spriteArray[40];

int checkLCDEnable(){
	unsigned char LCDC = readByte(0xff40);
	if(LCDC & 0x80){
		return 1;
	}else{
		return 0;
	}
}

unsigned short getBGTileDataBase(){
	unsigned char LCDC = readByte(0xff40);
	if(LCDC & 0x10){
		return 0x8000;
	}else{
		return 0x8800;
	}
}

unsigned short getBGMapBase(){
	unsigned char LCDC = readByte(0xff40);
	if(LCDC & 0x08){
		return 0x9c00;
	}else{
		return 0x9800;
	}
}

void writeLCDCYCoord(unsigned char scanline){
	writeByte(0xff44, scanline);
}

unsigned char getLCDCYCoord(){
	return readByte(0xff44);
}

// will need to handle interrupt status later, for now only handles modes
void writeLCDCStatus(int status){
	unsigned char curStat = readByte(0xff41) & 0xfc;
	curStat += status;
	writeByte(0xff41, curStat);
}

unsigned int getScrollY(){
	return readByte(0xff42);
}

int getScrollX(){
	return (unsigned int)readByte(0xff43);
}

unsigned char getBGPallete(){
	return readByte(0xff47);
}

unsigned char getOBPalette(int palleteNo){
	if(palleteNo == 0){
		return readByte(0xff48);
	}else{
		return readByte(0xff49);
	}
}

// apply the pallete to a pixel by bit bit shifting by the pixels value and extracting the colour from the pallette
int applyPalette(unsigned char pallete, int data){
	unsigned char mask = 0x03;
	int colour = (mask << (data * 2)) & pallete;

	return colour >> (data * 2);
}

int getPixelData(unsigned char byte1, unsigned char byte2, int idx){
	unsigned char mask = 0x01 << idx;
	int data = ((byte1 & mask) >> idx) | (((byte2 & mask) << 1) >> idx);

	return data;
}

unsigned char testRender(int scanLine, int x){
	// what line on the background should we be drawing
	// tile row will help find where we fetch the tiles from in the map
	printf("getScrollY is %x\n", getScrollY());
	int y = (getScrollY() + scanLine) % 256;
	int tileRow = (unsigned int)(y >> 3);

	// find the tile of each pixel in this scanline, taking into consideration th scroll

	int z = getScrollX() + x;

	printf("at 0x9800: %x\n",readByte(0x9800));

	int rowOffset = (unsigned int)(z >> 3);

	char tileAddrs = readByte(0x9800 + (32 * tileRow) + rowOffset);

		// from the tile address we can then address the bytes containing that line
		// and store the line in two bytes so we can double up the bits for color purposes
		//unsigned char byte1 = readByte(0x8000 + (tileAddrs * 16) + (y % 8) * 2);
		//unsigned char byte2 = readByte(0x8000 + (tileAddrs * 16) + (y % 8) * 2 + 1);

		//return getPixelData(byte1, byte2, 7 - (x % 8));
	return tileAddrs;

}

void test(){
	unsigned char byte1 = readByte(0x8002);
	unsigned char byte2 = readByte(0x8003);

	printf("bytes are: %x %x\n", byte1, byte2);

}

void renderTiles(int scanLine){
	// what line on the background should we be drawing
	// tile row will help find where we fetch the tiles from in the map
	int y = (getScrollY() + scanLine) % 256;
	int tileRow = (unsigned int)(y >> 3);

	// find the tile of each pixel in this scanline, taking into consideration th scroll
	for(int i = 0; i < 160; i++){
		int x = getScrollX() + i;

		int rowOffset = (unsigned int)(x >> 3);

		unsigned char tileAddrs = readByte(getBGMapBase() + (32 * tileRow) + rowOffset);
		
		// from the tile address we can then address the bytes containing that line
		// and store the line in two bytes so we can double up the bits for color purposes
		unsigned short tileBase = getBGTileDataBase();
		unsigned char byte1 = readByte(tileBase + (tileAddrs * 16) + (y % 8) * 2);
		unsigned char byte2 = readByte(tileBase + (tileAddrs * 16) + (y % 8) * 2 + 1);

		// we can then extract the pixels for this particular tile using the x coord, 
		//and apply the palette from the extracted pixel, storing accordingly into the lcd array
		int pixel = getPixelData(byte1, byte2, 7 - (x % 8));

		int bgPixel = applyPalette(getBGPallete(), pixel);
		int spritePixel = oamDraw(scanLine, i);

		// check which pixel to display based on priority of sprite
		// where a value less than 3 is to display the sprite, a value of 4 means the sprite doesnt have priority
		// and 5 means there is no sprite at this pixel
		if(spritePixel <= 3){
			LCDScreen[i][scanLine] = spritePixel;
		}else if(spritePixel == 4 && bgPixel == 0){
			LCDScreen[i][scanLine] = spritePixel;
		}else{
			LCDScreen[i][scanLine] = bgPixel;
		}
	}
}

void oamSearch(){
	// go through all sprites and add them to our sprite list
	int i = 0;

	for(unsigned short mem = 0xfe00; mem <= 0xfe9f; mem += 4){
		spriteArray[i].y = readByte(mem) - 16;
		spriteArray[i].x = readByte(mem + 1) - 8;
		spriteArray[i].tileNo = readByte(mem + 2);

		unsigned char byte4 = readByte(mem + 3);
		spriteArray[i].priority = (byte4 & 0x80) >> 7;
		spriteArray[i].yFlip = (byte4 & 0x40) >> 6;
		spriteArray[i].xFlip = (byte4 & 0x20) >> 5;
		spriteArray[i].pallete = (byte4 & 0x10) >> 4;

		if(spriteArray[i].tileNo != 0){
			printf("%x\n", spriteArray[i].tileNo);
		}

		i++;
	}
}

unsigned int oamDraw(int scanLine, int x){
	for(int i = 0; i < 40; i++){
		Sprite sprite = spriteArray[i];

		if(scanLine >= sprite.y && (scanLine < (sprite.y + 8))
		&& x >= sprite.x && (x < (sprite.x + 8))){

			unsigned char byte1 = readByte(0x8000 + (sprite.tileNo * 16) + (scanLine % 8) * 2);
			unsigned char byte2 = readByte(0x8000 + (sprite.tileNo * 16) + (scanLine % 8) * 2 + 1);

			// we can then extract the pixels for this particular tile using the x coord, 
			//and apply the palette from the extracted pixel, storing accordingly into the lcd array
			int pixel = getPixelData(byte1, byte2, 7 - (x % 8));
			int palleteData = applyPalette(getOBPalette(sprite.pallete), pixel);

			// the sprite has a priority of 1, it is to be drawn behind the background, else draw it
			if(sprite.priority == 1){
				return 4;
			}else{
				return palleteData;
			}
		}
	}

	// there are no sprites to draw here
	return 5;
}

void renderSprites(){

}

void gpu(unsigned char lastCycle){ 
	enum gpuModes{HBlank, VBlank, OAMSearch, PixelTransfer};

	static short gpuClock = 0;
	static unsigned char scanLine = 0;
	static enum gpuModes gpuMode = OAMSearch;

	if(checkLCDEnable()){
		// LCD is on, increment our gpu clock, and then check whoch mode we are in
		gpuClock += lastCycle;
		switch(gpuMode){
			// we are in OAMSearch
			case OAMSearch:
				// stay in oamsearch, once out search for sprites and change modes
				if(gpuClock >= 80){
					oamSearch();
					gpuMode = PixelTransfer;
					writeLCDCStatus(3);
				}
				break;

			case PixelTransfer:
				if(gpuClock >= 252){
					//pixel transfer
					renderTiles(scanLine);
					writeLCDCYCoord(scanLine);
					if(scanLine == 143){
						// draw frame to lcd
						drawScreen(LCDScreen);
						gpuMode = VBlank;
						writeLCDCStatus(1);
						gpuClock = 0;
						scanLine++;
					}else{
						gpuMode = HBlank;
						writeLCDCStatus(0);
					}
				}
				break;

			case HBlank:
				if(gpuClock >= 456){
					//hblank
					usleep(1);
					scanLine++;
					gpuClock = 0;
					gpuMode = OAMSearch;
					writeLCDCStatus(2);
				}
				break;

			case VBlank:
				writeByte(0xff0f, 0x01);
				if(gpuClock >= 456){
					writeLCDCYCoord(scanLine);
					gpuClock = 0;
					scanLine++;
					usleep(2);
				}if(scanLine > 153){
					scanLine = 0;
					gpuClock = 0;
					gpuMode = OAMSearch;
					writeLCDCStatus(2);
				}
				break;
		}
	}else{
		// lcd is turned off, no need for drawing
		scanLine = 0;
		return;
	}
}
