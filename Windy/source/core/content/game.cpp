#include <algorithm>

#include "core/content/game.hpp"

#include "core/content/container.hpp"

windy::content::game::game() {
}

void windy::content::game::init() {
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

		std::string content_caption = "\tGame";
		this->_label.caption(nana::string(content_caption.begin(), content_caption.end()));
		this->_label.bgcolor(nana::colors::black);
		this->_label.fgcolor(nana::colors::white);

		this->_content.append_header(L"");
		this->_content.append(L"Scenes");

		this->_content.show_header(false);
		this->_content.enable_single(true, false);

		this->_place["label"]	<< this->_label.handle();
		this->_place["content"] << this->_content.handle();

		this->_content.events().selected(std::bind(&game::on_item_selected,
													this, 
													std::placeholders::_1));


		this->_place.collocate();
}

void windy::content::game::on_item_selected(const nana::arg_listbox& arg) {

	auto item_value = arg.item.text(0);
	auto item_name = std::string(item_value.begin(), item_value.end());

	if (arg.item.selected()) {
		if (item_name != this->_current_item) {
			this->_current_item = item_name;
			this->container(this->_current_item);
		}
	}
	else {
		if (this->_current_item == item_name) {
			arg.item.select(true);
		}

	}

}

void windy::content::game::add_container(std::shared_ptr<class container> container) {
	this->_containers.push_back(container);
	this->_content.at(1).append(container->name());
	this->_content.at(1).back().select(true);
}

void windy::content::game::remove_container(std::shared_ptr<class container> container) {

}

//@TODO arrange predicate builders
void windy::content::game::container(const std::string& name) {

	auto string_predicate_builder = [](const std::string& name) {
		auto closure_string = std::make_shared<std::string>(name);
		return [closure_string](auto a) {
			if (a->name() == *closure_string) {
				return true;
			} else {
				return false;
			}
		};
	};

	auto named_container = std::find_if(this->_containers.begin(),
										this->_containers.end(),
										string_predicate_builder(name));

	this->_container = *named_container;
}
			
std::shared_ptr<class windy::content::container> windy::content::game::container() {
	return this->_container;
}
