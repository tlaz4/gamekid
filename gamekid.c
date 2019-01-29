#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
	unsigned char a;

	unsigned char f;

	unsigned char b;
	unsigned char c;
	unsigned short bc;

	unsigned char d;
	unsigned char e;
	unsigned short de;

	unsigned char h;
	unsigned char l;
	unsigned short hl;

	unsigned short pc;
	unsigned short sp;
}CPURegisters;

typedef struct{
	char *instruction;
	unsigned char operandLen;
	unsigned char cycleLen;
	void (*execute)(CPURegisters *registers, unsigned char *cartMem);
}CPUInstruction;
	

int getCartLen(FILE *cart);
void powerUpSequence(unsigned char *cartMem, CPURegisters *registers);
void initRegisters(CPURegisters *registers);
unsigned char readByte(unsigned short address, unsigned char *cartMem);
void writeByte(unsigned short address, unsigned char *cartMem, unsigned char sp);
void CPUFunction(char opcode, CPURegisters *registers, CPUInstruction *instructions, unsigned char *cartMem);

void alignRegistersBC(CPURegisters *registers);
void alignRegistersBCByte(CPURegisters *registers);
void alignRegistersHL(CPURegisters *registers);
void alignRegistersHLByte(CPURegisters * registers);
void alignRegistersDE(CPURegisters *registers);
void alignRegistersDEByte(CPURegisters *registers);

void setZeroFlag(CPURegisters *registers);
void resetZeroFlag(CPURegisters *registers);
unsigned int getZeroFlag(CPURegisters *registers);
void setSubFlag(CPURegisters *registers);
void resetSubFlag(CPURegisters *registers);
void setCarryFlag(CPURegisters *registers);
void resetCarryFlag(CPURegisters *registers);
unsigned int getCarryFlag(CPURegisters *registers);
void setHalfCarryFlag(CPURegisters *registers);
void resetHalfCarryFlag(CPURegisters *registers);
unsigned int getHalfCarryFlag(CPURegisters *registers);
void checkHalfCarryFlag8B(CPURegisters * registers, unsigned int x, unsigned int y);
void checkCarry8B(CPURegisters *registers, unsigned int x, unsigned int y);
void checkHalfCarryFlag16B(CPURegisters *registers, unsigned int x, unsigned int y);
void checkCarry16B(CPURegisters *registers, unsigned int x, unsigned int y);

void debug(CPURegisters *registers, CPUInstruction instruction);

// all instruction functions
void NOP(CPURegisters *registers, unsigned char *cartMem);
void LDBCn(CPURegisters *registers, unsigned char *cartMem);
void LDBCA(CPURegisters *registers, unsigned char *cartMem);
void INCBC(CPURegisters *registers, unsigned char *cartMem);
void INCB(CPURegisters *registers, unsigned char *cartMem);
void DECB(CPURegisters *registers, unsigned char *cartMem);
void RLCA(CPURegisters *registers, unsigned char *cartMem);
void LDB(CPURegisters *registers, unsigned char *cartMem);
void LDnnSP(CPURegisters *registers, unsigned char *cartMem);
void ADDHLBC(CPURegisters *registers, unsigned char *cartMem);
void LDABC(CPURegisters *registers, unsigned char *cartMem);
void DECBC(CPURegisters *registers, unsigned char *cartMem);
void INCC(CPURegisters *registers, unsigned char *cartMem);
void DECC(CPURegisters *registers, unsigned char *cartMem);
void LDC(CPURegisters *registers, unsigned char *cartMem);
void RRCA(CPURegisters *registers, unsigned char *cartMem);

void LDDEn(CPURegisters *registers, unsigned char *cartMem);
void LDDEA(CPURegisters *registers, unsigned char *cartMem);
void INCDE(CPURegisters *registers, unsigned char *cartMem);
void INCD(CPURegisters *registers, unsigned char *cartMem);
void DECD(CPURegisters *registers, unsigned char *cartMem);
void LDD(CPURegisters *registers, unsigned char *cartMem);
void RLA(CPURegisters *registers, unsigned char *cartMem);
void JRN(CPURegisters *registers, unsigned char *cartMem);
void ADDHLDE(CPURegisters *registers, unsigned char *cartMem);
void LDADE(CPURegisters *registers, unsigned char *cartMem);
void DECDE(CPURegisters *registers, unsigned char *cartMem);
void INCE(CPURegisters *registers, unsigned char *cartMem);
void DECE(CPURegisters *registers, unsigned char *cartMem);
void RRA(CPURegisters *registers, unsigned char *cartMem);
void LDE(CPURegisters *registers, unsigned char *cartMem);

