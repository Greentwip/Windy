#include <cassert>

#include "core/content/item.hpp"

#include "core/content/instance.hpp"

windy::content::item::item(const std::string& name,
		const std::string& uuid) {
	this->_name = name;
	this->_uuid = uuid;
}

std::shared_ptr<windy::content::instance> windy::content::item::create_instance() {
					
	auto& instances = this->_instances;
	auto name = this->_name;

	name += "_";

	unsigned long higher_instance = 0;

	if (instances.size() > 0) {

		for (auto item_instance : instances) {

			// split the string
			auto item_instance_name = item_instance->name();

			auto iterator = item_instance_name.find_last_of("_");

			if (iterator != std::string::npos) {

				auto instance_number = std::string(item_instance_name.begin() + iterator + 1,
													item_instance_name.end());

				auto instance_number_digit = std::stoul(instance_number);

				if (instance_number_digit >= higher_instance) {
					higher_instance = instance_number_digit;
				}

			}
			else {
				assert(false); // something really bad could happen
			}


		}

	}

	name += std::to_string(++higher_instance);

	auto instance = this->build();

	instance->name(name);
	instance->asset_uuid(this->_uuid);

	this->_instances.push_back(instance);

	return instance;

}

void windy::content::item::remove_instance(std::shared_ptr<class instance> instance) {

	auto iterator = std::find(this->_instances.begin(),
		this->_instances.end(),
		instance);

	this->_instances.erase(iterator);
					
	this->remove(instance);
}

std::string windy::content::item::name() {
	return this->_name;
}

std::string windy::content::item::uuid() {
	return this->_uuid;
}

std::vector<std::shared_ptr<class windy::content::instance> >& windy::content::item::instances() {
	return this->_instances;
}
