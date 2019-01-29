#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "memory.h"
#include "gpu.h"
#include "interrupts.h"

const CPUInstruction instructions[256] = {
		{"NOP", 1, 4, NOP}, {"LD BC", 3, 12, LDBCn}, {"LD (BC)A", 1, 8, LDBCA}, {"INC BC", 1, 8, INCBC}, {"INC B", 1, 4, INCB}, {"DEC B", 1, 4, DECB}, {"LD B", 2, 8, LDB}, {"RLCA", 1, 4, RLCA}, {"LD (nn)SP", 3, 20, LDnnSP},
		{"ADD HL,BC", 1, 8, ADDHLBC}, {"LD A,(BC)", 1, 8, LDABC}, {"DEC BC", 1, 8, DECBC}, {"INC C", 1, 4, INCC}, {"DEC C", 1, 4, DECC}, {"LD C", 2, 8, LDC}, {"RRCA", 1, 4, RRCA}, //0x00

		{"STOP 0", 2, 4, STOP}, {"LD DE", 3, 12, LDDEn}, {"LD (DE)A", 1, 8, LDDEA}, {"INC DE", 1, 8, INCDE}, {"INC D", 1, 4, INCD}, {"DEC D", 1, 4, DECD}, {"LD D", 2, 8, LDD}, {"RLA", 1, 4, RLA}, {"JR n", 2, 12, JRN}, 
		{"ADD HL,DE", 1, 8, ADDHLDE}, {"LD A,(DE)", 1, 8, LDADE}, {"DEC DE", 1, 8, DECDE}, {"INC E", 1, 4, INCE}, {"DEC E", 1, 4, DECE}, {"LD E", 2, 8, LDE}, {"RRA", 1, 4, RRA}, // 0x01 

		{"JR NZ n", 2, 8, JRNZn}, {"LD HL", 3, 12, LDHLn}, {"LDI (HL), A", 1, 8, LDIHLA}, {"INC HL", 1, 8, INCHL}, {"INC H", 1, 4, INCH}, {"DEC H", 1, 4, DECH}, {"LD H", 2, 8, LDH},{"DAA", 1, 4, DAA}, {"JR Z n", 2, 8, JRZn}, 
		{"ADD HL,HL", 1, 8, ADDHLHL}, {"LDI A, (HL+)", 1, 8, LDIAHL}, {"DEC HL", 1, 8, DECHL}, {"INC L", 1, 4, INCL}, {"DEC L", 1, 4, DECL}, {"LD L", 2, 8, LDL}, {"CPL", 1, 4, CPL}, //0x02

		{"JR NC n", 2, 8, JRNCn}, {"LD SP", 3, 12, LDSP}, {"LDD (HL), A", 1, 8, LDDHLA}, {"INC SP", 1, 8, INCSP}, {"INC (HL)", 1, 12, INCHLa}, {"DEC (HL)", 1, 12, DECHLa}, {"LD (HL),n", 2, 12, LDHLaddrs}, {"SCF", 1, 4, SCF}, 
		{"JR C n", 2, 8, JRCn}, {"ADD HL,SP", 1, 8, ADDHLSP}, {"LDD A, (HL)", 1, 8, LDDAHL}, {"DEC SP", 1, 8, DECSP}, {"INC A", 1, 4, INCA}, {"DEC A", 1, 4, DECA}, {"LD A", 2, 8, LDA}, {"CCF", 1, 4, CCF}, // 0x03

		{"LD B,B", 1, 4, LDBB}, {"LD B,C", 1, 4, LDBC}, {"LD B,D", 1, 4, LDBD}, {"LD B,E", 1, 4, LDBE}, {"LD B,H", 1, 4, LDBH}, {"LD B,L", 1, 4, LDBL}, {"LD B,(HL)", 1, 8, LDBHL}, {"LD B,A", 1, 4, LDBA}, 
		{"LD C,B", 1, 4, LDCB}, {"LD C,C", 1, 4, LDCC}, {"LD C,D", 1, 4, LDCD}, {"LD C,E", 1, 4, LDCE}, {"LD C,H", 1, 4, LDCH}, {"LD C,L", 1, 4, LDCL}, {"LD C,(HL)", 1, 8, LDCHL}, {"LD C,A", 1, 4, LDCA}, //0x04

		{"LD D,B", 1, 4, LDDB}, {"LD D,C", 1, 4, LDDC}, {"LD D,D", 1, 4, LDDD}, {"LD D,E", 1, 4, LDDE}, {"LD D,H", 1, 4, LDDH}, {"LD D,L", 1, 4, LDDL}, {"LD D,(HL)", 1, 8, LDDHL}, {"LD D,A", 1, 4, LDDA},
		{"LD E,B", 1, 4, LDEB}, {"LD E,C", 1, 4, LDEC}, {"LD E,D", 1, 4, LDED}, {"LD E,E", 1, 4, LDEE}, {"LD E,H", 1, 4, LDEH}, {"LD E,L", 1, 4, LDEL}, {"LD E,(HL)", 1, 8, LDEHL}, {"LD E,A", 1, 4, LDEA}, // 0x05

		{"LD H,B", 1, 4, LDHB}, {"LD H,C", 1, 4, LDHC}, {"LD H,D", 1, 4, LDHD}, {"LD H,E", 1, 4, LDHE}, {"LD H,H", 1, 4, LDHH}, {"LD H,L", 1, 4, LDHL}, {"LD H,(HL)", 1, 8, LDHHL}, {"LD H,A", 1, 4, LDHA},
		{"LD L,B", 1, 4, LDLB}, {"LD L,C", 1, 4, LDLC}, {"LD L,D", 1, 4, LDLD}, {"LD L,E", 1, 4, LDLE}, {"LD L,H", 1, 4, LDLH}, {"LD L,L", 1, 4, LDLL}, {"LD L,(HL)", 1, 8, LDLHL}, {"LD L,A", 1, 4, LDLA}, //0x06

		{"LD (HL),B", 1, 8, LDHLB}, {"LD (HL),C", 1, 8, LDHLC}, {"LD (HL),D", 1, 8, LDHLD}, {"LD (HL),E", 1, 8, LDHLE}, {"LD (HL),H", 1, 8, LDHLH}, {"LD (HL),L", 1, 8, LDHLL}, {"HALT", 1, 4, NULL}, {"LD (HL),A", 1, 8, LDHLA},
		{"LD A,B", 1, 4, LDAB}, {"LD A,C", 1, 4, LDAC}, {"LD A,D", 1, 4, LDAD}, {"LD A,E", 1, 4, LDAE}, {"LD A,H", 1, 4, LDAH}, {"LD A,L", 1, 4, LDAL}, {"LD A,(HL)", 1, 8, LDAHL}, {"LD A,A", 1, 4, LDAA}, //0x07

		{"ADD A,B", 1, 4, ADDAB}, {"ADD A,C", 1, 4, ADDAC}, {"ADD A,D", 1, 4, ADDAD}, {"ADD A,E", 1, 4, ADDAE}, {"ADD A,H", 1, 4, ADDAH}, {"ADD A,L", 1, 4, ADDAL}, {"ADD A,(HL)", 1, 8, ADDAHL}, {"ADD A,A", 1, 4, ADDAA},
		{"ADC A,B", 1, 4, ADCAB}, {"ADC A,C", 1, 4, ADCAC}, {"ADC A,D", 1, 4, ADCAD} ,{"ADC A,E", 1, 4, ADCAE}, {"ADC A,H", 1, 4, ADCAH}, {"ADC A,L", 1, 4, ADCAL}, {"ADC A,(HL)", 1, 8, ADCAHL}, {"ADC A,A", 1, 4, ADCAA}, //0x08

		{"SUB B", 1, 4, SUBB}, {"SUB C", 1, 4, SUBC}, {"SUB D", 1, 4, SUBD}, {"SUB E", 1, 4, SUBE}, {"SUB H", 1, 4, SUBH}, {"SUB L", 1, 4, SUBL}, {"SUB HL", 1, 8, SUBHL}, {"SUB A", 1, 4, SUBA}, 
		{"SBC A,B", 1, 4, SBCAB}, {"SBC A,C", 1, 4, SBCAC}, {"SBC A,D", 1, 4, SBCAD}, {"SBC A,E", 1, 4, SBCAE}, {"SBC A,H", 1, 4, SBCAH}, {"SBC A,L", 1, 4, SBCAL}, {"SBC A,(HL)", 1, 8, SBCAHL}, {"SBC A,A", 1, 4, SBCAA}, //0x09

		{"AND B", 1, 4, ANDB}, {"AND C", 1, 4, ANDC}, {"AND D", 1, 4, ANDD}, {"AND E", 1, 4, ANDE}, {"AND H", 1, 4, ANDH}, {"AND L", 1, 4, ANDL}, {"AND (HL)", 1, 8, ANDHL}, {"AND A", 1, 4, ANDA},
		{"XOR B", 1, 4, XORB}, {"XOR C", 1, 4, XORC}, {"XOR D", 1, 4, XORD}, {"XOR E", 1, 4, XORE}, {"XOR H", 1, 4, XORH}, {"XOR L", 1, 4, XORL}, {"XOR (HL)", 1, 8, XORHL}, {"XOR A", 1, 4, XORA}, //0xa

		{"OR B", 1, 4, ORB}, {"OR C", 1, 4, ORC}, {"OR D", 1, 4, ORD}, {"OR E", 1, 4, ORE}, {"OR H", 1, 4, ORH}, {"OR L", 1, 4, ORL}, {"OR (HL)", 1, 8, ORHL}, {"OR A", 1, 4, ORA},
		{"CP B", 1, 4, CPB}, {"CP C", 1, 4, CPC} ,{"CP D", 1, 4, CPD}, {"CP E", 1, 4, CPE}, {"CP H", 1, 4, CPH}, {"CP L", 1, 4, CPLs}, {"CP (HL)", 1, 8, CPHL}, {"CP A", 1, 4, CPA}, //0xb

		{"RET NZ", 1, 8, RETNZ}, {"POP BC", 1, 12, POPBC}, {"JP NZ,n", 3, 16, JPNZ}, {"JP n", 3, 16, JP}, {"CALL NZ,n", 3, 12, CALLNZ}, {"PUSH BC", 1, 16, PUSHBC}, {"ADD A,n", 2, 8, ADDAn}, {"RST 00", 1, 16, RST00}, 
		{"RET Z", 1, 8, RETZ}, {"RET", 1, 16, RET}, {"JP Z,n", 3, 16, JPZ}, {"PREFIX CB", 2, 12, PREFIXCB}, {"CALL Z,n", 3, 12, CALLZ}, {"CALL n", 3, 24, CALL}, {"ADC A,n", 2, 8, ADCAn}, {"RST 08", 1, 16, RST08}, // 0xc

		{"RET NC", 1, 8, RETNC}, {"POP DE", 1, 12, POPDE}, {"JP NC,n", 3, 16, JPNC}, {}, {"CALL NC,n", 3, 12, CALLNC}, {"PUSH DE", 1, 16, PUSHDE}, {"SUB n", 2, 8, SUBn}, {"RST 10", 1, 16, RST10},
		{"RET C", 1, 8, RETC}, {"RETI", 1, 16, RETI}, {"JP C,n", 3, 16, JPC}, {}, {"CALL C,n", 3, 12, CALLC}, {}, {"SBC A,n", 2, 8, SBCAn}, {"RST 18", 1, 16, RST18}, // 0xd

		{"LDH (n),A", 2, 12, LDHnA}, {"POP HL", 1, 12, POPHL}, {"LD (C),A", 1, 8, LDCAaddr}, {}, {}, {"PUSH HL", 1, 16, PUSHHL}, {"AND n", 2, 8, ANDn}, {"RST 20", 1, 16, RST20},
		{"ADDSPn", 2, 16, ADDSPn}, {"JP HL", 1, 4, JPHL}, {"LD (nn),A", 3, 16, LDnnA}, {}, {}, {}, {"XOR n", 2, 8, XORn}, {"RST 28", 1, 16, RST28}, //0xe

		{"LDH a,(n)", 2, 12, LDHAn}, {"POP AF", 1, 12, POPAF}, {"LD A,(C)", 1, 8, LDACaddr}, {"DI", 1, 4, DI}, {}, {"PUSH AF", 1, 16, PUSHAF}, {"OR n", 2, 8, ORn}, {"RST 30", 1, 16, RST30},
		{"LD HL,SP+n", 2, 12, LDHLSPn}, {"LD SP,HL", 1, 8, LDSPHL}, {"LD A,(nn)", 3, 16, LDAnn}, {"EI", 1, 4, EI}, {}, {}, {"CP n", 2, 8, CPn}, {"RST 38", 1, 16, RST38}}; 

