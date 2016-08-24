#include "core/animation/trigger.hpp"

windy::animation::trigger::trigger(kind kind,
		uint64_t frame,
		std::shared_ptr<class frame_set> frame_set) {
	this->_playing = false;
	this->_kind = kind;
	this->_frame = frame;
	this->_frame_set = frame_set;
}

windy::animation::trigger::kind windy::animation::trigger::get_kind() {
	return this->_kind;
}

void windy::animation::trigger::set_kind(windy::animation::trigger::kind kind) {
	this->_kind = kind;
}

uint64_t windy::animation::trigger::frame() {
	return this->_frame;
}

void windy::animation::trigger::playing(const bool& playing) {
	this->_playing = playing;
}

bool windy::animation::trigger::playing() {
	return this->_playing;
}

std::shared_ptr<windy::animation::frame_set> windy::animation::trigger::frame_set() {
	return this->_frame_set;
}

std::shared_ptr<windy::animation::trigger> windy::animation::trigger::previous() {
	return this->_previous;
}

void windy::animation::trigger::previous(std::shared_ptr<trigger> previous) {
	this->_previous = previous;
}

std::shared_ptr<windy::animation::trigger> windy::animation::trigger::next() {
	return this->_next;
}

void windy::animation::trigger::next(std::shared_ptr<trigger> next) {
	this->_next = next;
}
