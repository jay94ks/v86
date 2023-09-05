#include "i8086.h"

namespace v86 {

	uint8_t Ci8086::PARITY_MAP[32] = {
		0x96, 0x69, 0x69, 0x96, 0x69, 0x96, 0x96, 0x69, 0x69, 0x96,
		0x96, 0x69, 0x96, 0x69, 0x69, 0x96, 0x69, 0x96, 0x96, 0x69,
		0x96, 0x69, 0x69, 0x96, 0x96, 0x69, 0x69, 0x96, 0x69, 0x96,
		0x96, 0x69
	};

	uint8_t Ci8086::fetch() {
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fetch);

		uint8_t code;
		uint32_t addr = addr16(SEG_CS, state->ip);

		state->ip++;
		read(addr, &code, 1);

		// --> store fetched code byte.
		fetch->fetch[fetch->length++] = code;
		return code;
	}

	void Ci8086::push(const void* buf, uint32_t size)
	{
		USE_STATE(this, state);

		
		/* set the overflow flag. */
		// Overflow = state->sp < size;
		state->sp -= size;

		/* write from the buffer. */
		write(addr16(SEG_SS, state->sp), buf, size);
	}

	void Ci8086::pop(void* buf, uint32_t size)
	{
		USE_STATE(this, state);

		/* set the overflow flag. */
		// Overflow = state->sp > 0xffffu - size;

		/* read to the buffer. */
		read(addr16(SEG_SS, state->sp), buf, size);
		state->sp += size;
	}

	void Ci8086::exec()
	{
		// todo: trap, intcall(1).
		// todo: halt.

		USE_STATE(this, state);

		// --> clear the prefix state.
		state->prefix.use = 0;
		state->prefix.seg = state->ds; // --> data segment. 

		// --> reset the fetch state.
		state->fetch.length = 0;
		state->fetch.prefix = 0;

		// --> store starting EIP, CS.
		state->t_eip = state->eip;
		state->t_cs = state->cs;

		// --> reboot checking.
		if (state->cs == 0xf000 && state->ip == 0xe066) {
			// todo: clear boot flag.
		}

		uint8_t opcode;
		while (true) {
			// --> store previous EIP, CS.
			state->p_eip = state->eip;
			state->p_cs = state->cs;

			opcode = fetch();
			state->eip++;

			// --> handle segment override prefixes and repeat prefixes.
			if (execSov16(opcode) == false) {
				break;
			}

			continue;
		}

		/**
		 *
		 * 0	0000	Mod RM
		 * 1	0001	Mod RM
		 * 2	0010	Mod RM
		 * 3	0011	Mod RM
		 * 4	0100
		 * 5	0101
		 * 6	0110
		 * 7	0111
		 * 8	1000	Mod RM
		 * 9	1001	Mod RM
		 * A	1010	Mod RM
		 * B	1011	Mod RM
		 * C	1100
		 * D	1101
		 * E	1110
		 * F	1111
		 */
		if ((opcode & 3) != 0) {
			fetchModRm16();
		}

		uint8_t series = opcode >> 4;
		switch (series) {
		case 0x00: onOpcode0X(opcode); break;
		}
	}

	bool Ci8086::execSov16(uint8_t opcode) {
		switch (opcode) {
		case 0x26: // --> ES override.
		case 0x2e: // --> CS override.
		case 0x36: // --> SS override.
		case 0x3e: // --> DS override.
		{
			USE_STATE(this, state);
			uint8_t n = (opcode - 0x26) >> 3; // 0 ~ 3.
			/* ((opcode - 0x26) div 8). */

			state->prefix.seg = state->segs[n].dword;
			state->prefix.use = 1; // --> override.
			state->fetch.prefix++;
			return true;
		}

		case 0xf3: // --> rep/repe/repz.
		case 0xf2: // --> repne/repnz.
		{
			USE_STATE(this, state);
			state->prefix.rep = opcode;
			state->fetch.prefix++;
			return true;
		}

		default:
			break;
		}

		return false;
	}

	void Ci8086::fetchModRm16()
	{
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		uint8_t byte = fetch();
		uint8_t mode = fst->mode = byte >> 6;
		uint8_t reg = fst->reg = (byte >> 3) & 7;
		uint8_t rm = fst->rm = byte & 7;

		// --> remember the ModRM's index.
		fst->modrm = fst->length - 1;
		switch (mode) {
		case 0:
			// --> fetch `disp16` word.
			if (rm == 6) {
				read(addr16(SEG_CS, state->ip), &fst->disp.word[REG_WORD], sizeof(uint16_t));
				state->ip += 2; fst->disp.word[REG_WORD_HI] = 0;
			}

			// --> replace to stack segment.
			if ((rm == 2 || rm == 3) && !state->prefix.use) {
				state->prefix.seg = state->ss;
			}
			break;

		case 1:
			// --> fetch `disp8` byte.
			read(addr16(SEG_CS, state->ip), &fst->disp.byte[REG_BYTE_LO], sizeof(uint8_t));
			fst->disp.byte[REG_BYTE_HI] = 0; // --> fill zero to high 8 bit.
			fst->disp.word[REG_WORD_HI] = 0;
			state->ip++;

			// --> replace to stack segment.
			if ((rm == 2 || rm == 3 || rm == 6) && !state->prefix.use) {
				state->prefix.seg = state->ss;
			}
			break;

		case 2:
			read(addr16(SEG_CS, state->ip), &fst->disp.word[REG_WORD], sizeof(uint16_t));
			state->ip += 2; fst->disp.word[REG_WORD_HI] = 0;

			// --> replace to stack segment.
			if ((rm == 2 || rm == 3 || rm == 6) && !state->prefix.use) {
				state->prefix.seg = state->ss;
			}
			break;

		default:
			fst->disp.dword = 0;
			break;
		}
	}

	uint32_t Ci8086::addrModRM16()
	{
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		uint32_t addr = 0;

		switch (fst->mode) {
		case 0: {
			/**
			 * 0 000	BX + SI
			 * 1 001	BX + DI
			 * 2 010	BP + SI
			 * 3 011	BP + DI
			 * 4 100	SI
			 * 5 101	DI
			 * 6 110	DISP16
			 * 7 111	BX
			 */

			switch (fst->rm) {
			case 0: addr = state->bx + state->si; break;
			case 1: addr = state->bx + state->di; break;
			case 2: addr = state->bp + state->si; break;
			case 3: addr = state->bp + state->di; break;

			case 4: addr = state->si; break;
			case 5: addr = state->di; break;
			case 6: addr = fst->disp.word[REG_WORD]; break;
			case 7: addr = state->bx; break;
			}
			break;
		}

		case 1:
		case 2:
			/**
			 * 0 000	BX + SI + DISP16
			 * 1 001	BX + DI + DISP16
			 * 2 010	BP + SI + DISP16
			 * 3 011	BP + DI + DISP16
			 * 4 100	SI + DISP16
			 * 5 101	DI + DISP16
			 * 6 110	BP + DISP16
			 * 7 111	BX + DISP16
			 */

			switch (fst->rm) {
			case 0: addr = state->bx + state->si + fst->disp.word[REG_WORD]; break;
			case 1: addr = state->bx + state->di + fst->disp.word[REG_WORD]; break;
			case 2: addr = state->bp + state->si + fst->disp.word[REG_WORD]; break;
			case 3: addr = state->bp + state->di + fst->disp.word[REG_WORD]; break;

			case 4: addr = state->si + fst->disp.word[REG_WORD]; break;
			case 5: addr = state->di + fst->disp.word[REG_WORD]; break;
			case 6: addr = state->bp + fst->disp.word[REG_WORD]; break;
			case 7: addr = state->bx + fst->disp.word[REG_WORD]; break;
			}

			break;

		default:
			break;
		}

		return addr16imm(state->prefix.seg, addr);
	}

