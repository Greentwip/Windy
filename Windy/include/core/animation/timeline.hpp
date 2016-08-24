#ifndef windy_timeline
#define windy_timeline

#include <memory>
#include <vector>

#include "nana/gui/widgets/group.hpp"
#include "nana/gui/drawing.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nana/gui/widgets/scroll.hpp"
#include "nana/gui/widgets/picture.hpp"

namespace windy{
	namespace animation{
			class timeline : public nana::group {
			public:
				timeline();
				void init();
				std::shared_ptr<nana::drawing> drawing();
				void drawing(std::shared_ptr<nana::drawing> drawing);
				void frame_set(std::shared_ptr<class frame_set> frame_set);
				std::shared_ptr<class frame_set> frame_set();

			private:
				void on_draw_update(nana::paint::graphics& graphics);
				bool on_animation_trigger();
				void play();
				void stop();

			public:
				void update(const float& delta);
				void recalculate_triggers();
				void recalculate();
				auto trigger_vector_iterator(std::shared_ptr<class trigger> trigger);
				void add_trigger(std::shared_ptr<class trigger> trigger);
				void remove_trigger(std::shared_ptr<class trigger> trigger);
				void balance_triggers();
				std::vector<std::vector<std::shared_ptr<class trigger > > >& triggers();
				void render_triggers(const std::vector<std::shared_ptr<class trigger> >& triggers);
				std::vector<std::shared_ptr<class trigger> >& render_triggers();

			private:
				bool _playing;
				float _counter;

			private:
				std::vector<std::shared_ptr<nana::button> > _buttons;				
				nana::scroll<false> _scroll;
				nana::picture _canvas;

				std::shared_ptr<nana::drawing> _drawing;

				std::shared_ptr<class frame_set> _frame_set;

				std::vector<std::vector<std::shared_ptr<trigger > > > _triggers;
				std::vector<std::shared_ptr<trigger> > _render_triggers;

				std::size_t _bar_value;
				std::size_t _timeline_size_in_frames;

				nana::size _timeline_cell_size;

				nana::place _place;
			};
		}
	}

#endif