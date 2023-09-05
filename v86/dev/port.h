#ifndef __V86_DEV_IOPORT_H__
#define __V86_DEV_IOPORT_H__
#include "device.h"

namespace v86 {
	class IPort : public IDevice {
	public:
		static constexpr EDEV TYPE = EDEV_IOPORT;

	public:
		IPort() : IDevice(TYPE) { }
		virtual ~IPort() { }

	public:
		/* write a byte to port. */
		virtual bool write(uint16_t port, uint8_t byte) = 0;

		/* read a byte from port. */
		virtual bool read(uint16_t port, uint8_t* byte) = 0;
	};
}

#endif // __V86_DEV_IOPORT_H__