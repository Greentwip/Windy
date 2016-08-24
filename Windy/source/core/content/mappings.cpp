#include <sstream>

#include "core/content/mappings.hpp"

#include "core/content/instance.hpp"

windy::content::mappings::mappings() {

}

void windy::content::mappings::init() {
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

		std::string content_caption = "\tMappings";
		this->_label.caption(nana::string(content_caption.begin(), content_caption.end()));
		this->_label.bgcolor(nana::colors::dark_gray);
		this->_label.borderless(false);

		this->_content.append_header(L"Name", 80U);
		this->_content.append_header(L"Value", 100U);

		/*this->_content.at(0).append({ L"  Anchor X", nana::string() });
		this->_content.at(0).append({ L"  Anchor Y", nana::string() });
		this->_content.at(0).append({ L"  X",		 nana::string() });
		this->_content.at(0).append({ L"  Y",		 nana::string() });
		this->_content.at(0).append({ L"  Width",	 nana::string() });
		this->_content.at(0).append({ L"  Height",	 nana::string() });
		this->_content.at(0).append({ L"  Duration", nana::string() });*/

		this->_content.show_header(false);
		this->_content.enable_single(true, false);

}

void windy::content::mappings::load(std::shared_ptr<class instance> instance) {

	this->_content.at(0).at(0).text(1, std::to_wstring(instance->anchor().x));
	this->_content.at(0).at(1).text(1, std::to_wstring(instance->anchor().y));

	this->_content.at(0).at(2).text(1, std::to_wstring(instance->location().x));
	this->_content.at(0).at(3).text(1, std::to_wstring(instance->location().y));

	this->_content.at(0).at(4).text(1, std::to_wstring(instance->size().width));
	this->_content.at(0).at(5).text(1, std::to_wstring(instance->size().height));

	//this->_content.at(0).at(0).text(1, std::to_wstring(instance->anchor().x));

}

void windy::content::mappings::clear() {

	for (unsigned int i = 0; i < this->_content.size_item(); ++i) {
		this->_content.at(0).at(i).text(1, nana::string());
	}

}