#ifndef __V86_TYPES_H__
#define __V86_TYPES_H__
#include <stdint.h>

namespace v86 {
	using uint8_t = ::uint8_t;
	using uint16_t = ::uint16_t;
	using uint32_t = ::uint32_t;
	using uint64_t = ::uint64_t;

	using int8_t = ::int8_t;
	using int16_t = ::int16_t;
	using int32_t = ::int32_t;
	using int64_t = ::int64_t;

	using nullptr_t = decltype(nullptr);

	/* reference counted interface. */
	class IRefCounted {
	private:
		int32_t m_Refs;

	public:
		IRefCounted() : m_Refs(1) { }
		virtual ~IRefCounted() { }

	public:
		inline void grab() { m_Refs++; }
		virtual bool drop() {
			m_Refs--;

			if (m_Refs == 0) {
				delete this;
				return true;
			}

			return false;
		}
	};
}

#endif // __V86_TYPES_H__