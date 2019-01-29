#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CPURegisters{
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

	unsigned short clock;
}CPURegisters;

typedef struct CPUInstruction{
	char *instruction;
	unsigned char operandLen;
	unsigned char cycleLen;
	void (*execute)(CPURegisters *registers);
}CPUInstruction;

int getCartLen(FILE *cart);
void initPc();
int checkPc(unsigned int cond);
unsigned short getPc();
void setPc(unsigned short value);
void initRegisters();
//unsigned char readByte(unsigned short address);
//void writeByte(unsigned short address, unsigned char sp);
void CPUFunction(unsigned char opcode);
unsigned char cpu();

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
void checkHalfCarryFlag8BSub(CPURegisters * registers, unsigned int x, unsigned int y);
void checkCarry8B(CPURegisters *registers, unsigned int x, unsigned int y);
void checkCarry8BSub(CPURegisters *registers, unsigned int x, unsigned int y);
void checkHalfCarryFlag16B(CPURegisters *registers, unsigned int x, unsigned int y);
void checkCarry16B(CPURegisters *registers, unsigned int x, unsigned int y);

void debug(CPURegisters *registers, CPUInstruction instruction);
void setDebug();
void setBreakpoint(unsigned short thisBreakpoint);

unsigned short readShortFromStack(CPURegisters *registers);
void writeShortToStack(unsigned short value);

// all instruction functions
void NOP(CPURegisters *registers);
void LDBCn(CPURegisters *registers);
void LDBCA(CPURegisters *registers);
void INCBC(CPURegisters *registers);
void INCB(CPURegisters *registers);
void DECB(CPURegisters *registers);
void RLCA(CPURegisters *registers);
void LDB(CPURegisters *registers);
void LDnnSP(CPURegisters *registers);
void ADDHLBC(CPURegisters *registers);
void LDABC(CPURegisters *registers);
void DECBC(CPURegisters *registers);
void INCC(CPURegisters *registers);
void DECC(CPURegisters *registers);
void LDC(CPURegisters *registers);
void RRCA(CPURegisters *registers);

void STOP(CPURegisters *registers);
void LDDEn(CPURegisters *registers);
void LDDEA(CPURegisters *registers);
void INCDE(CPURegisters *registers);
void INCD(CPURegisters *registers);
void DECD(CPURegisters *registers);
void LDD(CPURegisters *registers);
void RLA(CPURegisters *registers);
void JRN(CPURegisters *registers);
void ADDHLDE(CPURegisters *registers);
void LDADE(CPURegisters *registers);
void DECDE(CPURegisters *registers);
void INCE(CPURegisters *registers);
void DECE(CPURegisters *registers);
void RRA(CPURegisters *registers);
void LDE(CPURegisters *registers);

void JRNZn(CPURegisters *registers);
void LDHLn(CPURegisters *registers);
void LDIHLA(CPURegisters *registers);
void INCHL(CPURegisters *registers);
void INCH(CPURegisters *registers);
void DECH(CPURegisters *registers);
void LDH(CPURegisters *registers);
void DAA(CPURegisters *registers);
void JRZn(CPURegisters *registers);
void ADDHLHL(CPURegisters *registers);
void LDIAHL(CPURegisters *registers);
void DECHL(CPURegisters *registers);
void INCL(CPURegisters *registers);
void DECL(CPURegisters *registers);
void LDL(CPURegisters *registers);
void CPL(CPURegisters *registers);

void JRNCn(CPURegisters *registers);
void LDSP(CPURegisters *registers);
void LDDHLA(CPURegisters *registers);
void INCSP(CPURegisters *registers);
void INCHLa(CPURegisters *registers);
void DECHLa(CPURegisters *registers);
void LDHLaddrs(CPURegisters *registers);
void SCF(CPURegisters *registers);
void JRCn(CPURegisters *registers);
void ADDHLSP(CPURegisters *registers);
void LDDAHL(CPURegisters *registers);
void DECSP(CPURegisters *registers);
void INCA(CPURegisters *registers);
void DECA(CPURegisters *registers);
void LDA(CPURegisters *registers);
void CCF(CPURegisters *registers);

void LDBB(CPURegisters *registers);
void LDBC(CPURegisters *registers);
void LDBD(CPURegisters *registers);
void LDBE(CPURegisters *registers);
void LDBH(CPURegisters *registers);
void LDBL(CPURegisters *registers);
void LDBHL(CPURegisters *registers);
void LDBA(CPURegisters *registers);
void LDCB(CPURegisters *registers);
void LDCC(CPURegisters *registers);
void LDCD(CPURegisters *registers);
void LDCE(CPURegisters *registers);
void LDCH(CPURegisters *registers);
void LDCL(CPURegisters *registers);
void LDCHL(CPURegisters *registers);
void LDCA(CPURegisters *registers);

