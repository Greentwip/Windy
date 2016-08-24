#include "core/animation/frame.hpp"

windy::animation::frame::frame() :
	_is_key_frame(false) {
}

bool windy::animation::frame::is_key_frame() {
	return this->_is_key_frame;
}

void windy::animation::frame::is_key_frame(const bool& is_key_frame) {
	this->_is_key_frame = is_key_frame;
}