const CPUInstruction cbInstructions[256] = {
	{"RLC B", 2, 8, RLCB}, {"RLC C", 2, 8, RLCC}, {"RLC D", 2, 8, RLCD}, {"RLC E", 2, 8, RLCE}, {"RLC H", 2, 8, RLCH}, {"RLC L", 2, 8, RLCL}, {"RLC HL", 2, 16, RLCHL}, {"RLC A", 2, 8, RLCAC},
	{"RRC B", 2, 8, RRCB}, {"RRC C", 2, 8, RRCC}, {}, {}, {}, {}, {}, {}, //0x00

	{}, {"RL C", 2, 8, RLC}, {}, {}, {}, {}, {}, {},
	{}, {"RR C", 2, 8, RRC}, {"RR D", 2, 8, RRD}, {"RR E", 2, 8, RRE}, {}, {}, {}, {"RR A", 2, 8, RRAc}, // 0x01


	{}, {}, {}, {}, {}, {}, {}, {"SLA A", 2, 8, SLAA},
	{}, {}, {}, {}, {}, {}, {}, {}, // 0x02


	{}, {}, {}, {"SWAP E", 2, 8, SWAPE}, {}, {}, {}, {"SWAP A", 2, 8, SWAPA},
	{"SRL B", 2, 8, SRLB}, {}, {}, {}, {}, {}, {}, {"SRL A", 2, 8, SRLA}, // 0x03


	{"BIT 0,B", 2, 8, BIT0B}, {"BIT 0,C", 2, 8, BIT0C}, {}, {}, {}, {}, {"BIT 0,(HL)", 2, 16, BIT0HL}, {},
	{"BIT 1,B", 2, 8, BIT1B}, {}, {}, {}, {}, {}, {}, {"BIT 1,A", 2, 8, BIT1A}, // 0x04


	{"BIT 2,B", 2, 8, BIT2B}, {}, {}, {}, {}, {}, {}, {"BIT 2,A", 2, 8, BIT2A},
	{"BIT 3,B", 2, 8, BIT3B}, {}, {}, {}, {}, {}, {}, {"BIT 3,A", 2, 8, BIT3A}, // 0x05


	{"BIT 4,B", 2, 8, BIT4B}, {"BIT 4,C", 2, 8, BIT4C}, {}, {}, {}, {}, {}, {},
	{"BIT 5,B", 2, 8, BIT5B}, {"BIT 5,C", 2, 8, BIT5C}, {}, {}, {}, {}, {}, {"BIT 5,A", 2, 8, BIT5A}, // 0x06


											{"BIT 6,B", 2, 8, BIT6B}, {}, {}, {}, {}, {}, {}, {"BIT 6,A", 2, 8, BIT6A},
											{"BIT 7,B", 2, 8, BIT7B}, {}, {}, {}, {"BIT 7,H", 2, 8, BIT7H}, {}, {"BIT 7,(HL)", 2, 16, BIT7HL}, {"BIT 7,A", 2, 8, BIT7A}, //0x07


											{}, {}, {}, {}, {}, {}, {"RES 0,(HL)", 2, 16, RES0HL}, {"RES 0,A", 2, 8, RES0A},
											{}, {}, {}, {}, {}, {}, {}, {}, // 0x08


											{}, {}, {}, {}, {}, {}, {}, {},
											{}, {}, {}, {}, {}, {}, {}, {}, // 0x09


											{}, {}, {}, {}, {}, {}, {}, {},
											{}, {}, {}, {}, {}, {}, {}, {}, // 0xa


											{}, {}, {}, {}, {}, {}, {}, {},
											{}, {}, {}, {}, {}, {}, {"RES 7,(HL)", 2, 16, RES7HL}, {}, // 0xb


											{}, {}, {}, {}, {}, {}, {}, {},
											{}, {}, {}, {"SET 1,E", 2, 8, SET1E}, {}, {}, {}, {}, // 0xc


											{}, {}, {}, {}, {}, {}, {}, {},
											{}, {}, {}, {}, {}, {}, {}, {}, // 0xd


											{}, {}, {}, {}, {}, {}, {}, {},
											{}, {}, {}, {}, {}, {}, {}, {}, // 0xe


											{}, {}, {}, {}, {}, {}, {}, {},
											{}, {}, {}, {}, {}, {}, {"SET 7,HL", 2, 16, SET7HL}, {}}; // 0xf

