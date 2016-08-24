#include "program/commands/movement_command.hpp"

windy::movement_command::movement_command(const std::string& name) {
	this->_name = name;
}

std::string windy::movement_command::name() {
	return this->_name;
}

void windy::movement_command::add_target(std::shared_ptr<drawable> new_target) {
	this->_targets.push_back(new_target);
}

std::vector<std::shared_ptr<class windy::drawable> >& windy::movement_command::targets() {
	return this->_targets;
}
