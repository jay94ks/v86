#ifndef __V86_DEV_MEMORY_H__
#define __V86_DEV_MEMORY_H__
#include "device.h"

namespace v86 {
	class IMemory : public IDevice {
	public:
		static constexpr EDEV TYPE = EDEV_IOPORT;

	public:
		IMemory() : IDevice(TYPE) { }
		virtual ~IMemory() { }

	public:
		/* read memory to the buffer. */
		virtual uint32_t read(uint32_t addr, void* buf, uint32_t size) = 0;

		/* write memory from the buffer. */
		virtual uint32_t write(uint32_t addr, const void* buf, uint32_t size) = 0;
	};
}

#endif // __V86_DEV_MEMORY_H__