#ifndef windy_anchor_command
#define windy_anchor_command

#include <string>
#include "nana/basic_types.hpp"

#include "program/commands/movement_command.hpp"

namespace windy {
	class anchor_command : public movement_command {
	public:
		anchor_command(const std::string& name);

		void update(const nana::point& new_location);

	private:
		nana::point _drag_location;

	};
}
#endif