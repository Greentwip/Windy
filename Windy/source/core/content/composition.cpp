#include <algorithm>

#include "core/content/composition.hpp"

#include "core/content/compositable.hpp"
#include "core/content/groupable.hpp"

windy::content::composition::composition(const std::string& name, const std::string& uuid) :
	item(name, uuid) {

}

std::shared_ptr<class windy::content::instance> windy::content::composition::build() {
				
	auto instance = std::make_shared<compositable>();

	for (auto item_pair : this->_maps) {

		auto groupable = item_pair.second->create_instance();
		instance->add_child(groupable);

	}

	return instance;
}

void windy::content::composition::remove(std::shared_ptr<class instance> instance) {

	for (auto child : instance->children()) {
		auto groupable = std::dynamic_pointer_cast<class groupable>(child);
					
		for (auto item_pair : this->_maps) {
			if (item_pair.second->uuid() == groupable->asset_uuid()) {
				item_pair.second->remove_instance(groupable);
			}
						
		}
	}
}

void windy::content::composition::map(const std::pair<std::string, std::shared_ptr<class item> > map) {
	this->_maps.push_back(map);
}

auto windy::content::composition::map(const std::string& name) {
	return 0; // { 0 };
}

void windy::content::composition::unmap(std::shared_ptr<class item> item) {
	auto it = std::find_if(this->_maps.begin(), this->_maps.end(), 
		[item](auto a) {

		if (a.second == item) {
			return true;
		} else {
			return false;
		}

	});

	this->_maps.erase(it);
}

void windy::content::composition::maps(const std::vector<item_pair>& maps) {
	this->_maps = maps;
}

std::vector<windy::content::composition::item_pair>& windy::content::composition::maps() {
	return this->_maps;
}
