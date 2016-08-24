#ifndef windy_drawable
#define windy_drawable

#include "nana/basic_types.hpp"

#include "core/2d/node.hpp"

namespace windy {
	class drawable : public node {
	public:
		drawable();
		nana::rectangle projection_rectangle();
		nana::point anchor_point_in_points(bool scaled = true);
		nana::rectangle projection_anchor();
		nana::rectangle normalize_rectangle(const nana::rectangle& rectangle);
		auto rectangle();
	};
}

#endif