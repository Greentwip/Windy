#include <algorithm>

#include "app.hpp"

#include "nana/gui/msgbox.hpp"

#include "core/2d/texture.hpp"

#include "core/animation/frame.hpp"
#include "core/animation/frame_set.hpp"
#include "core/animation/trigger.hpp"

#include "core/content/assets.hpp"
#include "core/content/collection.hpp"
#include "core/content/compositable.hpp"
#include "core/content/composition.hpp"
#include "core/content/container.hpp"

#include "core/content/environment.hpp"
#include "core/content/project.hpp"
#include "core/content/properties.hpp"
#include "core/animation/timeline.hpp"

#include "core/content/edition.hpp"
#include "core/content/game.hpp"

#include "core/content/group.hpp"
#include "core/content/groupable.hpp"

#include "core/content/instance_creator.hpp"

#include "core/content/item.hpp"


#include "core/content/sprite.hpp"

#include "core/number/uuid.hpp"
#include "core/number/digits.hpp"

#include "program/commands/movement_command.hpp"


windy::app::app() :
	nana::form(nana::API::make_center(1280, 720),
	nana::appearance(true, true, false, false, true, true, true)), 
	_drawing(*this){

	this->_environment = std::make_shared<content::environment>();


#if defined(NANA_WINDOWS)
	nana::API::window_icon(*this, nana::paint::image(STR("res/windy.ico")));
#else
	nana::API::window_icon(*this, nana::paint::image(STR("res/windy.bmp")));
#endif

	std::string window_title = "Windy";

	this->caption(nana::string(window_title.begin(), window_title.end()));

	this->_menubar.create(*this);
	this->_menubar.bgcolor(nana::colors::silver);

	this->create_menus();

	this->_canvas_stub_widget.create(*this);
	this->_environment->edition()->create(*this);
	this->_environment->project()->create(*this);
	this->_environment->properties()->create(*this);
	this->_environment->timeline()->create(*this);

	this->_instance_creator = 
		std::make_shared<content::instance_creator>
		(std::bind(&app::instance_validation,
					this,
					std::placeholders::_1),
			std::bind(&app::on_instance_bake,
					this,
					std::placeholders::_1,
					std::placeholders::_2));

	this->_instance_creator->create(*this);
	this->_instance_creator->caption(L"Drag");

	this->_dragger.target(this->_instance_creator->handle());
	this->_dragger.trigger(this->_instance_creator->handle());

	this->prepare_place();

	this->_canvas_stub_widget.hide();

	this->_instance_creator->size(nana::size(60, 15));
	this->_instance_creator->move(nana::point(640, 480));

	this->_instance_creator->hide();
	this->_instance_creator->bgcolor(nana::colors::white);
	this->_instance_creator->fgcolor(nana::colors::black);


	// init groups
	this->_environment->edition()->init();
	this->_environment->project()->init();
	this->_environment->properties()->init();
	this->_environment->timeline()->init();
	
	this->_drawing.draw_diehard(std::bind(&app::on_draw_update,
											this,
											std::placeholders::_1));

	this->_update_timer.elapse(std::bind(&app::update,
											this));

	this->_update_timer.interval(uint32_t(1.00 / 60.00 * 1000)); // milliseconds
	this->_update_timer.start();

	this->_mouse_command_timer.elapse(std::bind(&app::on_mouse_command_update,
												this));

	this->_mouse_command_timer.interval(uint32_t(1.00 / 60.00 * 1000));
	this->_mouse_command_timer.start();

	this->events().key_press(std::bind(&app::on_key_down,
										this,
										std::placeholders::_1));

	this->events().key_release(std::bind(&app::on_key_up,
											this,
											std::placeholders::_1));

	this->events().mouse_down(std::bind(&app::on_mouse_down,
										this, 
										std::placeholders::_1));

	this->events().mouse_up(std::bind(&app::on_mouse_up,
										this, 
										std::placeholders::_1));

	this->events().mouse_move(std::bind(&app::on_mouse_move,
										this, 
										std::placeholders::_1));

	this->events().mouse_wheel(std::bind(&app::on_mouse_wheel,
											this, 
											std::placeholders::_1));

	this->events().dbl_click(std::bind(&app::on_mouse_double_click,
										this,
										std::placeholders::_1));

	this->_application_status = application_status::scene;
}