void LDDB(CPURegisters *registers);
void LDDC(CPURegisters *registers);
void LDDD(CPURegisters *registers);
void LDDE(CPURegisters *registers);
void LDDH(CPURegisters *registers);
void LDDL(CPURegisters *registers);
void LDDHL(CPURegisters *registers);
void LDDA(CPURegisters *registers);
void LDEB(CPURegisters *registers);
void LDEC(CPURegisters *registers);
void LDED(CPURegisters *registers);
void LDEE(CPURegisters *registers);
void LDEH(CPURegisters *registers);
void LDEL(CPURegisters *registers);
void LDEHL(CPURegisters *registers);
void LDEA(CPURegisters *registers);

void LDHB(CPURegisters *registers);
void LDHC(CPURegisters *registers);
void LDHD(CPURegisters *registers);
void LDHE(CPURegisters *registers);
void LDHH(CPURegisters *registers);
void LDHL(CPURegisters *registers);
void LDHHL(CPURegisters *registers);
void LDHA(CPURegisters *registers);
void LDLB(CPURegisters *registers);
void LDLC(CPURegisters *registers);
void LDLD(CPURegisters *registers);
void LDLE(CPURegisters *registers);
void LDLH(CPURegisters *registers);
void LDLL(CPURegisters *registers);
void LDLHL(CPURegisters *registers);
void LDLA(CPURegisters *registers);

void LDHLB(CPURegisters *registers);
void LDHLC(CPURegisters *registers);
void LDHLD(CPURegisters *registers);
void LDHLE(CPURegisters *registers);
void LDHLH(CPURegisters *registers);
void LDHLL(CPURegisters *registers);
//halt()
void LDHLA(CPURegisters *registers);
void LDAB(CPURegisters *registers);
void LDAC(CPURegisters *registers);
void LDAD(CPURegisters *registers);
void LDAE(CPURegisters *registers);
void LDAH(CPURegisters *registers);
void LDAL(CPURegisters *registers);
void LDAHL(CPURegisters *registers);
void LDAA(CPURegisters *registers);

void ADDAB(CPURegisters *registers);
void ADDAC(CPURegisters *registers);
void ADDAD(CPURegisters *registers);
void ADDAE(CPURegisters *registers);
void ADDAH(CPURegisters *registers);
void ADDAL(CPURegisters *registers);
void ADDAHL(CPURegisters *registers);
void ADDAA(CPURegisters *registers);
void ADCAB(CPURegisters *registers);
void ADCAC(CPURegisters *registers);
void ADCAD(CPURegisters *registers);
void ADCAE(CPURegisters *registers);
void ADCAH(CPURegisters *registers);
void ADCAL(CPURegisters *registers);
void ADCAHL(CPURegisters *registers);
void ADCAA(CPURegisters *registers);

void SUBB(CPURegisters *registers);
void SUBC(CPURegisters *registers);
void SUBD(CPURegisters *registers);
void SUBE(CPURegisters *registers);
void SUBH(CPURegisters *registers);
void SUBL(CPURegisters *registers);
void SUBHL(CPURegisters *registers);
void SUBA(CPURegisters *registers);
void SBCAB(CPURegisters *registers);
void SBCAC(CPURegisters *registers);
void SBCAD(CPURegisters *registers);
void SBCAE(CPURegisters *registers);
void SBCAH(CPURegisters *registers);
void SBCAL(CPURegisters *registers);
void SBCAHL(CPURegisters *registers);
void SBCAA(CPURegisters *registers);

void ANDB(CPURegisters *registers);
void ANDC(CPURegisters *registers);
void ANDD(CPURegisters *registers);
void ANDE(CPURegisters *registers);
void ANDH(CPURegisters *registers);
void ANDL(CPURegisters *registers);
void ANDHL(CPURegisters *registers);
void ANDA(CPURegisters *registers);
void XORB(CPURegisters *registers);
void XORC(CPURegisters *registers);
void XORD(CPURegisters *registers);
void XORE(CPURegisters *registers);
void XORH(CPURegisters *registers);
void XORL(CPURegisters *registers);
void XORHL(CPURegisters *registers);
void XORA(CPURegisters *registers);

void ORB(CPURegisters *registers);
void ORC(CPURegisters *registers);
void ORD(CPURegisters *registers);
void ORE(CPURegisters *registers);
void ORH(CPURegisters *registers);
void ORL(CPURegisters *registers);
void ORHL(CPURegisters *registers);
void ORA(CPURegisters *registers);
void CPB(CPURegisters *registers);
void CPC(CPURegisters *registers);
void CPD(CPURegisters *registers);
void CPE(CPURegisters *registers);
void CPH(CPURegisters *registers);
void CPLs(CPURegisters *registers);
void CPHL(CPURegisters *registers);
void CPA(CPURegisters *registers);

