/*	Copyright  (c)	Günter Woigk 1995 - 2020
					mailto:kio@little-bat.de

	This file is free software.

	Permission to use, copy, modify, distribute, and sell this software
	and its documentation for any purpose is hereby granted without fee,
	provided that the above copyright notice appears in all copies and
	that both that copyright notice, this permission notice and the
	following disclaimer appear in supporting documentation.

	THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT ANY WARRANTY, NOT EVEN THE
	IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
	AND IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DAMAGES
	ARISING FROM THE USE OF THIS SOFTWARE,
	TO THE EXTENT PERMITTED BY APPLICABLE LAW.
*/


#include "kio/kio.h"
#include "z80_goodies.h"


static char const len0[65] = // opcodes 0x00 - 0x3F
	"13111121"	// 	NOP,		LD_BC_NN,	LD_xBC_A,	INC_BC,		INC_B,		DEC_B,		LD_B_N,		RLCA,
	"11111121"	//	EX_AF_AF,	ADD_HL_BC,	LD_A_xBC,	DEC_BC,		INC_C,		DEC_C,		LD_C_N,		RRCA,
	"23111121"	//	DJNZ,		LD_DE_NN,	LD_xDE_A,	INC_DE,		INC_D,		DEC_D,		LD_D_N,		RLA,
	"21111121"	//	JR, 		ADD_HL_DE,	LD_A_xDE,	DEC_DE,		INC_E,		DEC_E,		LD_E_N,		RRA,
	"23311121"	//	JR_NZ,		LD_HL_NN,	LD_xNN_HL,	INC_HL,		INC_H,		DEC_H,		LD_H_N,		DAA,
	"21311121"	//	JR_Z,		ADD_HL_HL,	LD_HL_xNN,	DEC_HL,		INC_L,		DEC_L,		LD_L_N,		CPL,
	"23311121"	//	JR_NC,		LD_SP_NN,	LD_xNN_A,	INC_SP,		INC_xHL,	DEC_xHL,	LD_xHL_N,	SCF,
	"21311121";	//	JR_C,		ADD_HL_SP,	LD_A_xNN,	DEC_SP,		INC_A,		DEC_A,		LD_A_N,		CCF,

static char const len3[65] = // opcodes 0xC0 - 0xFF: prefixes are 0
	"11333121"	//	RET_NZ,		POP_BC,		JP_NZ,		JP,			CALL_NZ,	PUSH_BC,	ADD_N,		RST00,
	"11303321"	//	RET_Z,		RET,		JP_Z,		PFX_CB,		CALL_Z,		CALL,		ADC_N,		RST08,
	"11323121"	//	RET_NC,		POP_DE,		JP_NC,		OUTA,		CALL_NC,	PUSH_DE,	SUB_N,		RST10,
	"11323021"	//	RET_C,		EXX,		JP_C,		INA,		CALL_C,		PFX_IX,		SBC_N,		RST18,
	"11313121"	//	RET_PO,		POP_HL,		JP_PO,		EX_HL_xSP,	CALL_PO,	PUSH_HL,	AND_N,		RST20,
	"11313021"	//	RET_PE,		JP_HL,		JP_PE,		EX_DE_HL,	CALL_PE,	PFX_ED,		XOR_N,		RST28,
	"11313121"	//	RET_P,		POP_AF,		JP_P,		DI,			CALL_P,		PUSH_AF,	OR_N,		RST30,
	"11313021";	//	RET_M,		LD_SP_HL,	JP_M,		EI,			CALL_M,		PFX_IY,		CP_N,		RST38