void windy::app::prepare_place() {
			
	this->_place.bind(*this);

	auto div = std::stringstream();

	div << "<>"
			<< "<weight=100% vertical"
				<< "<weight=4%"
					<< "<weight=100% horizontal"
						<< "<menubar>"
					<< ">"
				<< ">"

				<< "<weight=86%"

					<< "<weight=15% horizontal" 
						<< "<edition>" 
					<< ">"
					<< "<weight=70% horizontal" 
						<< "<vertical"
							<< "<"
								<< "<canvas>"
							<< ">"
						<< ">"
					<< ">"

					<< "<weight=15%"
						<< "<vertical"
							<< "<weight=66%"
								<< "<project>"
							<< ">"
							<< "<"
								<< "<properties>"
							<< ">"
						<< ">"
					<< ">"

				<< ">"

				<< "<weight=10%"
					<< "<timeline>"
				<< ">"

			<< ">"
		<< "<>";

	this->_place.div(div.str().c_str());

	this->_place["canvas"]		<< this->_canvas_stub_widget.handle();
	this->_place["menubar"]		<< this->_menubar.handle();
	this->_place["edition"]		<< this->_environment->edition()->handle();
	this->_place["project"]		<< this->_environment->project()->handle();
	this->_place["properties"]	<< this->_environment->properties()->handle();
	this->_place["timeline"]	<< this->_environment->timeline()->handle();

	this->_place.collocate();

	this->_place["canvas"] << this->_instance_creator->handle(); // consider stack

	this->_place.field_visible("edition", false);
	this->_place.field_visible("project", false);
	this->_place.field_visible("properties", false);
	this->_place.field_visible("timeline", false);
}

void windy::app::create_menus() {

	std::vector<std::string> menu_strings = {
		"File", 
		"View", 
		"Assets", 
		"Game"
	};

	std::vector<std::vector<std::string> > sub_menu_strings = {
		{
			"New project...", 
			"Open project...", 
			"Save", 
			"Save As...", 
			"Export As...", 
			"Exit"
		},
		{
			"Edition", 
			"Project", 
			"Properties", 
			"Timeline"
		}, 
		{
			"Compose...",
			"Delete..."
		}, 
		{
			"Create Scene...", 
			"Delete Scene..."
		}
	};

	std::vector < std::vector<std::function<void(nana::menu::item_proxy)> >> sub_menu_commands = {
		{
			std::bind(&app::on_file_new_pressed, this, std::placeholders::_1),
			std::bind(&app::on_file_open_pressed, this, std::placeholders::_1),
			std::bind(&app::on_file_save_pressed, this, std::placeholders::_1),
			std::bind(&app::on_file_save_as_pressed, this, std::placeholders::_1),
			std::bind(&app::on_file_export_as_pressed, this, std::placeholders::_1),
			std::bind(&app::on_file_exit_pressed, this, std::placeholders::_1)
		}, 
		{
			std::bind(&app::on_view_edition_pressed, this, std::placeholders::_1),
			std::bind(&app::on_view_project_pressed, this, std::placeholders::_1),
			std::bind(&app::on_view_properties_pressed, this, std::placeholders::_1),
			std::bind(&app::on_view_timeline_pressed, this, std::placeholders::_1)
		}, 
		{
			std::bind(&app::on_assets_compose_pressed, this, std::placeholders::_1),
			std::bind(&app::on_assets_delete_pressed, this, std::placeholders::_1)
		}, 
		{
			std::bind(&app::on_game_create_scene_pressed, this, std::placeholders::_1),
			std::bind(&app::on_game_delete_scene_pressed, this, std::placeholders::_1)
		}

	};

	for (unsigned int i = 0; i < menu_strings.size(); ++i) {
		auto menu_string = menu_strings[i];
		this->_menubar.push_back({ menu_string.begin(), menu_string.end() });

		for (unsigned int j = 0; j < sub_menu_strings[i].size(); ++j) {

			auto sub_menu_string = sub_menu_strings[i][j];

			this->_menubar.at(i).append({ sub_menu_string.begin(), sub_menu_string.end() },
										sub_menu_commands[i][j]);
		}
	}


	this->_menubar.at(0).enabled(2, false);
	this->_menubar.at(0).enabled(3, false);
	this->_menubar.at(0).enabled(4, false);

	this->_menubar.at(1).check_style(0, nana::menu::checks::highlight);
	this->_menubar.at(1).check_style(1, nana::menu::checks::highlight);
	this->_menubar.at(1).check_style(2, nana::menu::checks::highlight);
	this->_menubar.at(1).check_style(3, nana::menu::checks::highlight);

	this->_menubar.at(1).checked(0, false);
	this->_menubar.at(1).checked(1, false);
	this->_menubar.at(1).checked(2, false);
	this->_menubar.at(1).checked(3, false);

	this->_menubar.at(1).enabled(0, false);
	this->_menubar.at(1).enabled(1, false);
	this->_menubar.at(1).enabled(2, false);
	this->_menubar.at(1).enabled(3, false);

	this->_menubar.at(2).enabled(0, false);
	this->_menubar.at(2).enabled(1, false);

	this->_menubar.at(3).enabled(0, false);
	this->_menubar.at(3).enabled(1, false);
}

