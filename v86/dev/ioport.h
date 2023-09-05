#ifndef __V86_DEV_IOPORT_H__
#define __V86_DEV_IOPORT_H__
#include "../types.h"

namespace v86 {
	class IIOPort : public IRefCounted {
	public:
		virtual ~IIOPort() { }

	public:
		/* write a byte to port. */
		virtual bool write(uint16_t port, uint8_t byte) = 0;

		/* read a byte from port. */
		virtual bool read(uint16_t port, uint8_t* byte) = 0;
	};
}

#endif // __V86_DEV_IOPORT_H__