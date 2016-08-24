#include <sstream>

#include "core/content/properties.hpp"

#include "core/content/groupable.hpp"

#include "core/animation/frame_set.hpp"


windy::content::properties::properties() {

}

void windy::content::properties::init() {
		this->_place.bind(*this);

		auto div = std::stringstream();

		div << ""	
				<< "<vertical"
					<< "<weight=16"
						<< "<label>"
					<< ">"
					<< "<"
						<< "<content>"
					<< ">"
				<< ">"
			<< "";

		this->_place.div(div.str().c_str());
					
		this->_label.create(*this);
		this->_content.create(*this);

		this->_place["label"] << this->_label.handle();
		this->_place["content"] << this->_content.handle();

		this->_place.collocate();

		std::string content_caption = "\tProperties";
		this->_label.caption(nana::string(content_caption.begin(), content_caption.end()));
		this->_label.bgcolor(nana::colors::dark_gray);
		this->_label.borderless(false);

		this->_content.append_header(L"Name", 80U);
		this->_content.append_header(L"Value", 100U);

		this->_content.at(0).append({ L"  Anchor X", nana::string() });
		this->_content.at(0).append({ L"  Anchor Y", nana::string() });
		this->_content.at(0).append({ L"  X",		 nana::string() });
		this->_content.at(0).append({ L"  Y",		 nana::string() });
		this->_content.at(0).append({ L"  Width",	 nana::string() });
		this->_content.at(0).append({ L"  Height",	 nana::string() });
		this->_content.at(0).append({ L"  Duration", nana::string() });

		this->_content.show_header(false);
		this->_content.enable_single(true, false);

}

void windy::content::properties::load(std::shared_ptr<instance> instance) {

	// we shall inverse all y values due to the coordinate system

	auto layer = instance->parent();

	auto container = layer->parent();

	auto instance_location = instance->location();
	auto instance_anchor = instance->anchor();
	auto instance_size = instance->size();

	instance_location.y = container->size().height - instance_location.y;

	auto anchor_point_in_points =
		nana::point(instance_size.width * instance_anchor.x, instance_size.height * instance_anchor.y);

	anchor_point_in_points.y = instance_size.height - anchor_point_in_points.y;

	instance_anchor.x = float(anchor_point_in_points.x) / float(instance_size.width);
	instance_anchor.y = float(anchor_point_in_points.y) / float(instance_size.height);

	this->_content.at(0).at(0).text(1, std::to_wstring(instance_anchor.x));
	this->_content.at(0).at(1).text(1, std::to_wstring(instance_anchor.y));

	this->_content.at(0).at(2).text(1, std::to_wstring(instance_location.x));
	this->_content.at(0).at(3).text(1, std::to_wstring(instance_location.y));

	this->_content.at(0).at(4).text(1, std::to_wstring(instance->size().width));
	this->_content.at(0).at(5).text(1, std::to_wstring(instance->size().height));

	std::wstring duration = L"static";

	switch (instance->kind()) {
		case layer::kind::groupable:{
			auto groupable = 
				std::dynamic_pointer_cast<content::groupable>(instance);

			auto frames_size = groupable->frame_set()->frames().size();

			auto seconds_duration = 1.0f / 60.0f * frames_size;

			duration = std::to_wstring(seconds_duration);
		};
	}

	this->_content.at(0).at(6).text(1, duration);

}

void windy::content::properties::clear() {

	for (unsigned int i = 0; i < this->_content.size_item(); ++i) {
		this->_content.at(0).at(i).text(1, nana::string());
	}

}
