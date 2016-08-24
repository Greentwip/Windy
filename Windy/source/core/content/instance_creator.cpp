#include "core/content/instance_creator.hpp"

#include "core/content/item.hpp"

windy::content::instance_creator::instance_creator(std::function<bool(const nana::point&)> instance_validation_callback,
	std::function<void(std::shared_ptr<instance>, const nana::point&)> on_instance_bake_callback) : 
	_dragging(false), 
	_instance_validation_callback(instance_validation_callback), 
	_instance_bake_callback(on_instance_bake_callback), 
	_category(layer::kind::none){

}

void windy::content::instance_creator::bake_instance(const nana::point& location) {

	if (this->_instance_validation_callback(location)) {

		auto instance_location = location;

		for (auto item : this->_items) {
			auto instance = item->create_instance();
			this->_instance_bake_callback(instance, instance_location);

			instance_location.x += 2;
			instance_location.y += 2;
		}
					

	} else {
		this->dragging(false);
		this->hide();
	}
				
}

void windy::content::instance_creator::dragging(const bool& dragging) {
	this->_dragging = dragging;
}

bool windy::content::instance_creator::dragging() {
	return this->_dragging;
}

void windy::content::instance_creator::items(std::vector<std::shared_ptr<class item> > items) {
	//@TODO assert on nullptr
	this->_items = items;
}

std::vector<std::shared_ptr<windy::content::item> >& windy::content::instance_creator::items() {
	return this->_items;
}

void windy::content::instance_creator::category(layer::kind category) {
	this->_category = category;
}

windy::layer::kind windy::content::instance_creator::category() {
	return this->_category;
}
