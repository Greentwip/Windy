#ifndef windy_drag_command
#define windy_drag_command

#include <string>

#include "nana/basic_types.hpp"

#include "program/commands/movement_command.hpp"

namespace windy {
	class drag_command : public movement_command {
	public:
		drag_command(const std::string& name);

		void update(const nana::point& new_location);

	private:
		nana::point _drag_location;
	};
}

#endif