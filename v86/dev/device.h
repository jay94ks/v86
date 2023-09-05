#ifndef __V86_DEV_DEVICE_H__
#define __V86_DEV_DEVICE_H__
#include "../types.h"

namespace v86 {
	enum EDEV {
		EDEV_UNKNOWN = 0,
		EDEV_MEMORY,
		EDEV_IOPORT,
	};

	/* device interface. */
	class IDevice : public IRefCounted {
	private:
		EDEV m_Type;

	public:
		IDevice(EDEV Type) : m_Type(Type) { }
		virtual ~IDevice() { }

	public:
		/* get the type of device. */
		inline EDEV getType() const { return m_Type; }
	};
}

#endif // __V86_DEV_DEVICE_H__