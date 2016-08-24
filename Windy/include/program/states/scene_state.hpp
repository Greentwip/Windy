#ifndef windy_scene_state
#define windy_scene_state

#include <memory>

#include "program/states/state.hpp"

#include "nana/basic_types.hpp"
#include "nana/gui/detail/general_events.hpp"
#include "nana/paint/graphics.hpp"

#include "core/content/instance.hpp"

namespace windy {

	class scene_state : public state {
		typedef std::shared_ptr<nana::paint::graphics>  graphics_ptr;
		typedef std::pair<nana::point,
			graphics_ptr>				render_pair;

	public:
		scene_state();

		void key_down_trigger(const nana::arg_keyboard& arg);
		void key_up_trigger(const nana::arg_keyboard& arg);

		void mouse_wheel_trigger(const nana::arg_wheel& arg);
		void mouse_down_trigger(const nana::arg_mouse& arg);
		void mouse_move_trigger(const nana::arg_mouse& arg);
		void mouse_up_trigger(const nana::arg_mouse& arg);

		void on_draw_update(nana::paint::graphics& graphics);

		void on_mouse_command_update();

	private:
		std::vector<render_pair>				_render_batch;
	};
}
#endif