CPURegisters registers;
unsigned short breakpoint = 0xffff;

// function to emulate a cpu step
// programs runs to scroll register, implement full background drawing next

// have completed test roms 04, 06
unsigned char cpu(){
	int debugBool = 0;

	unsigned char opcode = readByte(registers.pc);
	CPUInstruction instruction = instructions[(unsigned int)opcode];

	if(registers.pc == breakpoint){
		printf("coords are: %x\n", getLCDCYCoord());
		printf("Before instruction execution:\n");
		debug(&registers, instruction);
		debugBool = 1;
	}

	//debug(&registers, instruction);

	instruction.execute(&registers);

	if(debugBool){
		printf("After instruction execution:\n");
		debug(&registers, instruction);



	}


	registers.pc += instruction.operandLen;	
	registers.clock += instruction.cycleLen;

	if(debugBool){
		printf("Next instruction at: %x\n", registers.pc);
		printf("\n");
		exit(1);
		
		
	}

	//printf("pc after instruction: %x\n", registers.pc);
	//printf("%x: %x\n", 0xfffd, readByte(0xfffd));
	//printf("%x: %x\n", 0xfffc, readByte(0xfffc));

	return instruction.cycleLen;
	
}


// deprecated for now
int getCartLen(FILE *cart){
	int cartLen;

	fseek(cart, 0, SEEK_END);          
	cartLen = ftell(cart);             
	rewind(cart); 

	return cartLen;
}

void setBreakpoint(unsigned short thisBreakpoint){
	breakpoint = thisBreakpoint;
}

// power up sequence as in gameboy docs, extract the nintendo logo and then initialize registers to default values


// initialize registers to default values
// eventuall replace this so gameboy bootcode will do this for us
void initRegisters(CPURegisters *registers){
	registers->a = 0x11;//0x01;
	registers->f = 0x80;//0x0B;
	registers->b = 0x00;//0x00;
	registers->c = 0x00;//0x13;
	registers->d = 0xff;//0x0;
	registers->e = 0x56;//0xd8;
	registers->h = 0x00;//0x01;
	registers->l = 0x0d;//0x4d;

	registers->bc = 0x0000;//0x0013;
	registers->de = 0xff56;//0x00d8;
	registers->hl = 0x000d;//0x014d;

	registers->pc = 0x100;
	registers->sp = 0xfffe;

	registers->clock = 0;
}

void initPc(){
	registers.pc = 0x0000;
}

// check if pc is less than condition
int checkPc(unsigned int cond){
	if(registers.pc < cond){
		return 1;
	}else{
		return 0;
	}
}

unsigned short getPc(){
	return registers.pc;
}

