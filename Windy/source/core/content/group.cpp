#include "core/content/group.hpp"

#include "core/content/instance.hpp"

#include "core/content/groupable.hpp"

#include "core/2d/texture.hpp"

#include "core/animation/frame_set.hpp"
#include "core/animation/frame.hpp"


windy::content::group::group(const std::string& name, const std::string& uuid) :
	item(name, uuid) {
	this->_separation = 0;
}

void windy::content::group::textures(const std::vector <std::pair<std::string, std::shared_ptr<entity::texture> > >& textures) {
	this->_textures = textures;
}

std::shared_ptr<class windy::content::instance> windy::content::group::build() {

	auto instance = std::make_shared<groupable>();

	std::vector<std::shared_ptr<entity::texture> > textures;

	for (auto texture_pair : this->_textures) {
		if (texture_pair.second == nullptr) {
			texture_pair.second = std::make_shared<entity::texture>();
			texture_pair.second->load(texture_pair.first);
		}
		textures.push_back(texture_pair.second);
	}

	if (this->_frame_set == nullptr) {
						
		uint64_t frame_set_size = this->_textures.size();

		frame_set_size += (frame_set_size - 1) * this->_separation;

		this->_frame_set = std::make_shared<animation::frame_set>(frame_set_size);

		for (uint32_t i = 0; i < frame_set_size; ++i+=this->_separation) {
			this->_frame_set->frames()[i]->is_key_frame(true);
		}

	}

	instance->frame_set(std::make_shared<animation::frame_set>(this->_frame_set));
	instance->textures(textures);

	return instance;
}

void windy::content::group::remove(std::shared_ptr<class instance> instance) {

	if (this->_instances.empty()) {
		for (auto texture_pair : this->_textures) {
			texture_pair.second = nullptr;
		}
	}
}

void windy::content::group::separation(const unsigned int& separation) {
	this->_separation = separation;
}