void windy::app::on_file_new_pressed(nana::menu::item_proxy& ip) {
	auto construction = this->_environment->project()->construct("C:/local/project_test/info.wpf",
																  this->_instance_creator);

	if (construction) {
		this->_menubar.at(1).enabled(0, true);
		this->_menubar.at(1).enabled(1, true);
		this->_menubar.at(1).enabled(2, true);
		this->_menubar.at(1).enabled(3, true);

		this->_menubar.at(1).checked(0, true);
		this->_menubar.at(1).checked(1, true);
		this->_menubar.at(1).checked(2, true);
		this->_menubar.at(1).checked(3, true);

		this->_place.field_visible("edition", true);
		this->_place.field_visible("project", true);
		this->_place.field_visible("properties", true);
		this->_place.field_visible("timeline", true);

		this->_menubar.at(2).enabled(0, true);
		this->_menubar.at(2).enabled(1, true);

		this->_menubar.at(3).enabled(0, true);
		this->_menubar.at(3).enabled(1, true);


	}

}

void windy::app::on_file_open_pressed(nana::menu::item_proxy& ip) {

}

void windy::app::on_file_save_pressed(nana::menu::item_proxy& ip) {

}

void windy::app::on_file_save_as_pressed(nana::menu::item_proxy& ip) {

}

void windy::app::on_file_export_as_pressed(nana::menu::item_proxy& ip) {

}

void windy::app::on_file_exit_pressed(nana::menu::item_proxy& ip) {
	nana::API::exit();
}

void windy::app::on_view_edition_pressed(nana::menu::item_proxy& ip) {
	if (ip.checked()) {
		this->_place.field_visible("edition", true);
	}
	else {
		this->_place.field_visible("edition", false);
	}
}

void windy::app::on_view_project_pressed(nana::menu::item_proxy& ip) {
	if (ip.checked()) {
		this->_place.field_visible("project", true);
	}
	else {
		this->_place.field_visible("project", false);
	}
}

void windy::app::on_view_properties_pressed(nana::menu::item_proxy& ip) {
	if (ip.checked()) {
		this->_place.field_visible("properties", true);
	}
	else {
		this->_place.field_visible("properties", false);
	}
}

void windy::app::on_view_timeline_pressed(nana::menu::item_proxy& ip) {
	if (ip.checked()) {
		this->_place.field_visible("timeline", true);
	}
	else {
		this->_place.field_visible("timeline", false);
	}
}

