#ifndef __V86_CPU_MEMORY_H__
#define __V86_CPU_MEMORY_H__
#include "../types.h"

namespace v86 {
	class IMemory {
	public:
		virtual ~IMemory() { }

	public:
		/* read memory to the buffer. */
		virtual uint32_t read(uint32_t addr, void* buf, uint32_t size) = 0;

		/* write memory from the buffer. */
		virtual uint32_t write(uint32_t addr, const void* buf, uint32_t size) = 0;
	};
}

#endif // __V86_CPU_MEMORY_H__