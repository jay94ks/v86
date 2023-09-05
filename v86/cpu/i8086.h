#ifndef __V86_CPU_I8086_H__
#define __V86_CPU_I8086_H__
#include "proc.h"

namespace v86 {

	/* 8086 processor. */
	class Ci8086 : public IProc {
	protected:
		static uint8_t PARITY_MAP[32];
		inline static uint8_t parity(uint8_t n) {
			return PARITY_MAP[n / 8] >> (7 - n);
		}

	protected:
		/* translate 16-bit [IMM:ADDR] value to linear address. */
		inline uint32_t addr16imm(uint32_t seg, uint16_t addr) const {
			return ((seg & 0xffff) << 4) + uint32_t(addr & REG_MASK_WORD);
		}

		/* translate 16-bit [SEG:ADDR] value to linear address. */
		inline uint32_t addr16(ESEGS seg, uint16_t addr) const {
			USE_STATE(this, state);
			return addr16imm(state->segs[seg].dword, addr);
		}

	public:
		/* fetch a code byte. */
		virtual uint8_t fetch() override;

		/* fetch a code word. */
		inline uint16_t fetch16() {
			uint16_t first = fetch();
			return first | (uint16_t(fetch()) << 8);
		}

		/* push bytes to the stack. */
		virtual void push(const void* buf, uint32_t size) override;

		/* pop bytes from the stack. */
		virtual void pop(void* buf, uint32_t size) override;

	public:
		/* execute single step. */
		virtual void exec() override;

	protected:
		/* execute segment overrides. */
		virtual bool execSov16(uint8_t opcode);

	protected:
		/* fetch ModRM byte. */
		virtual void fetchModRm16();

		/* get the address from Mod RM byte. */
		virtual uint32_t addrModRM16();

		/* read reg/mem 8. */
		virtual uint8_t readRM8();

		/* read reg/mem 16.*/
		virtual uint16_t readRM16();

		/* read reg/mem 32. */
		virtual uint32_t readRM32();

		/* write reg/mem 8. */
		virtual void writeRM8(uint8_t value);

		/* write reg/mem 16. */
		virtual void writeRM16(uint16_t value);

		/* write reg/mem 32. */
		virtual void writeRM32(uint32_t value);

	protected:
		/* 0x00 ~ 0x0F opcode series (ADD, PUSH/POP ES, OR, PUSH/POP CS. */
		virtual void onOpcode0X(uint8_t opcode);

		/* 0x10 ~ 0x1F opcode series (ADC, PUSH/POP SS, SBB, PUSH/POP DS. */
		virtual void onOpcode1X(uint8_t opcode);

		/* 0x20 ~ 0x2F opcode series (AND, DAA, SUB, DAS). */
		virtual void onOpcode2X(uint8_t opcode);

		/* 0x30 ~ 0x3F opcode series (XOR, AAA, CMP, AAS). */
		virtual void onOpcode3X(uint8_t opcode);

		/* 0x40 ~ 0x4F opcode series (INC/DEC). */
		virtual void onOpcode4X(uint8_t opcode);

		/* 0x50 ~ 0x5F opcode series (PUSH/POP GPR). */
		virtual void onOpcode5X(uint8_t opcode);

		/* 0x60 ~ 0x6F opcode series (PUSHA, POPA, BOUND, PUSH Iv, IMUL, PUSB Ib, INSB, INSW, OUTSB, OUTSW) */
		virtual void onOpcode6X(uint8_t opcode);

		/* 0x70 ~ 0x7F opcode series (JMP series). */
		virtual void onOpcode7X(uint8_t opcode);
	};

}

#endif // __V86_CPU_I8086_H__