#if 0
static char const z180_ed_len0[65] = // opcodes 0xED 0x00 - 0x3F
{
	"33222222"	//	IN0_B_xN  OUT0_xN_B	 NOP  TST_B	  NOP  NOP  NOP  NOP
	"33222222"	//	IN0_C_xN  OUT0_xN_C	 NOP  TST_C	  NOP  NOP  NOP  NOP
	"33222222"	//	IN0_D_xN  OUT0_xN_D	 NOP  TST_D	  NOP  NOP  NOP  NOP
	"33222222"	//	IN0_E_xN  OUT0_xN_E	 NOP  TST_E	  NOP  NOP  NOP  NOP
	"33222222"	//	IN0_H_xN  OUT0_xN_H	 NOP  TST_H	  NOP  NOP  NOP  NOP
	"33222222"	//	IN0_L_xN  OUT0_xN_L	 NOP  TST_L	  NOP  NOP  NOP  NOP
	"32222222"	//	IN0_F_xN  NOP		 NOP  TST_xHL NOP  NOP  NOP  NOP
	"33222222"	//	IN0_A_xN  OUT0_xN_A	 NOP  TST_A	  NOP  NOP  NOP  NOP
};

static char const i8080len0[65] = // opcodes 0x00 - 0x3F
	"13111121"	// 	NOP,		LD_BC_NN,	LD_xBC_A,	INC_BC,		INC_B,		DEC_B,		LD_B_N,		RLCA,
	"11111121"	//	NOP_08,		ADD_HL_BC,	LD_A_xBC,	DEC_BC,		INC_C,		DEC_C,		LD_C_N,		RRCA,
	"13111121"	//	NOP_10,		LD_DE_NN,	LD_xDE_A,	INC_DE,		INC_D,		DEC_D,		LD_D_N,		RLA,
	"11111121"	//	NOP_18,		ADD_HL_DE,	LD_A_xDE,	DEC_DE,		INC_E,		DEC_E,		LD_E_N,		RRA,
	"13311121"	//	NOP_20,		LD_HL_NN,	LD_xNN_HL,	INC_HL,		INC_H,		DEC_H,		LD_H_N,		DAA,
	"11311121"	//	NOP_28,		ADD_HL_HL,	LD_HL_xNN,	DEC_HL,		INC_L,		DEC_L,		LD_L_N,		CPL,
	"13311121"	//	NOP_30,		LD_SP_NN,	LD_xNN_A,	INC_SP,		INC_xHL,	DEC_xHL,	LD_xHL_N,	SCF,
	"11311121";	//	NOP_38,		ADD_HL_SP,	LD_A_xNN,	DEC_SP,		INC_A,		DEC_A,		LD_A_N,		CCF,

static char const i8080len3[65] = // opcodes 0xC0 - 0xFF: prefixes are 0
	"11333121"	//	RET_NZ,		POP_BC,		JP_NZ,		JP,			CALL_NZ,	PUSH_BC,	ADD_N,		RST00,
	"11333321"	//	RET_Z,		RET,		JP_Z,		JP_CB,		CALL_Z,		CALL,		ADC_N,		RST08,
	"11323121"	//	RET_NC,		POP_DE,		JP_NC,		OUTA,		CALL_NC,	PUSH_DE,	SUB_N,		RST10,
	"11323321"	//	RET_C,		NOP_D9,		JP_C,		INA,		CALL_C,		CALL_DD,	SBC_N,		RST18,
	"11313121"	//	RET_PO,		POP_HL,		JP_PO,		EX_HL_xSP,	CALL_PO,	PUSH_HL,	AND_N,		RST20,
	"11313321"	//	RET_PE,		JP_HL,		JP_PE,		EX_DE_HL,	CALL_PE,	CALL_ED,	XOR_N,		RST28,
	"11313121"	//	RET_P,		POP_AF,		JP_P,		DI,			CALL_P,		PUSH_AF,	OR_N,		RST30,
	"11313321";	//	RET_M,		LD_SP_HL,	JP_M,		EI,			CALL_M,		CALL_FD,	CP_N,		RST38
#endif


uint i8080_opcode_length (const Byte* ip) noexcept
{
	// Calculate length [bytes] of instruction

	uint8 op = peek(ip);

	switch (op>>6)
	{
	case 0:
		if ((op&7)==0) return 1;				// 0x00=NOP and EX AF,AF'=NOP => len=1
		else return uint(len0[op]-'0');
	default:
		return 1;								// ld r,r and arith a,r
	case 3:
		if (len3[op-0xc0]=='0') return 3;		// prefixes are JP_NN or CALL_NN => len=3
		else return uint(len3[op-0xc0] - '0');	// 0xD9=NOP: same length as Z80:EXX
	}
}

