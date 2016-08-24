#include "core/content/groupable.hpp"

#include "core/2d/texture.hpp"

#include "core/animation/frame_set.hpp"

windy::content::groupable::groupable() {
	this->kind(layer::kind::groupable);
}

std::shared_ptr<class windy::entity::texture> windy::content::groupable::texture() {
	auto index = this->_frame_set->ordered_key_frame_index(this->_frame_set->current_key_frame_index());
	return this->_textures[index];
}

auto& windy::content::groupable::textures() {
	return this->_textures;
}

void windy::content::groupable::textures(const std::vector<std::shared_ptr<entity::texture> >& textures) {
	this->_textures = textures;
}

void windy::content::groupable::frame_set(std::shared_ptr<animation::frame_set> frame_set) {
	this->_frame_set = frame_set;
}

std::shared_ptr<class windy::animation::frame_set> windy::content::groupable::frame_set(){
	return this->_frame_set;
}

std::vector<std::shared_ptr<class windy::animation::trigger> >& windy::content::groupable::triggers() {
	return this->_triggers;
}

nana::size windy::content::groupable::size(){
	return this->_textures.front()->size();
}
 