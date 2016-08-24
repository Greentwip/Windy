#ifndef windy_movement_command
#define windy_movement_command

#include <string>
#include <vector>
#include <memory>

#include "nana/basic_types.hpp"

namespace windy{
	class movement_command {
	public:
		movement_command(const std::string& name);

		std::string name();

		void add_target(std::shared_ptr<class drawable> new_target);

		virtual void update(const nana::point& new_location) = 0;

	protected:
		std::vector<std::shared_ptr<class windy::drawable> >& targets();

	private:
		std::string _name;
		std::vector<std::shared_ptr<class drawable> > _targets;

	};
}

#endif