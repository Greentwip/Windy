#ifndef windy_state
#define windy_state

#include <map>
#include <memory>

#include "nana/gui/widgets/form.hpp"
#include "nana/gui/widgets/menubar.hpp"
#include "nana/gui/widgets/panel.hpp"

#include "nana/gui/dragger.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/drawing.hpp"

#include "nana/basic_types.hpp"
#include "nana/gui/detail/general_events.hpp"
#include "nana/paint/graphics.hpp"

namespace windy {

	namespace content {
		class instance;
		class environment;
	}

	class state {
	public:
		void bake(std::shared_ptr<class content::environment> environment);

	private:
		void on_key_down(const nana::arg_keyboard& arg);
		void on_key_up(const nana::arg_keyboard& arg);

		void on_mouse_wheel(const nana::arg_wheel& arg);
		void on_mouse_down(const nana::arg_mouse& arg);
		void on_mouse_move(const nana::arg_mouse& arg);
		void on_mouse_up(const nana::arg_mouse& arg);

	private:
		void trigger_recalculation();

	protected:
		virtual void animation_trigger_map();
		

	protected:
		virtual void key_down_trigger(const nana::arg_keyboard& arg) = 0;
		virtual void key_up_trigger(const nana::arg_keyboard& arg) = 0;

		virtual void mouse_wheel_trigger(const nana::arg_wheel& arg) = 0;
		virtual void mouse_down_trigger(const nana::arg_mouse& arg) = 0;
		virtual void mouse_move_trigger(const nana::arg_mouse& arg) = 0;
		virtual void mouse_up_trigger(const nana::arg_mouse& arg) = 0;

		virtual void on_draw_update(nana::paint::graphics& graphics) = 0;

		virtual void on_mouse_command_update() = 0;

	protected:
		nana::point _mouse_position;

	protected:
		void make_selection(const nana::point& position);

	private:
		void single_selection(std::shared_ptr<class content::instance> instance);

	protected:
		// @TODO command queue
		std::map<std::string, bool>								_key_modifiers;
		std::vector<std::shared_ptr<class movement_command> >	_movement_commands;

	protected:
		std::shared_ptr<class content::environment>	_environment;

		std::vector<std::shared_ptr<class content::instance> >	_selection_targets;

	};
}
#endif