void JRNZn(CPURegisters *registers, unsigned char *cartMem);
void LDHL(CPURegisters *registers, unsigned char *cartMem);
void LDIHLA(CPURegisters *registers, unsigned char *cartMem);
void INCHL(CPURegisters *registers, unsigned char *cartMem);
void INCH(CPURegisters *registers, unsigned char *cartMem);
void DECH(CPURegisters *registers, unsigned char *cartMem);
void LDH(CPURegisters *registers, unsigned char *cartMem);
void DAA(CPURegisters *registers, unsigned char *cartMem);
void JRZn(CPURegisters *registers, unsigned char *cartMem);
void ADDHLHL(CPURegisters *registers, unsigned char *cartMem);
void LDIAHL(CPURegisters *registers, unsigned char *cartMem);
void DECHL(CPURegisters *registers, unsigned char *cartMem);
void INCL(CPURegisters *registers, unsigned char *cartMem);
void DECL(CPURegisters *registers, unsigned char *cartMem);
void LDL(CPURegisters *registers, unsigned char *cartMem);
void CPL(CPURegisters *registers, unsigned char *cartMem);

void JRNCn(CPURegisters *registers, unsigned char *cartMem);
void LDSP(CPURegisters *registers, unsigned char *cartMem);
void LDDHLA(CPURegisters *registers, unsigned char *cartMem);
void INCSP(CPURegisters *registers, unsigned char *cartMem);
void INCHLa(CPURegisters *registers, unsigned char *cartMem);
void DECHLa(CPURegisters *registers, unsigned char *cartMem);
void LDHLaddrs(CPURegisters *registers, unsigned char *cartMem);
void SCF(CPURegisters *registers, unsigned char *cartMem);
void JRCn(CPURegisters *registers, unsigned char *cartMem);
void ADDHLSP(CPURegisters *registers, unsigned char *cartMem);
void LDDAHL(CPURegisters *registers, unsigned char *cartMem);
void DECSP(CPURegisters *registers, unsigned char *cartMem);
void INCA(CPURegisters *registers, unsigned char *cartMem);
void DECA(CPURegisters *registers, unsigned char *cartMem);
void CCF(CPURegisters *registers, unsigned char *cartMem);

void LDBB(CPURegisters *registers, unsigned char *cartMem);
void LDBC(CPURegisters *registers, unsigned char *cartMem);
void LDBD(CPURegisters *registers, unsigned char *cartMem);
void LDBE(CPURegisters *registers, unsigned char *cartMem);
void LDBH(CPURegisters *registers, unsigned char *cartMem);
void LDBL(CPURegisters *registers, unsigned char *cartMem);
void LDBHL(CPURegisters *registers, unsigned char *cartMem);
void LDBA(CPURegisters *registers, unsigned char *cartMem);

void JPnn(CPURegisters *registers, unsigned char *cartMem);


