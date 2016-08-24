#include "core/content/sprite.hpp"

#include "core/content/instance.hpp"
#include "core/content/graphicable.hpp"

#include "core/2d/texture.hpp"

windy::content::sprite::sprite(const std::string& name,
		const std::string& uuid, 
		const std::string& path) : 
	item(name, uuid){
	this->_path = path;
}

std::string windy::content::sprite::path() {
	return this->_path;
}

std::shared_ptr<windy::content::instance> windy::content::sprite::build() {

	auto instance = std::make_shared<graphicable>();

	if (this->_texture == nullptr) {
		this->_texture = std::make_shared<entity::texture>();
		this->_texture->load(this->_path);
	}

	instance->texture(this->_texture);

	return instance;
}

void windy::content::sprite::remove(std::shared_ptr<class instance> instance) {

	if (this->_instances.empty()) {
		this->_texture = nullptr;
	}

}
