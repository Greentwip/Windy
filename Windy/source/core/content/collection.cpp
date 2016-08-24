#include <cassert>
#include <algorithm>

#include "core/content/collection.hpp"

#include "core/content/sprite.hpp"
#include "core/content/group.hpp"
#include "core/content/composition.hpp"

#include "core/content/instance.hpp"

void windy::content::collection::add_sprite(std::shared_ptr<class sprite> sprite) {
	this->_sprites.push_back(sprite);
}

void windy::content::collection::add_group(std::shared_ptr<class group> group) {
	this->_groups.push_back(group);
}

void windy::content::collection::add_composition(std::shared_ptr<class composition> composition) {
	this->_compositions.push_back(composition);
}

void windy::content::collection::remove_sprite(std::shared_ptr<class sprite> sprite) {
	auto it = std::find(this->_sprites.begin(), this->_sprites.end(), sprite);

	this->_sprites.erase(it);
}

void windy::content::collection::remove_group(std::shared_ptr<class group> group) {
	auto it = std::find(this->_groups.begin(), this->_groups.end(), group);

	this->_groups.erase(it);
}
void windy::content::collection::remove_composition(std::shared_ptr<class composition> composition) {
	auto it = std::find(this->_compositions.begin(),
						this->_compositions.end(),
						composition);

	this->_compositions.erase(it);
}

void windy::content::collection::remove(std::shared_ptr<class instance> instance) {
				
	auto search = [](const std::vector<std::shared_ptr<class item> >& vector, 
						std::shared_ptr<class instance> instance) {

		auto predicate = [](const std::string& guid) {
			auto closure_string = std::make_shared<std::string>(guid);
			return [closure_string](auto a) {
				if (a->uuid() == *closure_string) {
					return true;
				} else {
					return false;
				}
			};
		};

		auto iterator = std::find_if(vector.begin(),
										vector.end(), 
										predicate(instance->asset_uuid()));

		return iterator;
	};
				 
	auto vector = this->get(instance->kind());
	auto iterator = search(vector, instance);
	if (iterator != vector.end()) {
		(*iterator)->remove_instance(instance);
	} else {
		assert(false); // something really wrong could happen
	}

}

std::vector<std::shared_ptr<class windy::content::item> >& windy::content::collection::get(layer::kind kind) {
	switch (kind) {
	case layer::kind::graphicable:
		return this->_sprites;
	break;
	case layer::kind::groupable:
		return this->_groups;
	break;

	case layer::kind::compositable:
		return this->_compositions;
	break;

	default: // none is assert error (should be), @TODO check this
		return this->_stub;
	break;
	}
}