int main(){

	FILE *cart;
	unsigned char *cartMem;
	CPURegisters registers;

	// need to implement LDBCA, LDABC, stop 0
	CPUInstruction instructions[256] = {
		{"NOP", 1, 4, NOP}, {"LD BC", 3, 12, LDBCn}, {"LD (BC)A", 1, 8, LDBCA}, {"INC BC", 1, 8, INCBC}, {"INC B", 1, 4, INCB}, {"DEC B", 1, 4, DECB}, {"LD B", 2, 8, LDB}, {"RLCA", 1, 4, RLCA}, {"LD (nn)SP", 3, 20, LDnnSP},
		{"ADD HL,BC", 1, 8, ADDHLBC}, {"LD A,(BC)", 1, 8, LDABC}, {"DEC BC", 1, 8, DECBC}, {"INC C", 1, 4, INCC}, {"DEC C", 1, 4, DECC}, {"LD C", 2, 8, LDC}, {"RRCA", 1, 4, RRCA}, {"STOP 0", 2, 4, NULL}, {"LD DE", 3, 12, LDDEn},
		{"LD (DE)A", 1, 8, LDDEA}, {"INC DE", 1, 8, INCDE}, {"INC D", 1, 4, INCD}, {"DEC D", 1, 4, DECD}, {"LD D", 2, 8, LDD}, {"RLA", 1, 4, RLA}, {"JR n", 2, 12, JRN}, {"ADD HL,DE", 1, 8, ADDHLDE}, {"LD A,(DE)", 1, 8, LDADE},
		{"DEC DE", 1, 8, DECDE}, {"INC E", 1, 4, INCE}, {"DEC E", 1, 4, DECE}, {"LD E", 2, 8, LDE}, {"RRA", 1, 4, RRA}, {"JR NZ n", 2, 8, JRNZn}, {"LD HL", 3, 12, LDHL}, {"LDI (HL), A", 1, 8, LDIHLA}, {"INC HL", 1, 8, INCHL},
		{"INC H", 1, 4, INCH}, {"DEC H", 1, 4, DECH}, {"LD H", 2, 8, LDH},{"DAA", 1, 4, DAA}, {"JR Z n", 2, 8, JRZn}, {"ADD HL,HL", 1, 8, ADDHLHL}, {"LDI A, (HL+)", 1, 8, LDIAHL}, {"DEC HL", 1, 8, DECHL}, {"INC L", 1, 4, INCL},
		{"DEC L", 1, 4, DECL}, {"LD L", 2, 8, LDL}, {"CPL", 1, 4, CPL}, {"JR NC n", 2, 8, JRNCn}, {"LD SP", 3, 12, LDSP}, {"LDD (HL), A", 1, 8, LDDHLA}, {"INC SP", 1, 8, INCSP}, {"INC (HL)", 1, 12, INCHLa}, {"DEC (HL)", 1, 12, DECHLa},
		{"LD (HL),n", 2, 12, LDHLaddrs}, {"SCF", 1, 4, SCF}, {"JR C n", 2, 8, JRCn}, {"ADD HL,SP", 1, 8, ADDHLSP}, {"LDD A, (HL)", 1, 8, LDDAHL}, {"DEC SP", 1, 8, DECSP}, {"INC A", 1, 4, INCA}, {"DEC A", 1, 4, DECA}, {"CCF", 1, 4, CCF}, 
		{"LD B,B", 1, 4, LDBB}, {"LD B,C", 1, 4, LDBC}, {"LD B,D", 1, 4, LDBD}, {"LD B,E", 1, 4, LDBE}, {"LD B,H", 1, 4, LDBH}, {"LD B,L", 1, 4, LDBL}, {"LD B,(HL)", 1, 8, LDBHL}, {"LD B,A", 1, 4, LDBA}};

	// load cartidge into memory
	// test is Tetris.gb
	cart = fopen("Tetris.gb", "rb");
	int cartLen = getCartLen(cart);

	cartMem = (unsigned char*)malloc(cartLen * sizeof(unsigned char));
	fread(cartMem, cartLen, 1, cart);

	powerUpSequence(cartMem, &registers);

	//registers.pc = 0x0000;

	unsigned char opcode = readByte(registers.pc, cartMem);
	CPUFunction(opcode, &registers, instructions, cartMem);
	//printf("%x, %x\n", registers.hl, registers.f);

	//CPUInstruction instruction = {"nop", 2, print};
	//instruction[0].execute();

	return 1;
	
}

int getCartLen(FILE *cart){
	int cartLen;

	fseek(cart, 0, SEEK_END);          
	cartLen = ftell(cart);             
	rewind(cart); 

	return cartLen;
}

// power up sequence as in gameboy docs, extract the nintendo logo and then initialize registers to default values
void powerUpSequence(unsigned char *cartMem, CPURegisters *registers){
	unsigned char *logoBuffer;

	logoBuffer = malloc(48 * sizeof(unsigned char));
	// display logo;
	memcpy(logoBuffer, &cartMem[260], 48);

	// init registers to reset values
	initRegisters(registers);
}

