#ifndef __V86_CPU_REG_H__
#define __V86_CPU_REG_H__
#include "../types.h"

namespace v86 {
	enum EREGS {
		REG_EAX = 0,
		REG_ECX,
		REG_EDX,
		REG_EBX,
		REG_ESP,
		REG_EBP,
		REG_ESI,
		REG_EDI,

		REG_EIP,
		REG_EFLAGS,

		REG_P_EIP, // -- previous EIP.
		REG_T_EIP, // -- trace purpose EIP.
		REG_MAX
	};

	enum ESEGS {
		SEG_ES = 0,
		SEG_CS,
		SEG_SS,
		SEG_DS,
		SEG_GS,
		SEG_FS,

		SEG_P_CS, // -- previous CS.
		SEG_T_CS, // -- trace purpose CS.
		SEG_MAX
	};

	enum EFLAGS {
		EFLAG_CF = 0,
		EFLAG_PF = 2,
		EFLAG_AF = 4,
		EFLAG_ZF = 6,
		EFLAG_SF = 7,
		EFLAG_TF = 8,
		EFLAG_IT = 9,
		EFLAG_DF = 10,
		EFLAG_OF = 11,
		EFLAG_IOPL = 12 | 0x80,
		EFLAG_NT = 14,
		EFLAG_RF = 16,
		EFLAG_VM = 17,
		EFLAG_AC = 18,
		EFLAG_VIF = 19,
		EFLAG_VIP = 20,
		EFLAG_ID = 21
	};

	/* register type. */
	union reg_t {
		uint32_t dword;
		uint16_t word[2];
		uint8_t  byte[4];
	};

	/* segment register type. */
	union seg_t {
		uint32_t dword;
		uint16_t word[2];
		uint8_t  byte[4];
	};
}

#endif // __V86_CPU_REG_H__