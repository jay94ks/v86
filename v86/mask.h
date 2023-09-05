#ifndef __V86_FLAG_H__
#define __V86_FLAG_H__
#include "types.h"

namespace v86 {
	namespace _ {
		/**
		 * mask generator. 
		 * 
		 * n	mask
		 * 0	0000 0000
		 * 1	0000 0001
		 * 2	0000 0011
		 * 3	0000 0111
		 * ..............
		 * 
		 * 16	1111 1111 1111 1111
		 */
		template<uint8_t n = 0>
		struct mask_t {
			static constexpr uint32_t mask 
				= mask_t<n - 1>::mask | (1 << (n - 1));
		};

		template<> struct mask_t<0> {
			static constexpr uint32_t mask = 0;
		};
	}

	template<uint8_t n, uint8_t sz = 1>
	constexpr uint32_t mask() {
		return _::mask_t<sz>::mask << n;
	}
}

#endif