// initialize registers to default values
void initRegisters(CPURegisters *registers){
	registers->a = 1;
	registers->f = 176;
	registers->b = 0;
	registers->c = 18;
	registers->d = 0;
	registers->e = 216;
	registers->h = 1;
	registers->l = 77;

	registers->bc = 18;
	registers->de = 216;
	registers->hl = 177;

	registers->pc = 256;
	registers->sp = 65534;
}


// read a byte from memory
unsigned char readByte(unsigned short address, unsigned char *cartMem){
	return cartMem[address];
}

void writeByte(unsigned short address, unsigned char *cartMem, unsigned char sp){
	cartMem[address] = sp;
}

// act on extracted instruction on the cpu
// and then increment cpu accordingly
void CPUFunction(char opcode, CPURegisters *registers, CPUInstruction *instructions, unsigned char *cartMem){
	CPUInstruction instruction = instructions[(unsigned int)opcode];

	instruction.execute(registers, cartMem);

	debug(registers, instruction);

	registers->pc += instruction.operandLen;	

}

void debug(CPURegisters *registers, CPUInstruction instruction){
	printf("testing instruction: %s\n", instruction.instruction);
	printf("af: %x%x\n", registers->a, registers->f);
	printf("bc: %x\n", registers->bc);
	printf("de: %x\n", registers->de);
	printf("hl: %x\n", registers->hl);
	printf("pc: %x\n", registers->pc);
	printf("sp: %x\n", registers->sp);
}

void setZeroFlag(CPURegisters *registers){
	unsigned char x = 1 << 7;
	registers->f |= x;

}

void resetZeroFlag(CPURegisters *registers){
	unsigned char x = 0x7f;
	registers->f &= x;
}

unsigned int getZeroFlag(CPURegisters *registers){
	return (registers->f &= 0x80) >> 7;
}

void setSubFlag(CPURegisters *registers){
	unsigned char x = 1 << 6;
	registers->f |= x;
}

void resetSubFlag(CPURegisters *registers){
	unsigned char x = 0xBF;
	registers->f &= x;
}

void setCarryFlag(CPURegisters *registers){
	registers->f |= 0x10;
}

void resetCarryFlag(CPURegisters *registers){
	registers->f &= 0xEF;
}

unsigned int getCarryFlag(CPURegisters *registers){
	return (registers->f &= 0x10) >> 4;
}

void setHalfCarryFlag(CPURegisters *registers){
	registers->f |= 0x20;
}

void resetHalfCarryFlag(CPURegisters *registers){
	registers->f &= 0xDF;
}

unsigned int getHalfCarryFlag(CPURegisters *registers){
	return (registers->f &= 0x20) >> 5;
}

void checkHalfCarryFlag8B(CPURegisters * registers, unsigned int x, unsigned int y){
	if((((x & 0xf) + (y & 0xf)) & 0x10) == 0x10){
		setHalfCarryFlag(registers);
	}
}

void checkCarry8B(CPURegisters *registers, unsigned int x, unsigned int y){
	if(x + y > 0xff){
		setCarryFlag(registers);
	}
}

void checkHalfCarryFlag16B(CPURegisters *registers, unsigned int x, unsigned int y){
	if((((x & 0x0f00) + (y & 0x0f00)) & 0x0100) == 0x0100){
		setHalfCarryFlag(registers);
	}
}

void checkCarry16B(CPURegisters *registers, unsigned int x, unsigned int y){
	if(x + y > 0xffff){
		setCarryFlag(registers);
	}
}

void alignRegistersBC(CPURegisters *registers){
	registers->bc = registers->b;
	registers->bc <<= 8;
	registers->bc |= registers->c;
}

void alignRegistersBCByte(CPURegisters *registers){
	registers->b = 0x00ff & (registers->bc >> 8);
    registers->c = 0x00ff & registers->bc;
}

void alignRegistersHL(CPURegisters *registers){
	registers->hl = registers->h;
	registers->hl <<= 8;
	registers->hl |= registers->l;
}

void alignRegistersHLByte(CPURegisters *registers){
	registers->h = 0x00ff & (registers->hl >> 8);
    registers->l = 0x00ff & registers->hl;
}

