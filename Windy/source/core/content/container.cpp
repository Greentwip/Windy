#include "core/content/container.hpp"

#include "core/content/instance.hpp"

windy::content::container::container() {
}

void windy::content::container::bake() {	// required
	auto background_layer = std::make_shared<layer>();
	auto animation_layer = std::make_shared<layer>();
	auto composition_layer = std::make_shared<layer>();

	this->add_child(background_layer);
	this->add_child(animation_layer);
	this->add_child(composition_layer);
}

void windy::content::container::add(std::shared_ptr<instance> instance) {
	this->children()[instance->kind()]->add_child(instance);
}

void windy::content::container::remove(std::shared_ptr<instance> instance) {
	this->children()[instance->kind()]->remove_child_by_name(instance->name());
}

std::vector<std::shared_ptr<class windy::node> >& windy::content::container::get(layer::kind kind) {
	return this->children()[kind]->children();
}