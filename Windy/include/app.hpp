#ifndef windy_app
#define windy_app

#include <memory>

#include "nana/gui/widgets/form.hpp"
#include "nana/gui/widgets/menubar.hpp"
#include "nana/gui/widgets/panel.hpp"

#include "nana/gui/dragger.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/drawing.hpp"



namespace windy {
	class state;

	namespace content {
		class instance;
		class instance_creator;
		class environment;
	}

	class app
		: public nana::form
	{
	public: 
		enum class application_status {
			startup, 
			scene,
			graphicable, 
			groupable, 
			compositable
		};

	private:
		application_status _application_status;

	public:
		app();

	private:
		void prepare_place();

	private:
		void create_menus();
		void on_file_new_pressed(nana::menu::item_proxy& ip);
		void on_file_open_pressed(nana::menu::item_proxy& ip);
		void on_file_save_pressed(nana::menu::item_proxy& ip);
		void on_file_save_as_pressed(nana::menu::item_proxy& ip);
		void on_file_export_as_pressed(nana::menu::item_proxy& ip);
		void on_file_exit_pressed(nana::menu::item_proxy& ip);
		void on_view_edition_pressed(nana::menu::item_proxy& ip);
		void on_view_project_pressed(nana::menu::item_proxy& ip);
		void on_view_properties_pressed(nana::menu::item_proxy& ip);
		void on_view_timeline_pressed(nana::menu::item_proxy& ip);
		void on_assets_compose_pressed(nana::menu::item_proxy& ip);
		void on_assets_delete_pressed(nana::menu::item_proxy& ip);
		void on_game_create_scene_pressed(nana::menu::item_proxy& ip);
		void on_game_delete_scene_pressed(nana::menu::item_proxy& ip);


	private:
		void compositable_map_menu();

	private:
		bool instance_validation(const nana::point& location);
		void on_instance_bake(std::shared_ptr<content::instance> instance, 
							  const nana::point& location);

	private:
		void update();

		// render
		void on_draw_update(nana::paint::graphics& graphics);

	private:
		void on_key_down(const nana::arg_keyboard& arg);
		void on_key_up(const nana::arg_keyboard& arg);
				
		void on_mouse_wheel(const nana::arg_wheel& arg);
		void on_mouse_down(const nana::arg_mouse& arg);
		void on_mouse_up(const nana::arg_mouse& arg);
		void on_mouse_move(const nana::arg_mouse& arg);
		void on_mouse_double_click(const nana::arg_mouse& arg);
		void on_mouse_command_update();

	private:
		nana::timer										_update_timer;
		nana::timer										_mouse_command_timer;

		nana::drawing									_drawing;

	private:
		nana::dragger										_dragger;
		std::shared_ptr<class content::instance_creator>	_instance_creator;
		std::shared_ptr<class content::environment>			_environment;

		nana::panel<false>									_canvas_stub_widget;

	private:
		nana::menubar _menubar;
		nana::place   _place;

	private:
		std::vector<std::shared_ptr<class windy::state> > _states;
	};
}

#endif