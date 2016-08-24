#include "core/animation/frame_set.hpp"

#include "core/animation/frame.hpp"

#include "core/number/digits.hpp"

windy::animation::frame_set::frame_set(std::shared_ptr<frame_set> frame_set) {
	this->_duration = frame_set->_duration;
	this->_timeline_frames = frame_set->_timeline_frames;

	this->_current_frame_index = 0;
	this->_current_key_frame_index = 0;

	this->_counter = 0;

	this->current_frame_index(this->_current_frame_index);
}

windy::animation::frame_set::frame_set(const uint64_t& duration)  {
	this->reset(duration);
}

void windy::animation::frame_set::reset(const uint64_t& duration) {

	this->_duration = duration;
	this->_current_frame_index = 0;
	this->_current_key_frame_index = 0;

	this->_timeline_frames.clear();


	for (uint32_t i = 0; i < this->_duration; ++i) {
		this->_timeline_frames.push_back(std::make_shared<frame>());
	}

	this->enable_key_frame(current_key_frame_index());
	this->current_frame_index(this->_current_frame_index);

}

void windy::animation::frame_set::swap_key_frame(const uint64_t& index) {
	if (this->_timeline_frames[index]->is_key_frame()) {
		this->disable_key_frame(index);
	} else {
		this->enable_key_frame(index);
	}
}

void windy::animation::frame_set::enable_key_frame(const uint64_t& index) {

	if (index == this->_current_frame_index) {
		this->_current_key_frame_index = this->_current_frame_index;
	}

	this->_timeline_frames[index]->is_key_frame(true);
}

void windy::animation::frame_set::disable_key_frame(const uint64_t& index) {
	if (index > 0) { // frame 0 is the only without enable/disable changes
		this->_timeline_frames[index]->is_key_frame(false);
		this->_current_key_frame_index = previous_key_frame_index();
	}
}

const uint64_t windy::animation::frame_set::previous_key_frame_index(bool from_current_key_frame) {

	bool looking = true;
	uint64_t previous_index = 0;

	auto search_index = from_current_key_frame ?
		this->_current_key_frame_index :
		this->_current_frame_index;

	for (uint64_t i = search_index; i > 0 && looking; --i) {
		if (this->_timeline_frames[i]->is_key_frame()) {
			previous_index = i;
			looking = false;
		}
	}

	return previous_index;
}

const uint64_t windy::animation::frame_set::current_key_frame_index() {
	return this->_current_key_frame_index;
}

const uint64_t windy::animation::frame_set::ordered_key_frame_index(const uint64_t& key_frame) {
	uint64_t key_Frame_counter = 0;

	//@TODO assert if key_frame is inexistent
	bool searching = true;
	for (uint64_t i = 0; i < this->_timeline_frames.size() && searching; ++i) {
		if (this->_timeline_frames[i]->is_key_frame()) {

			if (i == key_frame) {
				searching = false;
			} else {
				key_Frame_counter++;
			}

		}

	}

	return key_Frame_counter;
}


void windy::animation::frame_set::current_frame_index(const uint64_t& index) {
	this->_current_frame_index = index;

	if (this->_timeline_frames[_current_frame_index]->is_key_frame()) {
		this->_current_key_frame_index = this->_current_frame_index;
	}
	else {
		this->_current_key_frame_index = previous_key_frame_index(false);
	}
}

void windy::animation::frame_set::current_key_frame_index(const uint64_t& index) {
	this->_current_key_frame_index = index;
}

std::vector<std::shared_ptr<windy::animation::frame> >& windy::animation::frame_set::frames() {
	return _timeline_frames;
}

uint64_t windy::animation::frame_set::current_frame_index() {
	return this->_current_frame_index;
}

std::shared_ptr<class windy::animation::frame> windy::animation::frame_set::current_frame() {
	return this->_timeline_frames[this->_current_frame_index];
}

uint64_t windy::animation::frame_set::duration() {
	return this->_duration;
}

void windy::animation::frame_set::reset_counter(const float& new_counter) {
	this->_counter = new_counter;

	auto idx = windy::digits::clamp(static_cast<uint64_t>(std::round(new_counter * 60)),
									static_cast<uint64_t>(0),
									static_cast<uint64_t>(this->_timeline_frames.size()));


	if (idx == this->_timeline_frames.size()) {

		this->_counter = 0;
		idx = 0;

	} 

	this->current_frame_index(idx);
}

void windy::animation::frame_set::stop() {
	this->_counter = 0;
}

void windy::animation::frame_set::update(const float& delta) {

	auto idx = windy::digits::clamp(static_cast<uint64_t>(std::round(this->_counter * 60)),
									static_cast<uint64_t>(0),
									static_cast<uint64_t>(this->_timeline_frames.size()));


	if (idx == this->_timeline_frames.size()) {

		this->_counter = 0;
		idx = 0;

	} 

	this->current_frame_index(idx);

	this->_counter += delta;

}