void RETNZ(CPURegisters *registers);
void POPBC(CPURegisters *registers);
void JPNZ(CPURegisters *registers);
void JP(CPURegisters *registers);
void CALLNZ(CPURegisters *registers);
void PUSHBC(CPURegisters *registers);
void ADDAn(CPURegisters *registers);
void RST00(CPURegisters *registers);
void RETZ(CPURegisters *registers);
void RET(CPURegisters *registers);
void JPZ(CPURegisters *registers);
void PREFIXCB(CPURegisters *registers);
void CALLZ(CPURegisters *registers);
void CALL(CPURegisters *registers);
void ADCAn(CPURegisters *registers);
void RST08(CPURegisters *registers);

void RETNC(CPURegisters *registers);
void POPDE(CPURegisters *registers);
void JPNC(CPURegisters *registers);
void CALLNC(CPURegisters *registers);
void PUSHDE(CPURegisters *registers);
void SUBn(CPURegisters *registers);
void RST10(CPURegisters *registers);
void RETC(CPURegisters *registers);
void RETI(CPURegisters *registers);
void JPC(CPURegisters *registers);
void CALLC(CPURegisters *registers);
void SBCAn(CPURegisters *registers);
void RST18(CPURegisters *registers);

void LDHnA(CPURegisters *registers);
void POPHL(CPURegisters *registers);
void LDCAaddr(CPURegisters *registers);
void PUSHHL(CPURegisters *registers);
void ANDn(CPURegisters *registers);
void RST20(CPURegisters *registers);
void ADDSPn(CPURegisters *registers);
void JPHL(CPURegisters *registers);
void LDnnA(CPURegisters *registers);
void XORn(CPURegisters *registers);
void RST28(CPURegisters *registers);

void LDHAn(CPURegisters *registers);
void POPAF(CPURegisters *registers);
void LDACaddr(CPURegisters *registers);
void DI(CPURegisters *registers);
void PUSHAF(CPURegisters *registers);
void ORn(CPURegisters *registers);
void RST30(CPURegisters *registers);
void LDHLSPn(CPURegisters *registers);
void LDSPHL(CPURegisters *registers);
void LDAnn(CPURegisters *registers);
void EI(CPURegisters *registers);
void CPn(CPURegisters *registers);
void RST38(CPURegisters *registers);

void RLCB(CPURegisters *registers);
void RLCC(CPURegisters *registers);
void RLCD(CPURegisters *registers);
void RLCE(CPURegisters *registers);
void RLCH(CPURegisters *registers);
void RLCL(CPURegisters *registers);
void RLCHL(CPURegisters *registers);
void RLCAC(CPURegisters *registers);
void RRCB(CPURegisters *registers);
void RRCC(CPURegisters *registers);


void RRC(CPURegisters *registers);
void RRD(CPURegisters *registers);
void RRE(CPURegisters *registers);
void RRAc(CPURegisters *registers);

void SLAA(CPURegisters *registers);

void z80BIT(CPURegisters *registers, unsigned char *theRegister, unsigned char mask);

void BIT0B(CPURegisters *registers);
void BIT0C(CPURegisters *registers);
void BIT0HL(CPURegisters *registers);
void BIT1B(CPURegisters *registers);

void BIT2B(CPURegisters *registers);
void BIT2A(CPURegisters *registers);
void BIT3B(CPURegisters *registers);
void BIT3A(CPURegisters *registers);

void BIT4B(CPURegisters *registers);
void BIT4C(CPURegisters *registers);
void BIT5B(CPURegisters *registers);
void BIT5C(CPURegisters *registers);
void BIT5A(CPURegisters *registers);

void BIT6B(CPURegisters *registers);
void BIT6A(CPURegisters *registers);
void BIT7B(CPURegisters *registers);
void BIT7H(CPURegisters *registers);
void BIT7HL(CPURegisters *registers);
void BIT7A(CPURegisters *registers);

void SWAPE(CPURegisters *registers);
void SWAPA(CPURegisters *registers);
void SRLB(CPURegisters *registers);
void SRLA(CPURegisters *registers);

void BIT1A(CPURegisters *registers);

void RLC(CPURegisters *registers);

void SET1E(CPURegisters *registers);
void SET7HL(CPURegisters *registers);

void RES0A(CPURegisters *registers);
void RES0HL(CPURegisters *registers);
void RES7HL(CPURegisters *registers);


void JPnn(CPURegisters *registers);
