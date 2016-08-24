#include <sstream>
#include <algorithm>

#include "core/content/assets.hpp"

#include "nana/gui/msgbox.hpp"


#include "core/2d/layer.hpp"
#include "core/2d/texture.hpp"

#include "core/content/directory.hpp"
#include "core/content/collection.hpp"
#include "core/content/item.hpp"
#include "core/content/instance_creator.hpp"

#include "core/content/group.hpp"
#include "core/content/sprite.hpp"

#include "core/filesystem/path.hpp"

#include "core/number/uuid.hpp"

windy::content::assets::assets() {

}

void windy::content::assets::init() {
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

		std::string content_caption = "\tAssets";
		this->_label.caption(nana::string(content_caption.begin(), content_caption.end()));
		this->_label.bgcolor(nana::colors::black);
		this->_label.fgcolor(nana::colors::white);

		this->_content.append_header(L"");
		this->_content.append(L"Backgrounds");
		this->_content.append(L"Animations");
		this->_content.append(L"Compositions");

		this->_content.show_header(false);

		this->_content.enable_dropfiles(false);

		this->_content.events().mouse_dropfiles(std::bind(&assets::on_mouse_drop_files, 
															this, 
															std::placeholders::_1));

		this->_content.events().mouse_down(std::bind(&assets::on_mouse_down, this, std::placeholders::_1));
		this->_content.events().mouse_up(std::bind(&assets::on_mouse_up, this, std::placeholders::_1));
		this->_content.events().mouse_move(std::bind(&assets::on_mouse_move, this, std::placeholders::_1));


		this->_content.events().selected(std::bind(&assets::on_item_selected,
													this, 
													std::placeholders::_1));


		this->_place["label"]	<< this->_label.handle();
		this->_place["content"] << this->_content.handle();
		this->_place.collocate();
}

void windy::content::assets::construct(std::shared_ptr<windy::content::directory> directory,
									   std::shared_ptr<windy::content::collection> collection, 
									   std::shared_ptr<windy::content::instance_creator> instance_creator) {
				
	this->_directory	= directory;
	this->_collection	= collection;
	this->_instance_creator = instance_creator;

	this->refresh();

	this->_content.enable_dropfiles(true);
}

auto windy::content::assets::directory() {
	return this->_directory;
}

void windy::content::assets::refresh() {

	this->_content.clear();

	for (unsigned int i = 0; i <= layer::kind::compositable; ++i) {
		auto vector = this->_collection->get(layer::kind(i));
		for (auto item : vector) {
			this->_content.at(i + 1).append(item->name());
		}
					
	}

	this->_instance_creator->items().clear();

}

void windy::content::assets::on_item_selected(const nana::arg_listbox& arg) {
	// cancel the dragging operation
	this->_instance_creator->dragging(false);
	this->_instance_creator->hide();

	auto item_text = arg.item.text(0);
	auto item_name = std::string(item_text.begin(), item_text.end());
	auto item_category = layer::kind(arg.item.pos().cat - 1);


	std::vector<std::shared_ptr<class item> > collection_vector =
		this->_collection->get(layer::kind(item_category));


	auto it = std::find_if(collection_vector.begin(), collection_vector.end(),
		[&item_name](auto a) {
		if (a->name() == item_name) {
			return true;
		}
		else {
			return false;
		}
	});


	if (arg.item.selected()) {
		switch (item_category) {
			case layer::kind::graphicable:
				this->_content.at(layer::kind::groupable + 1).select(false);
				this->_content.at(layer::kind::compositable + 1).select(false);
			break;

			case layer::kind::groupable:
				this->_content.at(layer::kind::graphicable + 1).select(false);
				this->_content.at(layer::kind::compositable + 1).select(false);
			break;

			case layer::kind::compositable:
				this->_content.at(layer::kind::graphicable + 1).select(false);
				this->_content.at(layer::kind::groupable + 1).select(false);
			break;

		}

		this->_instance_creator->category(item_category);
		this->_instance_creator->items().push_back((*it));

	} else {

		auto& items = this->_instance_creator->items();

		auto new_it = std::find(items.begin(), items.end(), (*it));

		if (new_it != items.end()) {
			items.erase(new_it);
		}
	}

	/*if (arg.item.selected()) {
		if (item_name != this->_current_item) {
			this->_current_item = item_name;
			//this->_on_item_selected_callback(item_name);
		}
	} else {

		if (this->_current_item == item_name) {
			arg.item.select(true);
		}

	}*/

}

