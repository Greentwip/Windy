#include <memory>
#include <algorithm>

#include "core/geometry/rectangle.hpp"

std::pair<bool, std::shared_ptr<nana::rectangle> >
	windy::rectangle::intersects(const nana::rectangle& a, const nana::rectangle& b) {
	auto intersection_result = std::pair<bool, std::shared_ptr<nana::rectangle> >();

	intersection_result.first = !(a.right()		< b.x			||
									b.right()		< a.x			||
									a.bottom()	< b.y			||
									b.bottom()	< a.y);

	if (intersection_result.first) {
		auto distance_x = std::abs(a.x - b.x);
		auto distance_y = std::abs(a.y - b.y);

		auto intersection = std::make_shared<nana::rectangle>();
					
		intersection->x			= std::max(a.x, b.x);
		intersection->y			= std::max(a.y, b.y);
		intersection->width		= std::max(0,
											std::min(a.right(), 
													b.right()) - intersection->x);

		intersection->height	= std::max(0,
											std::min(a.bottom(), 
													b.bottom()) - intersection->y);

		intersection_result.second = intersection;

	} else {
		intersection_result.second = nullptr;
	}

	return intersection_result;

}

nana::rectangle windy::rectangle::merge(const nana::rectangle& first, const nana::rectangle& second) {
	auto x = std::min(first.x, second.x);
	auto y = std::min(first.y, second.y);
	auto r = std::max(first.right(), second.right());
	auto b = std::max(first.bottom(), second.bottom());

	return nana::rectangle(x, y, r - x, b - y);
}
