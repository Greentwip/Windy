#include "core/content/compositable.hpp"

#include "core/2d/texture.hpp"

windy::content::compositable::compositable() {
	this->kind(layer::kind::compositable);
}

std::shared_ptr<class windy::entity::texture> windy::content::compositable::texture() {

	auto child = this->children().front();

	auto instance = std::dynamic_pointer_cast<class instance>(child);

	return instance->texture();
}

nana::size windy::content::compositable::size(){
	return this->children().front()->size();
}
