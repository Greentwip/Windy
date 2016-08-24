#include "core/content/graphicable.hpp"

#include "core/2d/texture.hpp"

windy::content::graphicable::graphicable() {
	this->kind(layer::kind::graphicable);
}

void windy::content::graphicable::texture(std::shared_ptr<entity::texture> texture) {
	this->_texture = texture;
}

std::shared_ptr<class windy::entity::texture> windy::content::graphicable::texture() {
	return this->_texture;
}

nana::size windy::content::graphicable::size() {
	return this->_texture->size();
}