uint z80_opcode_length (const Byte* ip) noexcept
{
	// Calculate length of instruction
	// op2 is only used if op1 is a prefix instruction
	// IX/IY before IX/IY/ED have no effect and are reported as length 1

	uint8 op = peek(ip);

	switch(op>>6)
	{
	case 0:	return uint(len0[op]-'0');	// 0x00 - 0x3F:	various length
	case 1:								// 0x40 - 0x7F: ld r,r: all 1
	case 2:	return 1;					// 0x80 - 0xBF:	arithmetics/logics op a,r: all 1
	}

	// 0xC0 .. 0xFF:

	// test for prefix:
	switch(op)
	{
	case 0xcb:	return 2;
	case 0xed:  return (peek(ip+1) & 0xc7) == 0x43 ? 4 : 2;
	case 0xdd:
	case 0xfd:
		op = peek(ip+1);
		switch (op>>6)
		{
		case 0:	return uint(len0[op]+1-'0') + (op>=0x34 && op<=0x36);	// inc(hl); dec(hl); ld(hl),N: add displacement
		case 1:
		case 2:	return ((op&0x07)==6) != ((op&0x0F8)==0x70) ? 3 : 2;	// if (hl) add displacement except for HALT
		}
		if (op==0xcb) return 4;
		return uint(len3[op&0x3F]+1-'0');	// note: entries for prefixes are 0 giving a total of 1,
	}										// just to skip the useless prefix

	return uint(len3[op&0x3F]-'0');			// 0xC0 - 0xFF:	no prefix:	various length
}

#ifdef DEBUG
#include "z80_opcodes.h"
ON_INIT([]{
	static const Byte ld_a_xix[] = {0xdd,LD_A_xHL,0};	assert(z180_opcode_length(ld_a_xix)==3);
	static const Byte ld_xix_a[] = {0xdd,LD_xHL_A,0};	assert(z180_opcode_length(ld_xix_a)==3);
	static const Byte ld_b_xix[] = {0xdd,LD_B_xHL,0};	assert(z180_opcode_length(ld_b_xix)==3);
	static const Byte ld_xix_b[] = {0xdd,LD_xHL_B,0};	assert(z180_opcode_length(ld_xix_b)==3);
	static const Byte ld_c_xix[] = {0xdd,LD_C_xHL,0};	assert(z180_opcode_length(ld_c_xix)==3);
	static const Byte ld_xix_c[] = {0xdd,LD_xHL_C,0};	assert(z180_opcode_length(ld_xix_c)==3);
	static const Byte ld_ix_halt[] = {0xdd,HALT,0};  	assert(z180_opcode_length(ld_ix_halt)==2);
});
#endif

uint z180_opcode_length (const Byte* ip) noexcept
{
	// Calculate length [bytes] of instruction
	// op2 is only used if op1 is a prefix instruction
	// illegal opcodes are reported as for Z80

	if (peek(ip) == 0xED)				// additional opcodes of the Z80180 all have prefix 0xED
	{
		uint8 op2 = peek(ip+1);
		switch(op2>>6)
		{
		case 0:										// IN0_r_xN and OUT0_xN_r
			return 2 + ((op2&7)<=1 && op2!=0x31);	// not shure for 0x31: ill. OUT0_xN_0 ?
		case 1:
			if ((op2|0x10)==0x74) return 3;			// TST_N and TSTIO_N
			else break;
		}
	}

	return z80_opcode_length(ip);		// all other opcodes: same as Z80
}

uint cpu_opcode_length (CpuID cpuid, const Byte* ip) noexcept
{
	switch(cpuid)
	{
	case Cpu8080:
		return i8080_opcode_length(ip);

	case CpuZ180:
		return z180_opcode_length(ip);

	default:
	case CpuZ80:
		return z80_opcode_length(ip);
	}
}







