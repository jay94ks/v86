#ifndef __V86_CPU_STATE_H__
#define __V86_CPU_STATE_H__
#include "reg.h"
#include "../mask.h"

namespace v86 {
	struct state_t {
		reg_t regs[REG_MAX];
		reg_t segs[SEG_MAX];
	};

	/* initial value of eflags. */
#define EFLAGS_INIT_VALUE	1

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

#define ss regs[SEG_SS].dword
#define cs regs[SEG_CS].dword
#define ds regs[SEG_DS].dword
#define es regs[SEG_ES].dword
#define fs regs[SEG_FS].dword
#define gs regs[SEG_GS].dword

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
}

#endif // __V86_CPU_STATE_H__