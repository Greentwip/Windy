#include "core/content/edition.hpp"

#include "core/content/container.hpp"

windy::content::edition::edition() {

}

void windy::content::edition::init() {
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

		std::string content_caption = "\tEdition";
		this->_label.caption(nana::string(content_caption.begin(), content_caption.end()));
		this->_label.bgcolor(nana::colors::dark_gray);

		this->_content.append_header(L"");

		this->_content.append(L"Background");
		this->_content.append(L"Animation");
		this->_content.append(L"Composition");

		this->_content.show_header(false);
		this->_content.enable_single(true, false);

		this->_place["label"]	<< this->_label.handle();
		this->_place["content"] << this->_content.handle();

		this->_place.collocate();
}

void windy::content::edition::refresh(std::shared_ptr<class container> container) {
	this->clear();

	for (unsigned int i = 0; i <= layer::kind::compositable; ++i) {
		for (auto item : container->get(layer::kind(i))) {
			this->_content.at(i + 1).append(item->name());
		}

	}
}

void windy::content::edition::clear() {
	this->_content.clear();
}

	