void windy::app::on_assets_compose_pressed(nana::menu::item_proxy& ip) {
	if (this->_instance_creator->items().size() > 0) {
		switch (this->_instance_creator->category()) {
			case layer::kind::groupable: {
						
				nana::inputbox compose_inbox(this->handle(),
										L"Specify the name for this composition:",
										L"Compose...");

				nana::inputbox::text composition_name(L"<bold blue>Name</>");


				compose_inbox.verify([this, &composition_name](nana::window handle) {

					bool success = true;

					if (composition_name.value().empty()) {
						nana::msgbox mb(handle, L"Invalid input");
						mb << L"Name must be specified.";
						mb.show();
						success = false; //verification failed
					}

					return success;
				});

				auto result = compose_inbox.show(composition_name);

				if (result) {

					std::vector
						<std::pair<std::string, std::shared_ptr<content::item> > > maps;

					for (auto item : this->_instance_creator->items()) {

						nana::string info_message = L"Specify the mapping name for ";

						auto item_name = item->name();

						info_message.append(nana::string(item_name.begin(), 
															item_name.end()));

						info_message.append(nana::string(L":"));

						nana::inputbox mapping_inbox(this->handle(),
														info_message,
														L"Compose...");

						nana::inputbox::text mapping_name(L"<bold blue>Name</>");

						mapping_inbox.verify([this, &mapping_name](nana::window handle) {

							bool success = true;

							if (mapping_name.value().empty()) {
								nana::msgbox mb(handle, L"Invalid input");
								mb << L"Name must be specified.";
								mb.show();
								success = false; //verification failed
							}

							return success;
						});

						auto result = mapping_inbox.show(mapping_name);

						if (result) {
							auto string = mapping_name.value();
							std::string regular_mapping_name(string.begin(), string.end());
							maps.push_back(std::make_pair(regular_mapping_name, item));
						}

					}

					std::shared_ptr<content::composition> composition;

					if (maps.empty()) {
						nana::msgbox mb(this->handle(), L"Invalid data");
						mb << L"Animations must be specified.";
						mb.show();
					} else {
						auto string = composition_name.value();

						auto regular_composition_name =
							std::string(string.begin(), string.end());

						composition =
							std::make_shared<content::composition>
							(regular_composition_name, uuid::generate());

						composition->maps(maps);

						this->_environment->project()->assets()->
							collection()->add_composition(composition);

						this->_environment->project()->assets()->refresh();

					}
				}

			};
			break;
		}
	} else {
		nana::msgbox error(this->handle(), 
							L"Error");

		error << L"Please select one or more animations first.";

		error.show();
	}
}

void windy::app::on_assets_delete_pressed(nana::menu::item_proxy& ip) {
			
	if (this->_instance_creator->items().size() > 0) {

		switch (this->_instance_creator->category()) {
			case layer::kind::graphicable: {
				// references from graphicables and nuke 
				{
					for (auto item : this->_instance_creator->items()) {

						for (auto instance : item->instances()) {
							this->_environment->project()->remove(instance);
						}

						auto sprite =
							std::dynamic_pointer_cast<content::sprite>(item);

						this->_environment->project()->remove_sprite(sprite);
					}
				}

			}
			break;

			case layer::kind::groupable: {
				for (auto item : this->_instance_creator->items()) {

					// references from compositions and nuke if single-animation remains
					{
						auto compositions = 
						this->_environment->project()->assets()->collection()->
							get(layer::kind::compositable);

						std::vector <std::shared_ptr<content::composition> >
							compositions_nuked;

						for (auto composition_item : compositions) {
							auto composition = 
								std::dynamic_pointer_cast<content::composition>
								(composition_item);

							auto& instances = composition->instances();

							for (auto instance : instances) {
								auto compositable = 
									std::dynamic_pointer_cast
									<content::compositable>(instance);

								std::vector<std::shared_ptr<content::groupable> > 
									groupables_nuked;

								for (auto child : compositable->children()) {
									auto groupable = 
										std::dynamic_pointer_cast
										<content::groupable>(child);

									if (groupable->asset_uuid() == item->uuid()) {
										groupables_nuked.push_back(groupable);
										this->_environment->project()->assets()->
											collection()->remove(groupable);
									}

								}

								for (auto nuked : groupables_nuked) {
									auto& children = compositable->children();
									auto it = std::find(children.begin(), 
														children.end(),
														nuked);

									children.erase(it);
								}

							}

							composition->unmap(item);

							if (composition->maps().empty()) {
								compositions_nuked.push_back(composition);
							}
							
						}

						for (auto composition : compositions_nuked) {
							for (auto instance : composition->instances()) {
								this->_environment->project()->remove(instance);
							}

							this->_environment->project()->remove_composition(composition);
						}
					}
						
					// nuke timeline triggers 
					{

						for (auto instance : item->instances()) {
							auto groupable =
								std::dynamic_pointer_cast<content::groupable> (instance);

							for (auto trigger : groupable->triggers()) {
								this->_environment->timeline()->remove_trigger(trigger);
							}

						}

					}

					// references from groupables and nuke 

					{
						auto instances = item->instances(); // copy by reference to store the instances

						for (auto instance : instances) {
							this->_environment->project()->remove(instance);
						}

						auto group = std::dynamic_pointer_cast<content::group>(item);

						this->_environment->project()->remove_group(group);

					}
				}
			}
			break;

			case layer::kind::compositable: {
				// references from compositables and nuke 
				{
					for (auto item : this->_instance_creator->items()) {

						for (auto instance : item->instances()) {
							this->_environment->project()->remove(instance);
						}

						auto composition = 
							std::dynamic_pointer_cast<content::composition>(item);

						this->_environment->project()->remove_composition(composition);
					}
				}

			}
			break;

		}

		// refresh
		{
			this->_environment->project()->assets()->refresh();

//			this->_selection_targets.clear();

			this->_environment->edition()->refresh(this->_environment->project()->game()->container());

			this->_environment->properties()->clear();

			this->_environment->trigger_recalculation();
		}

	} else  {
		nana::msgbox error(this->handle(),
							L"Error");

		error << L"Please select one or more assets first.";

		error.show();
	}

}