#define RM_READ_FROM_MEM(type)	\
	if (fst->mode < 3) { \
		type temp;\
		read(addrModRM16(), &temp, sizeof(temp));\
		return temp;\
	}

#define RM_WRITE_INTO_MEM(value)	\
	if (fst->mode < 3) { \
		read(addrModRM16(), &value, sizeof(value));\
		return;\
	}

#define RM_REG_WORD(rm)		state->regs[rm].word[REG_WORD]
#define RM_REG_DWORD(rm)	state->regs[rm].dword

// RM= AL (0 | 0), CL (0 | 1), DL (0 | 2), BL (0 | 3),
//   : AH (4 | 0), CH (4 | 1), DH (4 | 2), BH (4 | 3)
#define RM_REG_BYTE(rm)		state->regs[(rm) & 0x03].byte[((rm) & 0x04) ? REG_BYTE_HI : REG_BYTE_LO]

	uint8_t Ci8086::readRM8() {
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		RM_READ_FROM_MEM(uint8_t);
		return RM_REG_BYTE(fst->rm);
	}

	uint16_t Ci8086::readRM16() {
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		RM_READ_FROM_MEM(uint16_t);
		return RM_REG_WORD(fst->rm);
	}

	uint32_t Ci8086::readRM32()
	{
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		RM_READ_FROM_MEM(uint32_t);
		return RM_REG_DWORD(fst->rm);
	}

	void Ci8086::writeRM8(uint8_t value) {
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		RM_WRITE_INTO_MEM(value);

		uint8_t reg = fst->rm & 0x03;
		uint8_t off = (fst->rm & 0x04)
			? REG_BYTE_HI : REG_BYTE_LO;

		state->regs[reg].byte[off] = value;
	}

	void Ci8086::writeRM16(uint16_t value) {
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		RM_WRITE_INTO_MEM(value);
		RM_REG_WORD(fst->rm) = value;
	}

	void Ci8086::writeRM32(uint32_t value) {
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);
		RM_WRITE_INTO_MEM(value);
		RM_REG_DWORD(fst->rm) = value;
	}

	/* PUSH, POP macros. */
