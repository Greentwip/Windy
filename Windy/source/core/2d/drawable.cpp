#include "core/2d/drawable.hpp"

windy::drawable::drawable() {
}

nana::rectangle windy::drawable::projection_rectangle() {
	auto projection_location = this->location(windy::node::location_kind::global);

	auto width = this->size().width;
	auto height = this->size().height;

	auto projection_width  = unsigned int(width * this->_scale);
	auto projection_height = unsigned int(height * this->_scale);

	auto x_offset = this->_zoom_focus.x - this->_zoom_focus.x * this->_scale;
	auto y_offset = this->_zoom_focus.y - this->_zoom_focus.y * this->_scale;

	auto projection_x = int(projection_location.x * this->_scale + x_offset);
	auto projection_y = int(projection_location.y * this->_scale + y_offset);
	auto projection = nana::rectangle(projection_x,
										projection_y,
										projection_width,
										projection_height);

	auto anchor_point_in_points = this->anchor_point_in_points();

	auto anchor_is_zero = anchor_point_in_points.x == 0 && anchor_point_in_points.y == 0;

	// calculate real position
	if (!anchor_is_zero)
	{
		projection.x += -anchor_point_in_points.x;
		projection.y += -anchor_point_in_points.y;
	}

	return projection;
}

nana::point windy::drawable::anchor_point_in_points(bool scaled) {
	auto anchor = this->anchor();

	auto anchor_point_in_points =
		nana::point(this->size().width * anchor.x, 
					this->size().height * anchor.y);


	nana::point projection_anchor = anchor_point_in_points;

	if (scaled) {
		projection_anchor.x *= this->scale();
		projection_anchor.y *= this->scale();
	}

	return projection_anchor;
}

nana::rectangle windy::drawable::projection_anchor() {

	auto projection_anchor = this->anchor_point_in_points();

	auto entity_rectangle = this->projection_rectangle();

	auto anchor_rectangle = entity_rectangle;

	anchor_rectangle.x += projection_anchor.x;
	anchor_rectangle.y += projection_anchor.y;

	anchor_rectangle.width = 12;   
	anchor_rectangle.height = 12;

	anchor_rectangle.x -= anchor_rectangle.width * 0.5;
	anchor_rectangle.y -= anchor_rectangle.height * 0.5;

	return anchor_rectangle;

}

nana::rectangle windy::drawable::normalize_rectangle(const nana::rectangle& rectangle) {
	// be aware that normalize_rectangle may return width == 0 and height == 0
	// for width == 0 and height == 0, leaving unrendered is the best
	auto normalized_rectangle = rectangle;

	normalized_rectangle.x = int(normalized_rectangle.x / this->_scale);
	normalized_rectangle.y = int(normalized_rectangle.y / this->_scale);

	normalized_rectangle.width = 
		unsigned int(normalized_rectangle.width / this->_scale);

	normalized_rectangle.height = 
		unsigned int(normalized_rectangle.height / this->_scale);

	return normalized_rectangle;
}

auto windy::drawable::rectangle() {
	return nana::rectangle(this->_location.x, this->_location.y, this->_size.width, this->_size.height);
}
