#ifndef __V86_CPU_STATE_H__
#define __V86_CPU_STATE_H__
#include "reg.h"
#include "../mask.h"

namespace v86 {
	enum EREPF {
		REP_NONE = 0,
		REP_PFX_F2 = 0xf2,	// --> REP, REPE, REPZ.
		REP_PFX_F3 = 0xf3,	// --> REPNE, REPNZ.
	};

	/* opcode prefix. */
	struct prefix_t {
		uint32_t seg;
		uint8_t use : 1;
		uint8_t rep;
	};

	/* fetch state. */
	struct fetch_t {
		uint8_t fetch[16]; // --> fetched bytes.
		uint8_t prefix; // --> prefix length
		uint8_t length; // --> total length
		uint8_t modrm; // --> modrm byte's index.

		/* Mode RMs. */
		uint8_t mode;
		uint8_t reg;
		uint8_t rm;

		/* disp 8/16. */
		reg_t disp;
		reg_t res;
		reg_t op[2];
	};

	/* processor state. */
	struct state_t {
		reg_t regs[REG_MAX];
		reg_t segs[SEG_MAX];

		fetch_t fetch;
		prefix_t prefix; // --> prefix info.
	};

	/* initial value of eflags. */
#define EFLAGS_INIT_VALUE	1
#ifndef __V86_BIG_ENDIAN__
#define REG_WORD		1
#define REG_WORD_HI		0
#define REG_BYTE_LO		3
#define REG_BYTE_HI		2
#else
#define REG_WORD		0
#define REG_WORD_HI		1
#define REG_BYTE_LO		0
#define REG_BYTE_HI		1
#endif

#define eax regs[REG_EAX].dword
#define ebx regs[REG_EBX].dword
#define ecx regs[REG_ECX].dword
#define edx regs[REG_EDX].dword
#define esi regs[REG_ESI].dword
#define edi regs[REG_EDI].dword
#define ebp regs[REG_EBP].dword
#define esp regs[REG_ESP].dword
#define eip regs[REG_EIP].dword
#define eflags regs[REG_EFLAGS].dword

#define ax regs[REG_EAX].word[REG_WORD]
#define bx regs[REG_EBX].word[REG_WORD]
#define cx regs[REG_ECX].word[REG_WORD]
#define dx regs[REG_EDX].word[REG_WORD]
#define si regs[REG_ESI].word[REG_WORD]
#define di regs[REG_EDI].word[REG_WORD]
#define bp regs[REG_EBP].word[REG_WORD]
#define sp regs[REG_ESP].word[REG_WORD]
#define ip regs[REG_EIP].word[REG_WORD]
#define flags regs[REG_EFLAGS].word[REG_WORD]


#define ah regs[REG_EAX].byte[REG_BYTE_HI]
#define al regs[REG_EAX].byte[REG_BYTE_LO]
#define bh regs[REG_EAX].byte[REG_BYTE_HI]
#define bl regs[REG_EAX].byte[REG_BYTE_LO]
#define ch regs[REG_EAX].byte[REG_BYTE_HI]
#define cl regs[REG_EAX].byte[REG_BYTE_LO]
#define dh regs[REG_EAX].byte[REG_BYTE_HI]
#define dl regs[REG_EAX].byte[REG_BYTE_LO]

#define ss regs[SEG_SS].dword
#define cs regs[SEG_CS].dword
#define ds regs[SEG_DS].dword
#define es regs[SEG_ES].dword
#define fs regs[SEG_FS].dword
#define gs regs[SEG_GS].dword

	// --> to remember previous EIP, CS.
#define p_eip regs[REG_P_EIP].dword
#define p_ip regs[REG_P_EIP].word[REG_WORD]
#define p_cs regs[SEG_P_CS].dword

	// --> to trace starting EIP, CS.
#define t_eip regs[REG_T_EIP].dword
#define t_ip regs[REG_T_EIP].word[REG_WORD]
#define t_cs regs[SEG_T_CS].dword

	template<EFLAGS flag> /* getter */
	inline uint8_t eflag(state_t* state) {
		constexpr uint32_t m = mask<flag & 0x7f, (flag & 0x80) ? 2 : 1>();
		return state->eflags & m;
	}

	template<EFLAGS flag> /* setter */
	inline void eflag(state_t* state, uint8_t value) {
		constexpr uint32_t t = mask<0, (flag & 0x80) ? 2 : 1>();
		constexpr uint32_t m = t << (flag & 0x7f);

		state->eflags &= ~m;
		state->eflags |= (value & t) << (flag & 0x7f);
	}

#define REG_MASK_HI16	0xffff0000u
#define REG_MASK_LO16	0x0000ffffu
#define REG_MASK_WORD	REG_MASK_LO16
#define REG_MASK_HI8	0x0000ff00u
#define REG_MASK_LO8	0x000000ffu
}

#endif // __V86_CPU_STATE_H__