#define PUSH16_SEG(type, seg)	\
	type val = state->seg; \
	push(&val, sizeof(val))

#define POP16_SEG(type, seg) \
	type val; \
	pop(&val, sizeof(val)); \
	state->seg = val;

#define PUSH16_REG(type, reg)	\
	type val = state->reg; \
	push(&val, sizeof(val))

#define POP16_REG(type, reg) \
	type val; \
	pop(&val, sizeof(val)); \
	state->reg = val;

#define OPERAND_RM8_REG8() \
	fst->op[0].dword = readRM8(); \
	fst->op[1].dword = RM_REG_BYTE(fst->reg)

#define OPERAND_RM16_REG16() \
	fst->op[0].dword = readRM16(); \
	fst->op[1].dword = RM_REG_WORD(fst->reg)

#define OPERAND_REG8_RM8() \
	fst->op[0].dword = RM_REG_BYTE(fst->reg); \
	fst->op[1].dword = readRM8()

#define OPERAND_REG16_RM16() \
	fst->op[0].dword = RM_REG_WORD(fst->reg); \
	fst->op[1].dword = readRM16()

#define OPERAND_RegAL_Ib() \
	fst->op[0].dword = state->al; \
	fst->op[1].dword = fetch()

#define OPERAND_RegEAX_Iv() \
	fst->op[0].dword = state->ax; \
	fst->op[1].dword = fetch()

#define COMPUTE(exec)	\
	fst->res.dword = fst->op[0].dword exec fst->op[1].dword

#define RES_XOR_OP_N(n) (fst->res.dword ^ fst->op[n].dword)
#define RES_XOR_OP_TWO() (fst->res.dword ^ fst->op[0].dword ^ fst->op[1].dword)

#define FLAG_ZF_SF_PF(size) \
	eflag<EFLAG_ZF>(state, fst->res.dword ? 0 : 1);\
	eflag<EFLAG_SF>(state, fst->res.dword >> (8 * size - 1));\
	eflag<EFLAG_PF>(state, parity(fst->res.byte[REG_BYTE_LO]))

#define FLAG_CF_OF_AF(size) \
	eflag<EFLAG_CF>(state, fst->res.word[REG_WORD_HI] ? 1 : 0);\
	eflag<EFLAG_OF>(state, (RES_XOR_OP_N(0) & RES_XOR_OP_N(1) & 0x8000) == 0x8000 ? 1: 0);\
	eflag<EFLAG_AF>(state, (RES_XOR_OP_TWO() & 0x10) == 0x10 ? 1 : 0)