void windy::app::on_game_create_scene_pressed(nana::menu::item_proxy& ip) {

}

void windy::app::on_game_delete_scene_pressed(nana::menu::item_proxy& ip) {

}

void windy::app::compositable_map_menu() {

	auto animations =
		this->_environment->project()->assets()->collection()->get(layer::groupable);

	if (animations.empty()) {
		nana::msgbox mb(*this, L"Error!");
		mb << L"Create an animation first.";
		mb.show();
	}
	else {
		nana::inputbox::text name(L"<bold blue>Name</>");

		std::vector<std::wstring> nana_vector;

		for (auto& animation : animations) {
			auto name = animation->name();
			nana_vector.push_back(std::wstring(name.begin(), name.end()));
		}

		nana::inputbox::text options(L"Animation", nana_vector);

		nana::inputbox inbox(*this,
			L"Please input the <bold>creation parameters</> for the mapping.",
			L"Map");

		inbox.verify([this, &name](nana::window handle) {

			auto object_buffer_name = name.value();
			std::string object_name(object_buffer_name.begin(), object_buffer_name.end());

			if (object_name.empty())
			{
				nana::msgbox mb(handle, L"Invalid input");
				mb << L"Name should not be empty, please input the mapping name.";
				mb.show();
				return false; //verification failed
			}

			return true; //verified successfully
		});

		if (inbox.show_modal(name, options/*, file,*/)) {

			auto mapping_buffer_name = name.value();
			auto animation_buffer_name = options.value();

			auto mapping_name =
				std::string(mapping_buffer_name.begin(), mapping_buffer_name.end());

			auto animation_name =
				std::string(animation_buffer_name.begin(), animation_buffer_name.end());
			
		}
	}
}

bool windy::app::instance_validation(const nana::point& location) {

	auto canvas_rectangle = nana::rectangle(this->_canvas_stub_widget.pos().x, 
											this->_canvas_stub_widget.pos().y, 
											this->_canvas_stub_widget.size().width, 
											this->_canvas_stub_widget.size().height);

	return canvas_rectangle.is_hit(location);

}

void windy::app::on_instance_bake(std::shared_ptr<content::instance> instance,
								  const nana::point& location) {

	auto scale = this->_environment->project()->game()->container()->scale();

	instance->zoom(this->_environment->project()->game()->container()->zoom_focus(),
				   scale);


	auto real_location = location;

	auto container_projection = 
		this->_environment->project()->game()->container()->projection_rectangle();

	real_location.x -= container_projection.x;
	real_location.y -= container_projection.y;

	real_location.x /= scale;
	real_location.y /= scale;

	instance->location(real_location);
		
	this->_environment->project()->game()->container()->add(instance);
	this->_environment->edition()->refresh(this->_environment->project()->game()->container());
}

void windy::app::update() {
	if (this->_environment->project()->game()->container() == nullptr) {
		return;
	}

	// update timeline
	this->_environment->timeline()->update(1.0f/60.0f);
			
	// update game container
	//this->_project->game()->container()->update(1.0f / 60.0f);

	// render game container
	this->_drawing.clear();
	this->_drawing.update();

	// render timeline
	this->_environment->timeline()->drawing()->clear();
	this->_environment->timeline()->drawing()->update();
}

void windy::app::on_key_down(const nana::arg_keyboard& arg) {
	
}

void windy::app::on_key_up(const nana::arg_keyboard& arg) {

}

void windy::app::on_mouse_wheel(const nana::arg_wheel& arg) {

}

void windy::app::on_mouse_down(const nana::arg_mouse& arg) {

}

void windy::app::on_mouse_up(const nana::arg_mouse& arg) {

}

void windy::app::on_mouse_move(const nana::arg_mouse& arg) {

}

void windy::app::on_mouse_double_click(const nana::arg_mouse& arg) {

}

void windy::app::on_draw_update(nana::paint::graphics& graphics) {

}

void windy::app::on_mouse_command_update() {

}