void windy::content::assets::on_mouse_down(const nana::arg_mouse& arg) {

	if (arg.button == nana::mouse::left_button) {
		if (this->_instance_creator->visible() &&
			this->_instance_creator->dragging()) {

			this->_instance_creator->hide();
			this->_instance_creator->dragging(false);
			this->_instance_creator->events().mouse_down.emit(arg);
			this->events().mouse_up.emit(arg);

			// happens on focus lost, like alt + tab
			/*
				mouse_down event emits twice even if the mouse position is
				outside the content rectangle

				therefore, mouse position validation via is_hit
			*/
			auto position = nana::API::window_position(this->parent());

			position += this->pos() + arg.pos;

			this->_instance_creator->bake_instance(position);

		} else {

			if (this->_instance_creator->items().size() != 0) { // verify selection

				auto parent_position = nana::API::window_position(this->parent());
				auto mouse_position = parent_position + this->pos() + arg.pos;

				auto content_position = parent_position + this->pos();

				auto content_size = this->_content.size();

				auto content_rectangle = nana::rectangle(content_position.x,
															content_position.y, 
															content_size.width, 
															content_size.height);

				// mouse position validation via is_hit
				if (content_rectangle.is_hit(mouse_position)
					&& !this->_content.empty()) {
					this->_instance_creator->move(mouse_position);
					this->_instance_creator->show();
					this->_instance_creator->dragging(true);
					this->_instance_creator->events().mouse_down.emit(arg);
				}

			}

		}

	}

}

void windy::content::assets::on_mouse_move(const nana::arg_mouse& arg) {
	if (this->_instance_creator->dragging() &&
		this->_instance_creator->visible()) {

		auto argument = arg;

		argument.left_button = true;	// to trigger the mouse_move dragger event

		this->_instance_creator->events().mouse_move.emit(argument);

	}
}

void windy::content::assets::on_mouse_up(const nana::arg_mouse& arg) {

	if (arg.button == nana::mouse::left_button) {
		if (this->_instance_creator->visible() &&
			this->_instance_creator->dragging()) {

			this->_instance_creator->dragging(false);
			this->_instance_creator->hide();
			this->_instance_creator->events().mouse_up.emit(arg);

			auto position = nana::API::window_position(this->parent());

			position += this->pos() + arg.pos;

			this->_instance_creator->bake_instance(position);

		}
	}
				
}

void windy::content::assets::on_mouse_drop_files(const nana::arg_dropfiles& arg) {

	std::vector<std::string> files;

	bool valid_extension = true;

	for (unsigned int i = 0; i < arg.files.size() && valid_extension; ++i) {
		auto file = arg.files[i];

		files.push_back({ file.begin(), file.end() });
		if (path::extension(files.back()) != ".png") {
			valid_extension = false;
		}

	}

	if (valid_extension) {

		this->import(files);

	} else {

		nana::msgbox mb(this->parent(), L"Invalid input");
		mb << L"Consider the extensions to be png.";
		mb.show();

	}

}

void windy::content::assets::import(const std::vector<std::string>& files) {


	auto parent = reinterpret_cast<nana::group*> (this->parent());

	nana::inputbox inbox(parent->parent(),
							L"Please select the import kind:",
							L"Import...");

	nana::inputbox::text name(L"<bold blue>Name</>");


	nana::inputbox::text options(L"Kind", { L"Background", L"Animation" });

	import_kind kind;

	bool success = true;

	inbox.verify([this, &name, &options, &kind, &success](nana::window handle) {

		success = true;

		auto import_value = options.value();

		if (name.value().empty()) {
			nana::msgbox mb(handle, L"Invalid input");
			mb << L"Name must be specified.";
			mb.show();
			success = false; //verification failed
		}

		if (import_value == L"Background") {
			kind = import_kind::background;
		} else {
			kind = import_kind::animation;
		}

		return success;
	});

	auto result = inbox.show(name, options);

	if (result) {
		if (this->verify_import(files, kind)) {
			auto name_value = name.value();

			auto import_name = std::string(name_value.begin(), 
											name_value.end());

			this->import(files, import_name, kind);
		}
	}

}