#define FLAG_CLEAR_CF_OF()	\
	eflag<EFLAG_CF>(state, 0);\
	eflag<EFLAG_OF>(state, 0);\
	

	void Ci8086::onOpcode0X(uint8_t opcode) {
		USE_STATE(this, state);
		USE_FETCH_STATE(this, fst);

		switch (opcode) {
		case 0x00: { /* 00 ADD Eb Gb */
			OPERAND_RM8_REG8();
			COMPUTE(+);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CF_OF_AF(sizeof(uint8_t));
			writeRM8(fst->res.dword);
			break;
		}

		case 0x01: { /* 01 ADD Ev Gv */
			OPERAND_RM16_REG16();
			COMPUTE(+);
			FLAG_ZF_SF_PF(sizeof(uint16_t));
			FLAG_CF_OF_AF(sizeof(uint16_t));
			writeRM16(fst->res.dword);
			break;
		}

		case 0x02: { /* 02 ADD Gb Eb */
			OPERAND_REG8_RM8();
			COMPUTE(+);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CF_OF_AF(sizeof(uint8_t));
			RM_REG_BYTE(fst->reg) = fst->res.dword;
			break;
		}

		case 0x03: { /* 03 ADD Gv Ev */
			OPERAND_REG16_RM16();
			COMPUTE(+);
			FLAG_ZF_SF_PF(sizeof(uint16_t));
			FLAG_CF_OF_AF(sizeof(uint16_t));
			RM_REG_WORD(fst->reg) = fst->res.dword;
			break;
		}

		case 0x04: { /* 04 ADD REG_AL Ib */
			OPERAND_RegAL_Ib();
			COMPUTE(+);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CF_OF_AF(sizeof(uint8_t));
			state->al = fst->res.dword;
			break;
		}

		case 0x05: { /* 05 ADD eAX Iv */
			OPERAND_RegEAX_Iv();
			COMPUTE(+);
			FLAG_ZF_SF_PF(sizeof(uint16_t));
			FLAG_CF_OF_AF(sizeof(uint16_t));
			state->ax = fst->res.dword;
			break;
		}

		case 0x06: { /* 06 PUSH SEG_ES */
			PUSH16_SEG(uint16_t, es);
			break;
		}

		case 0x07: { /* 07 POP SEG_ES */
			POP16_SEG(uint16_t, es);
			break;
		}

		case 0x08: { /* 08 OR Eb Gb */
			OPERAND_RM8_REG8();
			COMPUTE(|);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CLEAR_CF_OF();
			writeRM8(fst->res.dword);
			break;
		}

		case 0x09: { /* 09 OR Ev Gv */
			OPERAND_RM16_REG16();
			COMPUTE(|);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CLEAR_CF_OF();
			writeRM16(fst->res.dword);
			break;

		}
		case 0x0A: { /* 0A OR Gb Eb */
			OPERAND_REG8_RM8();
			COMPUTE(|);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CLEAR_CF_OF();
			RM_REG_BYTE(fst->reg) = fst->res.dword;
			break;
		}
		case 0x0B: { /* 0B OR Gv Ev */
			OPERAND_REG16_RM16();
			COMPUTE(|);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CLEAR_CF_OF();
			RM_REG_WORD(fst->reg) = fst->res.dword;
			break;
		}
		case 0x0C: { /* 0C OR REG_AL Ib */
			OPERAND_RegAL_Ib();
			COMPUTE(|);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CLEAR_CF_OF();
			state->al = fst->res.dword;
			break;
		}
		case 0x0D: { /* 0D OR eAX Iv */
			OPERAND_RegEAX_Iv();
			COMPUTE(|);
			FLAG_ZF_SF_PF(sizeof(uint8_t));
			FLAG_CLEAR_CF_OF();
			state->ax = fst->res.dword;
			break;

		}
		case 0x0E: { /* 0E PUSH SEG_CS */
			PUSH16_SEG(uint16_t, cs);
			break;
		}
		case 0x0F: { /* 0F POP SEG_CS */
			POP16_SEG(uint16_t, cs);
			break;
		}
		}
	}

}