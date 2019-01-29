typedef struct Sprite{
	unsigned char y;
	unsigned char x;
	unsigned char tileNo;

	// byte no 3 is display options for sprite
	unsigned int priority;
	unsigned int yFlip;
	unsigned int xFlip;
	unsigned int pallete;

}Sprite;

int checkLCDEnable();
void gpu(unsigned char lastCycle);
void writeLCDCStatus(int status);
void writeLCDCYCoord(unsigned char scanline);
unsigned char getLCDCYCoord();
int getScrollX();
unsigned int getScrollY();
unsigned char getBGPallete();
unsigned char getOBPalette(int palleteNo);
unsigned short getBGTileDataBase();
unsigned short getBGMapBase();
int applyPalette(unsigned char pallete, int data);
void renderTiles(int scanLine);
int getPixelData(unsigned char byte1, unsigned char byte2, int idx);
unsigned char testRender(int scanline, int x);
void oamSearch();
unsigned int oamDraw(int scanLine, int x, int bgPixel);

void test();