bool windy::content::assets::verify_import(const std::vector<std::string>& files, const import_kind& kind) {
	bool success = true;

	switch (kind) {
		case import_kind::background:
			if (files.size() > 1) {
				nana::msgbox mb(this->parent(), L"Invalid input");
				mb << L"Backgrounds consist in single files only.";
				mb.show();
				success = false; //verification failed
			}
		break;
	}

	return success;
}

void windy::content::assets::import(const std::vector<std::string>& files,
			const std::string& name,
			const import_kind& kind) {
				
	switch (kind) {
		case import_kind::background: {
			bool copy_success = this->_directory->store(files.back(),
														"Backgrounds",
														name,
														"png");
			if (copy_success) {

				auto sprite_path = 
					this->_directory->retrieve("Backgrounds", name, "png");
							
				auto sprite = 
					std::make_shared<class sprite>(name, 
													uuid::generate(), 
													sprite_path);

				this->_collection->add_sprite(sprite);

			} else {
				nana::msgbox mb(this->parent(), L"Invalid input");
				mb << L"Another background with the same name exists.";
				mb.show();
			}
		}
		break;

		case import_kind::animation: {

			// verify existing animation

			bool creation_success = this->_directory->make("Animations", name);
			bool sprite_copy_success = true;
			bool sprite_copy_error = false;

			auto group = std::make_shared<content::group>(name, uuid::generate());
			std::vector<std::pair<std::string, std::shared_ptr<entity::texture> > > animation_textures;

			unsigned int animation_separation = 0;

			if (creation_success) {
				auto parent = reinterpret_cast<nana::group*> (this->parent());

				nana::inputbox inbox(parent->parent(),
										L"Specify the frames in between each texture:",
										L"Import...");

				nana::inputbox::integer frames(L"Separation:", 0, 0, 60, 1);

				if (inbox.show(frames)) {

					animation_separation = frames.value();
					for (unsigned int i = 0; i < files.size() && !sprite_copy_error; ++i) {

						std::string frame_name = name + "_" + std::to_string(i);
						auto file = files[i];

							sprite_copy_success = this->_directory->store(file,
																	"Animations",
																	name + "/" + frame_name,
																	"png");
						if (sprite_copy_success) {

							auto sprite_path = 
								this->_directory->retrieve("Animations", name + "/" + frame_name, "png");
							
							std::pair<std::string, std::shared_ptr<entity::texture> > texture_pair;

							texture_pair.first = sprite_path;
							texture_pair.second = nullptr;

							animation_textures.push_back(texture_pair);

						} else {
							nana::msgbox mb(this->parent(), L"Error");
							mb << L"Failed to copy: " + std::wstring(file.begin(), file.end());
							mb.show();
							sprite_copy_error = true;
						}

					}

				}

			} else {
				nana::msgbox mb(this->parent(), L"Invalid input");
				mb << L"Another animation with the same name exists.";
				mb.show();
				sprite_copy_error = true;
			}

			if (sprite_copy_error) {
				this->_directory->remove_all("Animations", name);
			} else {
				group->textures(animation_textures);
				group->separation(animation_separation);
				this->_collection->add_group(group);
			}
						
		}
		break;
	}

	this->refresh();
}


void windy::content::assets::remove_sprite(std::shared_ptr<content::sprite> sprite) {
	this->_collection->remove_sprite(sprite);
	this->_directory->remove("Backgrounds", sprite->name(), "png");
}

void windy::content::assets::remove_group(std::shared_ptr<content::group> group) {
	this->_collection->remove_group(group);
	this->_directory->remove_all("Animations", group->name());
}

void windy::content::assets::remove_composition(std::shared_ptr<content::composition> composition) {
	this->_collection->remove_composition(composition);
}
			
std::shared_ptr<class windy::content::collection> windy::content::assets::collection() {
	return this->_collection;
}
