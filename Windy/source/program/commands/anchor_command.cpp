#include "program/commands/anchor_command.hpp"

#include "core/2d/drawable.hpp"

windy::anchor_command::anchor_command(const std::string& name) : movement_command(name) {

}

void windy::anchor_command::update(const nana::point& new_location) {

	auto target = this->targets().front();

	nana::point difference(new_location.x - this->_drag_location.x,
		new_location.y - this->_drag_location.y);

	auto previous_projection = target->projection_rectangle();

	auto previous_apip_no_scale = target->anchor_point_in_points(false);
	auto previous_apip = target->anchor_point_in_points();

	float x_diff = 0;
	float y_diff = 0;

	if (difference.x > 0) {
		x_diff = 0.015625f;
	}
	else if (difference.x < 0) {
		x_diff = -0.015625f;
	}

	if (difference.y > 0) {
		y_diff = 0.015625f;
	}
	else if (difference.y < 0) {
		y_diff = -0.015625f;
	}

	auto anchor = target->anchor();

	anchor.x += x_diff;
	anchor.y += y_diff;

	target->anchor(anchor);

	auto new_apip_no_scale = target->anchor_point_in_points(false);
	auto new_apip = target->anchor_point_in_points();

	auto location_difference = nana::point();

	location_difference.x = new_apip_no_scale.x - previous_apip_no_scale.x;
	location_difference.y = new_apip_no_scale.y - previous_apip_no_scale.y;

	auto location = target->location();

	location += location_difference;

	target->location(location);

	this->_drag_location.x = new_location.x;
	this->_drag_location.y = new_location.y;

}
 