#ifndef __V86_SPTR_H__
#define __V86_SPTR_H__
#include "types.h"

namespace v86 {
	template<typename T>
	class sptr {
	private:
		struct handle {
			int32_t use;
			uint8_t mem[sizeof(T)];
		};

	private:
		handle* m_Ref;

	public:
		sptr(nullptr_t = nullptr) : m_Ref(nullptr) { }
		sptr(sptr&& other) : m_Ref(other.m_Ref) { other.m_Ref = nullptr; }
		sptr(const sptr& other) : m_Ref(other.m_Ref) {
			if (m_Ref) {
				m_Ref->use++;
			}
		}

		~sptr() { reset(); }

	private:
		template<typename ... TArgs>
		sptr(TArgs&& ... args) : m_Ref(new handle()) {
			new (m_Ref->mem) T(static_cast<TArgs&&>(args) ...);
			m_Ref->use = 1;
		}

	public:
		template<typename ... TArgs>
		static sptr create(TArgs&& ... args) {
			return sptr(static_cast<TArgs&&>(args) ...);
		}

	public:
		/* reset this. */
		inline void reset() {
			if (m_Ref) {
				m_Ref->use--;

				if (m_Ref->use == 0) {
					((T*)m_Ref->mem)->~T();
				}

				delete m_Ref;
				m_Ref = nullptr;
			}
		}

		/* reset this. */
		inline sptr& operator =(nullptr_t) {
			reset();
			return *this;
		}

		/* copy from other. */
		inline sptr& operator =(const sptr& other) {
			sptr temp(const_cast<sptr&&>(*this));

			if ((m_Ref = other.m_Ref) != nullptr) {
				m_Ref->use++;
			}

			temp.reset();
			return *this;
		}

		/* swap two pointers. */
		inline sptr& operator =(sptr&& other) {
			handle* prevRef = m_Ref;
			m_Ref = other.m_Ref;
			other.m_Ref = prevRef;
			return *this;
		}

		/* test whether two pointers are same or not. */
		inline bool operator ==(const sptr& other) const {
			return m_Ref == other.m_Ref;
		}

		/* test whether two pointers are different or not. */
		inline bool operator !=(const sptr& other) const {
			return m_Ref != other.m_Ref;
		}

		/* boolean conversion. */
		inline operator bool() const {
			return m_Ref != nullptr;
		}

		/* boolean invert conversion. */
		inline bool operator !() const {
			return m_Ref == nullptr;
		}

		/* pointer-style member access. */
		inline T* operator ->() const {
			return ((T*)m_Ref->mem);
		}

		/* dereference. */
		inline const T& operator *() const {
			return *((T*)m_Ref->mem);
		}
	};
}

#endif // __V86_SPTR_H__