void setPc(unsigned short value){
	registers.pc = value;
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

unsigned short readShortFromStack(CPURegisters *registers){
	unsigned short value = readShort(registers->sp);
	registers->sp += 2;

	return value;
}

void writeShortToStack(unsigned short value){
	registers.sp -= 2;
	writeShort(registers.sp, value);
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
	return (registers->f & 0x80) >> 7;
}

void setSubFlag(CPURegisters *registers){
	registers->f |= 0x40;
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
	return (registers->f & 0x10) >> 4;
}

void setHalfCarryFlag(CPURegisters *registers){
	registers->f |= 0x20;
}

void resetHalfCarryFlag(CPURegisters *registers){
	registers->f &= 0xDF;
}

unsigned int getHalfCarryFlag(CPURegisters *registers){
	return (registers->f & 0x20) >> 5;
}

void checkHalfCarryFlag8B(CPURegisters * registers, unsigned int x, unsigned int y){
	if((((x & 0xf) + (y & 0xf)) & 0x10) == 0x10){
		setHalfCarryFlag(registers);
	}else{
		resetHalfCarryFlag(registers);
	}
}

//consider combining sub carries into one function, can probably just make y signed, but check
void checkHalfCarryFlag8BSub(CPURegisters * registers, unsigned int x, unsigned int y){
	if((((x & 0xf) - (y & 0xf)) & 0x10) == 0x10){
		setHalfCarryFlag(registers);
	}else{
		resetHalfCarryFlag(registers);
	}
}

void checkCarry8B(CPURegisters *registers, unsigned int x, unsigned int y){
	if(x + y > 0xff){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}
}

void checkCarry8BSub(CPURegisters *registers, unsigned int x, unsigned int y){
	if(x - y > 0xff){
		setCarryFlag(registers);
		//printf("set\n");
	}else{
		resetCarryFlag(registers);
		//printf("reset\n");
	}
}

void checkHalfCarryFlag16B(CPURegisters *registers, unsigned int x, unsigned int y){
	if((((x & 0x0fff) + (y & 0x0fff)) & 0x1000) == 0x1000){
		setHalfCarryFlag(registers);
	}else{
		resetHalfCarryFlag(registers);
	}
}

void checkCarry16B(CPURegisters *registers, unsigned int x, unsigned int y){
	if(x + y > 0xffff){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
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

void NOP(CPURegisters *registers){
	return;
}


void LDBCn(CPURegisters *registers){
	registers->c = readByte(registers->pc + 1);
	registers->b = readByte(registers->pc + 2);
	alignRegistersBC(registers);
}

void LDBCA(CPURegisters *registers){
	writeByte(registers->bc, registers->a);
}

void INCBC(CPURegisters *registers){
	registers->bc++;
	alignRegistersBCByte(registers);
}

void INCB(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->b, 1);
	registers->b++;
	if(registers->b == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersBC(registers);
}

void DECB(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->b, 1);
	registers->b--;

	if(registers->b == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersBC(registers);
}

void LDB(CPURegisters *registers){
	registers->b = readByte(registers->pc + 1);
	alignRegistersBC(registers);
}

void RLCA(CPURegisters *registers){
	if((registers->a >> 7) == 1){
		setCarryFlag(registers);
		registers->a <<= 1;
		registers->a++;
	}else{
		registers->a <<= 1;
		resetCarryFlag(registers);
	}
	
	resetZeroFlag(registers);
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void LDnnSP(CPURegisters *registers){
	// must write stack pointer to address, research it a bit before implementing, i assume you write one byte at address and the next byte at address + 1
	unsigned short memAddress = readByte(registers->pc + 1) | (readByte(registers->pc + 2) << 8);
	writeByte(memAddress, (unsigned char)(registers->sp & 0x00ff));
	writeByte(memAddress + 1, (unsigned char)(registers->sp & 0xff00));
}

void ADDHLBC(CPURegisters *registers){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->bc);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->bc);
	registers->hl += registers->bc;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDABC(CPURegisters *registers){
	registers->a = readByte(registers->bc);
}

void DECBC(CPURegisters *registers){
	registers->bc--;
	alignRegistersBCByte(registers);
}

void INCC(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->c, 1);
	registers->c++;
	if(registers->c == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersBC(registers);
}

void DECC(CPURegisters *registers){
	//need to check half carry flag
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->c, 1);
	registers->c--;

	if(registers->c == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersBC(registers);
}

void LDC(CPURegisters *registers){
	registers->c = readByte(registers->pc + 1);
	alignRegistersBC(registers);
}

void RRCA(CPURegisters *registers){
	unsigned char carry = registers->a & 0x01;
	if(carry == 1){
		setCarryFlag(registers);
		registers->a >>= 1;
		registers->a |= 0x80;
	}else{
		registers->a >>= 1;
		resetCarryFlag(registers);
	}
	
	
	resetZeroFlag(registers);
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void STOP(CPURegisters *registers){
	return;
}

void LDDEn(CPURegisters *registers){
	registers->e = readByte(registers->pc + 1);
	registers->d = readByte(registers->pc + 2);
	alignRegistersDE(registers);
}

void LDDEA(CPURegisters *registers){
	writeByte(registers->de, registers->a);
}

void INCDE(CPURegisters *registers){
	registers->de++;
	alignRegistersDEByte(registers);
}

void INCD(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->d, 1);
	registers->d++;
	if(registers->d == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersDE(registers);
}

void DECD(CPURegisters *registers){
	// need to check half carry flag on subtract
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->d, 1);
	registers->d--;

	if(registers->d == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersDE(registers);
}

void LDD(CPURegisters *registers){
	registers->d = readByte(registers->pc + 1);
	alignRegistersDE(registers);
}

void RLA(CPURegisters *registers){
	unsigned int carry = getCarryFlag(registers);

	if(registers->a & 0x80){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->a <<= 1;
	registers->a += carry;

	
	resetZeroFlag(registers);
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void JRN(CPURegisters *registers){
	registers->pc += (signed char)readByte(registers->pc + 1);
}

void ADDHLDE(CPURegisters *registers){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->de);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->de);
	registers->hl += registers->de;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDADE(CPURegisters *registers){
	registers->a = readByte(registers->de);
}

void DECDE(CPURegisters *registers){
	registers->de--;
	alignRegistersDEByte(registers);
}

void INCE(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->e, 1);
	registers->e++;
	if(registers->e == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersDE(registers);
}

void DECE(CPURegisters *registers){
	// need to check half carry flag on subtract
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->e, 1);
	registers->e--;

	if(registers->e == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersDE(registers);
}

void LDE(CPURegisters *registers){
	registers->e = readByte(registers->pc + 1);
	alignRegistersDE(registers);
}

void RRA(CPURegisters *registers){
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

	resetZeroFlag(registers);
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void JRNZn(CPURegisters *registers){
	if(!getZeroFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1);
	}
}

void LDHLn(CPURegisters *registers){
	registers->l = readByte(registers->pc + 1);
	registers->h = readByte(registers->pc + 2);
	alignRegistersHL(registers);

}

void LDIHLA(CPURegisters *registers){
	writeByte(registers->hl++, registers->a);
	alignRegistersHLByte(registers);
}

void INCHL(CPURegisters *registers){
	registers->hl++;
	alignRegistersHLByte(registers);
}

void INCH(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->h, 1);
	registers->h++;
	if(registers->h == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersHL(registers);
}

void DECH(CPURegisters *registers){
	// need to check half carry flag on subtract
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->h, 1);

	registers->h--;
	if(registers->h == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersHL(registers);
}

void LDH(CPURegisters *registers){
	registers->h = readByte(registers->pc + 1);
	alignRegistersHL(registers);
}

void DAA(CPURegisters *registers){
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
	}else{
		resetZeroFlag(registers);
	}
	resetHalfCarryFlag(registers);

	registers->a = daa;
}

void JRZn(CPURegisters *registers){
	if(getZeroFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1);
	}
}

void ADDHLHL(CPURegisters *registers){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->hl);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->hl);
	registers->hl += registers->hl;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDIAHL(CPURegisters *registers){
	registers->a = readByte(registers->hl++);
	alignRegistersHLByte(registers);
}

void DECHL(CPURegisters *registers){
	registers->hl--;
	alignRegistersHLByte(registers);
}

void INCL(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->l, 1);
	registers->l++;
	if(registers->l == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	alignRegistersHL(registers);
}

void DECL(CPURegisters *registers){
	// need to check half carry flag on subtract
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->l, 1);
	registers->l--;
	
	if(registers->l == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
	alignRegistersHL(registers);
}

void LDL(CPURegisters *registers){
	registers->l = readByte(registers->pc + 1);
	alignRegistersHL(registers);
}

void CPL(CPURegisters *registers){
	registers->a = ~registers->a;
	setSubFlag(registers);
	setHalfCarryFlag(registers);
}

void JRNCn(CPURegisters *registers){
	if(!getCarryFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1);
	}
}

void LDSP(CPURegisters *registers){
	registers->sp = readByte(registers->pc + 2);
	registers->sp <<= 8;
	registers->sp |= readByte(registers->pc + 1);
}

void LDDHLA(CPURegisters *registers){
	writeByte(registers->hl--, registers->a);
	alignRegistersHLByte(registers);
}

void INCSP(CPURegisters *registers){
	registers->sp++;
}

void INCHLa(CPURegisters *registers){
	unsigned char x = readByte(registers->hl);
	checkHalfCarryFlag8B(registers, (unsigned int)x, 1);
	x += 1;
	if(x == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetSubFlag(registers);
	writeByte(registers->hl, (unsigned char)x);
}

void DECHLa(CPURegisters *registers){
	unsigned char x = readByte(registers->hl);// check if half carry flag is set in subtraction
	
	x -= 1;
	if(x == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setSubFlag(registers);
	writeByte(registers->hl, (unsigned char)x);
}

void LDHLaddrs(CPURegisters *registers){
	writeByte(registers->hl, readByte(registers->pc + 1));
}

void SCF(CPURegisters *registers){
	setCarryFlag(registers);
	resetHalfCarryFlag(registers);
	resetSubFlag(registers);
}

void JRCn(CPURegisters *registers){
	if(getCarryFlag(registers)){
		registers->pc += (signed char)readByte(registers->pc + 1);
	}
}

void ADDHLSP(CPURegisters *registers){
	checkHalfCarryFlag16B(registers, (unsigned int)registers->hl, (unsigned int)registers->sp);
	checkCarry16B(registers, (unsigned int)registers->hl, (unsigned int)registers->sp);
	registers->hl += registers->sp;
	alignRegistersHLByte(registers);
	resetSubFlag(registers);
}

void LDDAHL(CPURegisters *registers){
	registers->a = readByte(registers->hl--);
}

void DECSP(CPURegisters *registers){
	registers->sp--;
}

void INCA(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, 1);
	registers->a++;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void DECA(CPURegisters *registers){
	//need to check half carry flag
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, 1);
	registers->a--;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void LDA(CPURegisters *registers){
	registers->a = readByte(registers->pc + 1);
}

void CCF(CPURegisters *registers){
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
	if(getCarryFlag(registers)){
		resetCarryFlag(registers);
	}else{
		setCarryFlag(registers);
	}
}

void LDBB(CPURegisters *registers){
	registers->b = registers->b;
	alignRegistersBC(registers);
}

void LDBC(CPURegisters *registers){
	registers->b = registers->c;
	alignRegistersBC(registers);
}

void LDBD(CPURegisters *registers){
	registers->b = registers->d;
	alignRegistersBC(registers);
}

void LDBE(CPURegisters *registers){
	registers->b = registers->e;
	alignRegistersBC(registers);
}

void LDBH(CPURegisters *registers){
	registers->b = registers->h;
	alignRegistersBC(registers);
}

void LDBL(CPURegisters *registers){
	registers->b = registers->l;
	alignRegistersBC(registers);
}

void LDBHL(CPURegisters *registers){
	registers->b = readByte(registers->hl);
	alignRegistersBC(registers);
}

void LDBA(CPURegisters *registers){
	registers->b = registers->a;
	alignRegistersBC(registers);
}

void LDCB(CPURegisters *registers){
	registers->c = registers->b;
	alignRegistersBC(registers);
}

void LDCC(CPURegisters *registers){
	registers->c = registers->c;
	alignRegistersBC(registers);
}

void LDCD(CPURegisters *registers){
	registers->c = registers->d;
	alignRegistersBC(registers);
}

void LDCE(CPURegisters *registers){
	registers->c = registers->e;
	alignRegistersBC(registers);
}

void LDCH(CPURegisters *registers){
	registers->c = registers->h;
	alignRegistersBC(registers);
}

void LDCL(CPURegisters *registers){
	registers->c = registers->l;
	alignRegistersBC(registers);
}

void LDCHL(CPURegisters *registers){
	registers->c = readByte(registers->hl);
	alignRegistersBC(registers);
}

void LDCA(CPURegisters *registers){
	registers->c = registers->a;
	alignRegistersBC(registers);
}

void LDDB(CPURegisters *registers){
	registers->d = registers->b;
	alignRegistersDE(registers);
}

void LDDC(CPURegisters *registers){
	registers->d = registers->c;
	alignRegistersDE(registers);
}

void LDDD(CPURegisters *registers){
	registers->d = registers->d;
	alignRegistersDE(registers);
}

void LDDE(CPURegisters *registers){
	registers->d = registers->e;
	alignRegistersDE(registers);
}

void LDDH(CPURegisters *registers){
	registers->d = registers->h;
	alignRegistersDE(registers);
}

void LDDL(CPURegisters *registers){
	registers->d = registers->l;
	alignRegistersDE(registers);
}

void LDDHL(CPURegisters *registers){
	registers->d = readByte(registers->hl);
	alignRegistersDE(registers);
}

void LDDA(CPURegisters *registers){
	registers->d = registers->a;
	alignRegistersDE(registers);
}

void LDEB(CPURegisters *registers){
	registers->e = registers->b;
	alignRegistersDE(registers);
}

void LDEC(CPURegisters *registers){
	registers->e = registers->c;
	alignRegistersDE(registers);
}

void LDED(CPURegisters *registers){
	registers->e = registers->d;
	alignRegistersDE(registers);
}

void LDEE(CPURegisters *registers){
	registers->e = registers->e;
	alignRegistersDE(registers);
}

void LDEH(CPURegisters *registers){
	registers->e = registers->h;
	alignRegistersDE(registers);
}

void LDEL(CPURegisters *registers){
	registers->e = registers->l;
	alignRegistersDE(registers);
}

void LDEHL(CPURegisters *registers){
	registers->e = readByte(registers->hl);
	alignRegistersDE(registers);
}

void LDEA(CPURegisters *registers){
	registers->e = registers->a;
	alignRegistersDE(registers);
}

void LDHB(CPURegisters *registers){
	registers->h = registers->b;
	alignRegistersHL(registers);
}

void LDHC(CPURegisters *registers){
	registers->h = registers->c;
	alignRegistersHL(registers);
}

void LDHD(CPURegisters *registers){
	registers->h = registers->d;
	alignRegistersHL(registers);
}

void LDHE(CPURegisters *registers){
	registers->h = registers->e;
	alignRegistersHL(registers);
}

void LDHH(CPURegisters *registers){
	registers->h = registers->h;
	alignRegistersHL(registers);
}

void LDHL(CPURegisters *registers){
	registers->h = registers->l;
	alignRegistersHL(registers);
}

void LDHHL(CPURegisters *registers){
	registers->h = readByte(registers->hl);
	alignRegistersHL(registers);
}

void LDHA(CPURegisters *registers){
	registers->h = registers->a;
	alignRegistersHL(registers);
}

void LDLB(CPURegisters *registers){
	registers->l = registers->b;
	alignRegistersHL(registers);
}

void LDLC(CPURegisters *registers){
	registers->l = registers->c;
	alignRegistersHL(registers);
}

void LDLD(CPURegisters *registers){
	registers->l = registers->d;
	alignRegistersHL(registers);
}

void LDLE(CPURegisters *registers){
	registers->l = registers->e;
	alignRegistersHL(registers);
}

void LDLH(CPURegisters *registers){
	registers->l = registers->h;
	alignRegistersHL(registers);
}

void LDLL(CPURegisters *registers){
	registers->l = registers->l;
	alignRegistersHL(registers);
}

void LDLHL(CPURegisters *registers){
	registers->l = readByte(registers->hl);
	alignRegistersHL(registers);
}

void LDLA(CPURegisters *registers){
	registers->l = registers->a;
	alignRegistersHL(registers);
}

void LDHLB(CPURegisters *registers){
	writeByte(registers->hl, registers->b);
}

void LDHLC(CPURegisters *registers){
	writeByte(registers->hl, registers->c);
}

void LDHLD(CPURegisters *registers){
	writeByte(registers->hl, registers->d);
}

void LDHLE(CPURegisters *registers){
	writeByte(registers->hl, registers->e);
}

void LDHLH(CPURegisters *registers){
	writeByte(registers->hl, registers->h);
}

void LDHLL(CPURegisters *registers){
	writeByte(registers->hl, registers->l);
}

void LDHLA(CPURegisters *registers){
	writeByte(registers->hl, registers->a);
}

void LDAB(CPURegisters *registers){
	registers->a = registers->b;
}

void LDAC(CPURegisters *registers){
	registers->a = registers->c;
}

void LDAD(CPURegisters *registers){
	registers->a = registers->d;
}

void LDAE(CPURegisters *registers){
	registers->a = registers->e;
}

void LDAH(CPURegisters *registers){
	registers->a = registers->h;
}

void LDAL(CPURegisters *registers){
	registers->a = registers->l;
}

void LDAHL(CPURegisters *registers){
	registers->a = readByte(registers->hl);
}

void LDAA(CPURegisters *registers){
	registers->a = registers->a;
}

void ADDAB(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	registers->a += registers->b;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADDAC(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	registers->a += registers->c;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADDAD(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	registers->a += registers->d;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADDAE(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	registers->a += registers->e;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADDAH(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	registers->a += registers->h;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADDAL(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	registers->a += registers->l;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADDAHL(CPURegisters *registers){
	unsigned int x = readByte(registers->hl);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, x);
	checkCarry8B(registers, (unsigned int)registers->a, x);
	registers->a += x;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADDAA(CPURegisters *registers){
	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	registers->a += registers->a;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

// add carry doesnt test for the addition of the carry, we should check each operation and generate a bitmask based on what was reset and what wasnt? ie if carry is set in the operand adition, 
//but not in the carry addition, we dont count that as resetting the carry, however currently once implemented it would
// simple ++ check carries algorithm, if 0x0f == 0x0f, there is a half carry, if 0xf0 == 0xf0, there is a carry (IMPLEMENTED MORE TESTING?!?!?!)

void ADCAB(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	registers->a += registers->b;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADCAC(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	registers->a += registers->c;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADCAD(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	registers->a += registers->d;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADCAE(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	registers->a += registers->e;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADCAH(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	registers->a += registers->h;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADCAL(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	registers->a += registers->l;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADCAHL(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);
	unsigned char x = readByte(registers->hl);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)x);
	registers->a += x;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void ADCAA(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	registers->a += registers->a;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void SUBB(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	registers->a -= registers->b;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SUBC(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	registers->a -= registers->c;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SUBD(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	registers->a -= registers->d;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SUBE(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	registers->a -= registers->e;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SUBH(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	registers->a -= registers->h;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SUBL(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	registers->a -= registers->l;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SUBHL(CPURegisters *registers){
	unsigned int x = readByte(registers->hl);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	registers->a -= x;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SUBA(CPURegisters *registers){
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	registers->a -= registers->a;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAB(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	registers->a -= registers->b;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAC(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	registers->a -= registers->c;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAD(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	registers->a -= registers->d;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAE(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	registers->a -= registers->e;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAH(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	registers->a -= registers->h;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAL(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	registers->a -= registers->l;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAHL(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);
	unsigned char x = readByte(registers->hl);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	registers->a -= x;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void SBCAA(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	registers->a -= registers->a;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void ANDB(CPURegisters *registers){
	registers->a &= registers->b;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ANDC(CPURegisters *registers){
	registers->a &= registers->c;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ANDD(CPURegisters *registers){
	registers->a &= registers->d;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ANDE(CPURegisters *registers){
	registers->a &= registers->e;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ANDH(CPURegisters *registers){
	registers->a &= registers->h;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ANDL(CPURegisters *registers){
	registers->a &= registers->l;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ANDHL(CPURegisters *registers){
	registers->a &= readByte(registers->hl);

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ANDA(CPURegisters *registers){
	registers->a &= registers->a;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORB(CPURegisters *registers){
	registers->a ^= registers->b;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORC(CPURegisters *registers){
	registers->a ^= registers->c;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORD(CPURegisters *registers){
	registers->a ^= registers->d;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORE(CPURegisters *registers){
	registers->a ^= registers->e;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORH(CPURegisters *registers){
	registers->a ^= registers->h;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORL(CPURegisters *registers){
	registers->a ^= registers->l;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORHL(CPURegisters *registers){
	registers->a ^= readByte(registers->hl);

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void XORA(CPURegisters *registers){
	registers->a ^= registers->a;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void ORB(CPURegisters *registers){
	registers->a |= registers->b;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void ORC(CPURegisters *registers){
	registers->a |= registers->c;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void ORD(CPURegisters *registers){
	registers->a |= registers->d;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void ORE(CPURegisters *registers){
	registers->a |= registers->e;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void ORH(CPURegisters *registers){
	registers->a |= registers->h;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void ORL(CPURegisters *registers){
	registers->a |= registers->l;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void ORHL(CPURegisters *registers){
	registers->a |= readByte(registers->hl);

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void ORA(CPURegisters *registers){
	registers->a |= registers->a;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);

}

void CPB(CPURegisters *registers){
	if(registers->a == registers->b){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->b);
	setSubFlag(registers);
}

void CPC(CPURegisters *registers){
	if(registers->a == registers->c){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->c);
	setSubFlag(registers);
}

void CPD(CPURegisters *registers){
	if(registers->a == registers->d){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->d);
	setSubFlag(registers);
}

void CPE(CPURegisters *registers){
	if(registers->a == registers->e){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->e);
	setSubFlag(registers);
}

void CPH(CPURegisters *registers){
	if(registers->a == registers->h){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->h);
	setSubFlag(registers);
}

void CPLs(CPURegisters *registers){
	if(registers->a == registers->l){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->l);
	setSubFlag(registers);
}

void CPHL(CPURegisters *registers){
	unsigned char x = readByte(registers->hl);

	if(registers->a == x){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	setSubFlag(registers);
}

void CPA(CPURegisters *registers){
	printf("in cpa\n");
	setZeroFlag(registers);
	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)registers->a);
	setSubFlag(registers);
}

void RETNZ(CPURegisters *registers){
	if(!getZeroFlag(registers)){
		RET(registers);
	}
}

void POPBC(CPURegisters *registers){
	registers->bc = readShortFromStack(registers);
	alignRegistersBCByte(registers);
}

void JPNZ(CPURegisters *registers){
	if(!getZeroFlag(registers)){
		registers->pc = readShort(registers->pc + 1);
		registers->pc -= 3;
	}
}

void JP(CPURegisters *registers){
	registers->pc = readShort(registers->pc + 1);
	registers->pc -= 3;
}

void CALLNZ(CPURegisters *registers){
	if(!getZeroFlag(registers)){
		CALL(registers);
	}
}

void PUSHBC(CPURegisters *registers){
	writeShortToStack(registers->bc);
}

void ADDAn(CPURegisters *registers){
	unsigned char x = readByte(registers->pc + 1);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)x);
	registers->a += x;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void RST00(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0000;
}

void RETZ(CPURegisters *registers){
	if(getZeroFlag(registers)){
		RET(registers);
	}
}

void RET(CPURegisters *registers){
	registers->pc = readShortFromStack(registers);
	registers->pc -= 1;
}

void JPZ(CPURegisters *registers){
	if(getZeroFlag(registers)){
		registers->pc = readShort(registers->pc + 1);
		registers->pc -= 3;
	}
}

// implement shift instructions here
void PREFIXCB(CPURegisters *registers){
	unsigned char cbOpcode = readByte(registers->pc + 1);
	//printf("missing instruction: %x\n", cbOpcode);
	CPUInstruction cbInstruction = cbInstructions[(unsigned int)cbOpcode];

	//printf("Prefix instruction: %s\n", cbInstruction.instruction);

	cbInstruction.execute(registers);
}

void CALLZ(CPURegisters *registers){
	if(getZeroFlag(registers)){
		CALL(registers);
	}
}

void CALL(CPURegisters *registers){
	writeShortToStack(registers->pc + 3);

	registers->pc = readShort(registers->pc + 1);
	registers->pc -= 3;
}

void ADCAn(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);
	unsigned char x = readByte(registers->pc + 1);

	checkHalfCarryFlag8B(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8B(registers, (unsigned int)registers->a, (unsigned int)x);
	registers->a += x;
	if(carry){
		if((registers->a & 0x0f) == 0x0f){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0xff){
			setCarryFlag(registers);
		}
		registers->a++;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
}

void RST08(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0008;
	registers->pc -= 1;
}

void RETNC(CPURegisters *registers){
	if(!getCarryFlag(registers)){
		RET(registers);
	}
}

void POPDE(CPURegisters *registers){
	registers->de = readShortFromStack(registers);
	alignRegistersDEByte(registers);
}

void JPNC(CPURegisters *registers){
	if(!getCarryFlag(registers)){
		registers->pc = readShort(registers->pc + 1);
		registers->pc -= 3;
	}
}

void CALLNC(CPURegisters *registers){
	if(!getCarryFlag(registers)){
		CALL(registers);
	}
}

void PUSHDE(CPURegisters *registers){
	writeShortToStack(registers->de);
}

void SUBn(CPURegisters *registers){
	unsigned int x = readByte(registers->pc + 1);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	registers->a -= x;
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void RST10(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0010;
	registers->pc -= 1;
}

void RETC(CPURegisters *registers){
	if(getCarryFlag(registers)){
		RET(registers);
	}
}

// something to do with interrupts, will fully implement later
void RETI(CPURegisters *registers){
	setIME();
	RET(registers);
}

void JPC(CPURegisters *registers){
	if(getCarryFlag(registers)){
		registers->pc = readShort(registers->pc + 1);
		registers->pc -= 3;
	}
}

void CALLC(CPURegisters *registers){
	if(getCarryFlag(registers)){
		CALL(registers);
	}
}

void SBCAn(CPURegisters *registers){
	unsigned char carry = (unsigned char)getCarryFlag(registers);
	unsigned char x = readByte(registers->pc + 1);

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	registers->a -= x;
	if(carry){
		if((registers->a & 0x0f) == 0x00){
			setHalfCarryFlag(registers);
		}
		if(registers->a == 0x00){
			setCarryFlag(registers);
		}
		registers->a--;
	}
	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	setSubFlag(registers);
}

void RST18(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0018;
	registers->pc -= 1;
}

void LDHnA(CPURegisters *registers){
	writeByte(0xff00 + readByte(registers->pc + 1), registers->a);
}

void POPHL(CPURegisters *registers){
	registers->hl = readShortFromStack(registers);
	alignRegistersHLByte(registers);
}

void LDCAaddr(CPURegisters *registers){
	writeByte(0xff00 + registers->c, registers->a);
}

void PUSHHL(CPURegisters *registers){
	writeShortToStack(registers->hl);
}

void ANDn(CPURegisters *registers){
	registers->a &= readByte(registers->pc + 1);

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	setHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void RST20(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0020;
	registers->pc -= 1;
}

void ADDSPn(CPURegisters *registers){
	unsigned char x = readByte(registers->pc + 1);

	checkHalfCarryFlag16B(registers, (unsigned int)registers->sp, (unsigned int)x);
	checkCarry16B(registers, (unsigned int)registers->sp, (unsigned int)x);
	registers->sp += x;
	
	resetSubFlag(registers);
	resetZeroFlag(registers);
}

void JPHL(CPURegisters *registers){
	registers->pc = registers->hl;
	registers->pc -= 1;
}

void LDnnA(CPURegisters *registers){
	writeByte(readShort(registers->pc + 1), registers->a);
}

void XORn(CPURegisters *registers){
	registers->a ^= readByte(registers->pc + 1);

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void RST28(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0028;
	registers->pc -= 1;
}

void LDHAn(CPURegisters *registers){
	registers->a = readByte(0xff00 + readByte(registers->pc + 1));
}

void POPAF(CPURegisters *registers){
	unsigned short x = readShortFromStack(registers);
	
	registers->a = (x & 0xff00) >> 8;
	registers->f = x & 0x00ff;
}

void LDACaddr(CPURegisters *registers){
	registers->a = readByte(0xff00 + registers->c);
}

// something to do with interrupts, implement later
void DI(CPURegisters *registers){
	resetIME();
}

void PUSHAF(CPURegisters *registers){
	unsigned short x = registers->a << 8;
	x |= registers->f;

	writeShortToStack(x);
}

void ORn(CPURegisters *registers){
	registers->a |= readByte(registers->pc + 1);

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
	resetSubFlag(registers);
}

void RST30(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0030;
	registers->pc -= 1;
}

void LDHLSPn(CPURegisters *registers){
	unsigned char x = readByte(registers->pc + 1);

	checkHalfCarryFlag16B(registers, (unsigned int)registers->sp, (unsigned int)x);
	checkCarry16B(registers, (unsigned int)registers->sp, (unsigned int)x);
	registers->hl = registers->sp + x;
	alignRegistersHLByte(registers);
	
	resetSubFlag(registers);
	resetZeroFlag(registers);
}

void LDSPHL(CPURegisters *registers){
	registers->sp = registers->hl;
}

void LDAnn(CPURegisters *registers){
	registers->a = readByte(readShort(registers->pc + 1));
}

// interrupts, implement later
void EI(CPURegisters *registers){
	setIME();
}

void CPn(CPURegisters *registers){
	unsigned char x = readByte(registers->pc + 1);

	if(registers->a == x){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	checkHalfCarryFlag8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	checkCarry8BSub(registers, (unsigned int)registers->a, (unsigned int)x);
	setSubFlag(registers);
}

void RST38(CPURegisters *registers){
	writeShortToStack(registers->pc + 1);
	registers->pc = 0x0038;
	registers->pc -= 1;
}

void RLCB(CPURegisters *registers){
	if((registers->b >> 7) == 1){
		setCarryFlag(registers);
		registers->b <<= 1;
		registers->b++;
	}else{
		registers->b <<= 1;
		resetCarryFlag(registers);
	}
	alignRegistersBC(registers);

	if(registers->b == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RLCC(CPURegisters *registers){
	if((registers->c >> 7) == 1){
		setCarryFlag(registers);
		registers->c <<= 1;
		registers->c++;
	}else{
		registers->c <<= 1;
		resetCarryFlag(registers);
	}
	alignRegistersBC(registers);

	if(registers->c == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RLCD(CPURegisters *registers){
	if((registers->d >> 7) == 1){
		setCarryFlag(registers);
		registers->d <<= 1;
		registers->d++;
	}else{
		registers->d <<= 1;
		resetCarryFlag(registers);
	}
	alignRegistersDE(registers);

	if(registers->d == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RLCE(CPURegisters *registers){
	if((registers->e >> 7) == 1){
		setCarryFlag(registers);
		registers->e <<= 1;
		registers->e++;
	}else{
		registers->e <<= 1;
		resetCarryFlag(registers);
	}
	alignRegistersDE(registers);

	if(registers->e == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RLCH(CPURegisters *registers){
	if((registers->h >> 7) == 1){
		setCarryFlag(registers);
		registers->h <<= 1;
		registers->h++;
	}else{
		registers->h <<= 1;
		resetCarryFlag(registers);
	}
	alignRegistersHL(registers);

	if(registers->h == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RLCL(CPURegisters *registers){
	if((registers->l >> 7) == 1){
		setCarryFlag(registers);
		registers->l <<= 1;
		registers->l++;
	}else{
		registers->l <<= 1;
		resetCarryFlag(registers);
	}
	alignRegistersHL(registers);

	if(registers->l == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RLCHL(CPURegisters *registers){
	unsigned char byte = readByte(registers->hl);

	if((byte >> 7) == 1){
		setCarryFlag(registers);
		byte <<= 1;
		byte++;
	}else{
		byte <<= 1;
		resetCarryFlag(registers);
	}

	if(byte == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	writeByte(registers->hl, byte);
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RLCAC(CPURegisters *registers){
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
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RRCB(CPURegisters *registers){
	if(registers->b & 0x01){
		setCarryFlag(registers);
		registers->b >>= 1;
		registers->b |= 0x80;
	}else{
		registers->b >>= 1;
		resetCarryFlag(registers);
	}

	if(registers->b == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RRCC(CPURegisters *registers){
	if(registers->c & 0x01){
		setCarryFlag(registers);
		registers->c >>= 1;
		registers->c |= 0x80;
	}else{
		registers->c >>= 1;
		resetCarryFlag(registers);
	}

	if(registers->c == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}
	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void z80BIT(CPURegisters *registers, unsigned char *theRegister, unsigned char mask){
	if(*theRegister & mask){
		resetZeroFlag(registers);
	}else{
		setZeroFlag(registers);
	}

	resetSubFlag(registers);
	setHalfCarryFlag(registers);
}

void BIT0B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x01);
}

void BIT0C(CPURegisters *registers){
	z80BIT(registers, &(registers->c), 0x01);
}

void BIT0HL(CPURegisters *registers){
	if(readByte(registers->hl) & 0x01){
		resetZeroFlag(registers);
	}else{
		setZeroFlag(registers);
	}

	resetSubFlag(registers);
	setHalfCarryFlag(registers);
}

void BIT1B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x02);
}

void BIT4B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x10);
}

void BIT2B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x04);
}

void BIT2A(CPURegisters *registers){
	z80BIT(registers, &(registers->a), 0x04);
}

void BIT3B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x08);
}

void BIT3A(CPURegisters *registers){
	z80BIT(registers, &(registers->a), 0x08);
}

void BIT4C(CPURegisters *registers){
	z80BIT(registers, &(registers->c), 0x10);
}

void BIT5B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x20);
}

void BIT5C(CPURegisters *registers){
	z80BIT(registers, &(registers->c), 0x20);
}

void BIT5A(CPURegisters *registers){
	z80BIT(registers, &(registers->a), 0x20);
}

void BIT6B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x40);
}

void BIT6A(CPURegisters *registers){
	z80BIT(registers, &(registers->a), 0x40);
}

void BIT7B(CPURegisters *registers){
	z80BIT(registers, &(registers->b), 0x80);
}

void BIT7H(CPURegisters *registers){
	z80BIT(registers, &(registers->h), 0x80);
}

void BIT7HL(CPURegisters *registers){
	if(readByte(registers->hl) & 0x80){
		resetZeroFlag(registers);
	}else{
		setZeroFlag(registers);
	}

	resetSubFlag(registers);
	setHalfCarryFlag(registers);
}

void BIT7A(CPURegisters *registers){
	z80BIT(registers, &(registers->a), 0x80);
}

void BIT1A(CPURegisters *registers){
	z80BIT(registers, &(registers->a), 0x02);
}

void SET1E(CPURegisters *registers){
	registers->e |= 0x1;
}

void SET7HL(CPURegisters *registers){
	unsigned char byte = readByte(registers->hl);
	writeByte(registers->hl, byte | 0x80);
}

void RLC(CPURegisters *registers){
	int carry = getCarryFlag(registers);

	if((registers->c >> 7) == 1){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->c <<= 1;

	if(carry){
		registers->c++;
	}

	if(registers->b == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	alignRegistersBC(registers);

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);

}

void SRLB(CPURegisters *registers){
	if(registers->b & 0x01){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->b >>= 1;

	if(registers->b == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	alignRegistersBC(registers);

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void SRLA(CPURegisters *registers){
	if(registers->a & 0x01){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->a >>= 1;

	if(registers->b == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}


void RRC(CPURegisters *registers){
	int carry = getCarryFlag(registers);

	if(registers->c & 0x01){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->c >>= 1;

	if(carry){
		registers->c |= carry << 7;
	}

	if(registers->c == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	alignRegistersBC(registers);

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RRD(CPURegisters *registers){
	int carry = getCarryFlag(registers);

	if(registers->d & 0x01){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->d >>= 1;

	if(carry){
		registers->d |= carry << 7;
	}

	if(registers->d == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	alignRegistersDE(registers);

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RRE(CPURegisters *registers){
	int carry = getCarryFlag(registers);

	if(registers->e & 0x01){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->e >>= 1;

	if(carry){
		registers->e |= carry << 7;
	}

	if(registers->e == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	alignRegistersDE(registers);

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void SLAA(CPURegisters *registers){
	if(registers->a & 0x80){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->a <<= 1;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void RRAc(CPURegisters *registers){
	int carry = getCarryFlag(registers);

	if(registers->a & 0x01){
		setCarryFlag(registers);
	}else{
		resetCarryFlag(registers);
	}

	registers->a >>= 1;

	if(carry){
		registers->a |= carry << 7;
	}

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
}

void SWAPA(CPURegisters *registers){
	unsigned char high = (registers->a & 0xf0) >> 4;
	registers->a = (registers->a << 4) | high;

	if(registers->a == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
}

void SWAPE(CPURegisters *registers){
	unsigned char high = (registers->e & 0xf0) >> 4;
	registers->e = (registers->e << 4) | high;

	if(registers->e == 0){
		setZeroFlag(registers);
	}else{
		resetZeroFlag(registers);
	}

	resetSubFlag(registers);
	resetHalfCarryFlag(registers);
	resetCarryFlag(registers);
}

void RES0A(CPURegisters *registers){
	registers->a &= ~(0x01);
}

void RES0HL(CPURegisters *registers){
	writeByte(registers->hl, readByte(registers->hl) & ~(0x01));
}

void RES7HL(CPURegisters *registers){
	writeByte(registers->hl, readByte(registers->hl) & ~(0x80));
}