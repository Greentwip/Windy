#ifndef windy_rectangle
#define windy_rectangle

#include "nana/basic_types.hpp"

namespace windy {
	class rectangle {
	public:
		static nana::rectangle merge(const nana::rectangle& first, const nana::rectangle& second);
		static std::pair<bool, std::shared_ptr<nana::rectangle> >
				intersects(const nana::rectangle& a, const nana::rectangle& b);
	};
}

#endif