void alignRegistersDE(CPURegisters *registers){
	registers->de = registers->d;
	registers->de <<= 8;
	registers->de |= registers->e;
}

void alignRegistersDEByte(CPURegisters *registers){
	registers->d = 0x00ff & (registers->de >> 8);
    registers->e = 0x00ff & registers->de;
}

void NOP(CPURegisters *registers, unsigned char *cartMem){
	return;
}


void LDBCn(CPURegisters *registers, unsigned char *cartMem){
	registers->c = readByte(registers->pc + 1, cartMem);
	registers->b = readByte(registers->pc + 2, cartMem);
	alignRegistersBC(registers);
}

void LDBCA(CPURegisters *registers, unsigned char *cartMem){
	writeByte(registers->bc, cartMem, registers->a);
}

void INCBC(CPURegisters *registers, unsigned char *cartMem){
	registers->bc++;
	alignRegistersBCByte(registers);
}

void INCB(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->b, 1);
	registers->b++;
	if(registers->b == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersBC(registers);
}

void DECB(CPURegisters *registers, unsigned char *cartMem){
	registers->b--;
	//checkHalfCarryFlag8B(registers);
	if(registers->b == 0){
		setZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersBC(registers);
}

void LDB(CPURegisters *registers, unsigned char *cartMem){
	registers->b = readByte(registers->pc + 1, cartMem);
	alignRegistersBC(registers);
}

void RLCA(CPURegisters *registers, unsigned char *cartMem){
	if((registers->a >> 7) == 1){
		setCarryFlag(registers);
		registers->a <<= 1;
		registers->a++;
	}else{
		registers->a <<= 1;
		resetCarryFlag(registers);
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void LDnnSP(CPURegisters *registers, unsigned char *cartMem){
	// must write stack pointer to address, research it a bit before implementing, i assume you write one byte at address and the next byte at address + 1
	unsigned short memAddress = readByte(registers->pc + 1, cartMem) | (readByte(registers->pc + 2, cartMem) << 8);
	writeByte(memAddress, cartMem, (unsigned char)(registers->sp & 0x00ff));
	writeByte(memAddress + 1, cartMem, (unsigned char)(registers->sp & 0xff00));
}

void ADDHLBC(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->bc);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->bc);
	registers->hl += registers->bc;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDABC(CPURegisters *registers, unsigned char *cartMem){
	registers->a = readByte(registers->bc, cartMem);
}

void DECBC(CPURegisters *registers, unsigned char *cartMem){
	registers->bc--;
	alignRegistersBCByte(registers);
}

void INCC(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->c, 1);
	registers->c++;
	if(registers->c == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersBC(registers);
}

void DECC(CPURegisters *registers, unsigned char *cartMem){
	//need to check half carry flag
	registers->c--;
	if(registers->c == 0){
		setZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersBC(registers);
}

void LDC(CPURegisters *registers, unsigned char *cartMem){
	registers->c = readByte(registers->pc + 1, cartMem);
	alignRegistersBC(registers);
}

void RRCA(CPURegisters *registers, unsigned char *cartMem){
	unsigned char carry = registers->a & 0x01;
	if(carry == 1){
		setCarryFlag(registers);
		registers->a >>= 1;
		registers->a |= 0x80;
	}else{
		registers->a >>= 1;
		resetCarryFlag(registers);
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void LDDEn(CPURegisters *registers, unsigned char *cartMem){
	registers->e = readByte(registers->pc + 1, cartMem);
	registers->d = readByte(registers->pc + 2, cartMem);
	alignRegistersDE(registers);
}

void LDDEA(CPURegisters *registers, unsigned char *cartMem){
	writeByte(registers->de, cartMem, registers->a);
}

void INCDE(CPURegisters *registers, unsigned char *cartMem){
	registers->de++;
	alignRegistersDEByte(registers);
}

void INCD(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->d, 1);
	registers->d++;
	if(registers->d == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersDE(registers);
}

void DECD(CPURegisters *registers, unsigned char *cartMem){
	// need to check half carry flag on subtract
	registers->d--;
	if(registers->d == 0){
		setZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersDE(registers);
}

void LDD(CPURegisters *registers, unsigned char *cartMem){
	registers->d = readByte(registers->pc + 1, cartMem);
	alignRegistersDE(registers);
}

void RLA(CPURegisters *registers, unsigned char *cartMem){
	unsigned int carry = getCarryFlag(registers);

	if(registers->a & 0x80){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->a <<= 1;
	registers->a += carry;

	if(registers->a == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void JRN(CPURegisters *registers, unsigned char *cartMem){
	registers->pc += (signed char)readByte(registers->pc + 1, cartMem);
}

void ADDHLDE(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->de);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->de);
	registers->hl += registers->de;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDADE(CPURegisters *registers, unsigned char *cartMem){
	registers->a = readByte(registers->de, cartMem);
}

void DECDE(CPURegisters *registers, unsigned char *cartMem){
	registers->de--;
	alignRegistersDEByte(registers);
}

void INCE(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->e, 1);
	registers->e++;
	if(registers->e == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersDE(registers);
}

void DECE(CPURegisters *registers, unsigned char *cartMem){
	// need to check half carry flag on subtract
	registers->e--;
	if(registers->e == 0){
		setZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersDE(registers);
}

void LDE(CPURegisters *registers, unsigned char *cartMem){
	registers->e = readByte(registers->pc + 1, cartMem);
	alignRegistersDE(registers);
}

void RRA(CPURegisters *registers, unsigned char *cartMem){
	unsigned int carry = getCarryFlag(registers);

	if(registers->a & 0x01){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->a >>= 1;
	if(carry){
		registers->a |= 0x80;
	}

	if(registers->a == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void JRNZn(CPURegisters *registers, unsigned char *cartMem){
	if(!getZeroFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1, cartMem);
	}
}

void LDHL(CPURegisters *registers, unsigned char *cartMem){
	registers->l = readByte(registers->pc + 1, cartMem);
	registers->h = readByte(registers->pc + 2, cartMem);
	alignRegistersHL(registers);

}

void LDIHLA(CPURegisters *registers, unsigned char *cartMem){
	writeByte(registers->hl++, cartMem, registers->a);
	alignRegistersHLByte(registers);
}

void INCHL(CPURegisters *registers, unsigned char *cartMem){
	registers->hl++;
	alignRegistersHLByte(registers);
}

void INCH(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->h, 1);
	registers->h++;
	if(registers->h == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersHL(registers);
}

void DECH(CPURegisters *registers, unsigned char *cartMem){
	// need to check half carry flag on subtract
	registers->h--;
	if(registers->h == 0){
		setZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersHL(registers);
}

void LDH(CPURegisters *registers, unsigned char *cartMem){
	registers->h = readByte(registers->pc + 1, cartMem);
	alignRegistersHL(registers);
}

void DAA(CPURegisters *registers, unsigned char *cartMem){
	unsigned char daa = registers->a;
	if((daa & 0x0f) > 9 ||  getHalfCarryFlag(registers)){
		daa += 6;
	}
	if(((daa & 0xf0) >> 4) > 9 ||  getCarryFlag(registers)){
		daa += 96;
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	if(daa == 0){
		setZeroFlag(registers);
	}
	resetHalfCarryFlag(registers);

	registers->a = daa;
}

void JRZn(CPURegisters *registers, unsigned char *cartMem){
	if(getZeroFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1, cartMem);
	}
}

void ADDHLHL(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->hl);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->hl);
	registers->hl += registers->hl;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDIAHL(CPURegisters *registers, unsigned char *cartMem){
	registers->a = readByte(registers->hl++, cartMem);
}

void DECHL(CPURegisters *registers, unsigned char *cartMem){
	registers->hl--;
	alignRegistersHLByte(registers);
}

void INCL(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->l, 1);
	registers->l++;
	if(registers->l == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersHL(registers);
}

void DECL(CPURegisters *registers, unsigned char *cartMem){
	// need to check half carry flag on subtract
	registers->l--;
	if(registers->l == 0){
		setZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersHL(registers);
}

void LDL(CPURegisters *registers, unsigned char *cartMem){
	registers->l = readByte(registers->pc + 1, cartMem);
	alignRegistersHL(registers);
}

void CPL(CPURegisters *registers, unsigned char *cartMem){
	registers->a = ~registers->a;
	setSubFlag(registers);
	setHalfCarryFlag(registers);
}

void JRNCn(CPURegisters *registers, unsigned char *cartMem){
	if(!getCarryFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1, cartMem);
	}
}

void LDSP(CPURegisters *registers, unsigned char *cartMem){
	registers->sp = readByte(registers->pc + 2, cartMem);
	registers->sp <<= 8;
	registers->sp |= readByte(registers->pc + 1, cartMem);
}

void LDDHLA(CPURegisters *registers, unsigned char *cartMem){
	writeByte(registers->hl--, cartMem, registers->a);
	alignRegistersHLByte(registers);
}

void INCSP(CPURegisters *registers, unsigned char *cartMem){
	registers->sp++;
}

void INCHLa(CPURegisters *registers, unsigned char *cartMem){
	unsigned char x = readByte(registers->hl, cartMem);
	
	checkHalfCarryFlag8B(registers, (unsigned int)x, 1);
	x += 1;
	if(x == 0){
		setZeroFlag(registers);
	}

	resetSubFlag(registers);
	writeByte(registers->hl, cartMem, (unsigned char)x);
}

void DECHLa(CPURegisters *registers, unsigned char *cartMem){
	unsigned char x = readByte(registers->h, cartMem);

	// check if half carry flag is set in subtraction
	x -= 1;
	if(x == 0){
		setZeroFlag(registers);
	}

	setSubFlag(registers);
	writeByte(registers->hl, cartMem, (unsigned char)x);
}

void LDHLaddrs(CPURegisters *registers, unsigned char *cartMem){
	writeByte(registers->hl, cartMem, readByte(registers->pc + 1, cartMem));
}

void SCF(CPURegisters *registers, unsigned char *cartMem){
	setCarryFlag(registers);
	resetHalfCarryFlag(registers);
	resetSubFlag(registers);
}

void JRCn(CPURegisters *registers, unsigned char *cartMem){
	if(getCarryFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1, cartMem);
	}
}

void ADDHLSP(CPURegisters *registers, unsigned char *cartMem){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->sp);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->sp);
	registers->hl += registers->sp;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDDAHL(CPURegisters *registers, unsigned char *cartMem){
	registers->a = readByte(registers->hl--, cartMem);
}

void DECSP(CPURegisters *registers, unsigned char *cartMem){
	registers->sp--;
}

void INCA(CPURegisters *registers, unsigned char* cartMem){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, 1);
	registers->a++;
	if(registers->a == 0){
		setZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void DECA(CPURegisters *registers, unsigned char *cartMem){
	//need to check half carry flag
	registers->a--;
	if(registers->a == 0){
		setZeroFlag(registers);
	}
	setSubFlag(registers);
}

void CCF(CPURegisters *registers, unsigned char *cartMem){
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
	if(getCarryFlag(registers)){
		resetCarryFlag(registers);
	}else{
		setCarryFlag(registers);
	}
}

void LDBB(CPURegisters *registers, unsigned char *cartMem){
	registers->b = registers->b;
	alignRegistersBC(registers);
}

void LDBC(CPURegisters *registers, unsigned char *cartMem){
	registers->b = registers->c;
	alignRegistersBC(registers);
}

void LDBD(CPURegisters *registers, unsigned char *cartMem){
	registers->b = registers->d;
	alignRegistersBC(registers);
}

void LDBE(CPURegisters *registers, unsigned char *cartMem){
	registers->b = registers->e;
	alignRegistersBC(registers);
}

void LDBH(CPURegisters *registers, unsigned char *cartMem){
	registers->b = registers->h;
	alignRegistersBC(registers);
}

void LDBL(CPURegisters *registers, unsigned char *cartMem){
	registers->b = registers->l;
	alignRegistersBC(registers);
}

void LDBHL(CPURegisters *registers, unsigned char *cartMem){
	registers->b = readByte(registers->hl, cartMem);
	alignRegistersBC(registers);
}

void LDBA(CPURegisters *registers, unsigned char *cartMem){
	registers->b = registers->a;
	alignRegistersBC(registers);
}