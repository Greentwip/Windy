#include "program/commands/drag_command.hpp"

#include "core/2d/drawable.hpp"

windy::drag_command::drag_command(const std::string& name) : movement_command(name) {

}

void windy::drag_command::update(const nana::point& new_location) {

	if (this->_drag_location.x == 0 &&
		this->_drag_location.y == 0) {
		this->_drag_location.x = new_location.x;
		this->_drag_location.y = new_location.y;
	}
	else if (this->_drag_location != new_location) {

		nana::point difference(new_location.x - this->_drag_location.x,
			new_location.y - this->_drag_location.y);

		for (auto target : this->targets()) {
			auto target_location = target->location();

			//difference.x /= target->scale();
			//difference.y /= target->scale();

			target_location += difference;

			target->location(target_location);
		}

		this->_drag_location.x = new_location.x;
		this->_drag_location.y = new_location.y;

	}

}
