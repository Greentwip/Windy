#ifndef windy_frame_set
#define windy_frame_set

#include <memory>
#include <vector>
#include <cstdint>

namespace windy {
	namespace animation {

		class frame_set {
		public:
			frame_set(std::shared_ptr<frame_set> frame_set);
			frame_set(const uint64_t& duration);
			void reset(const uint64_t& duration);
			void swap_key_frame(const uint64_t& index);
			void enable_key_frame(const uint64_t& index);
			void disable_key_frame(const uint64_t& index);
			const uint64_t previous_key_frame_index(bool from_current_key_frame = true);
			const uint64_t current_key_frame_index();
			const uint64_t ordered_key_frame_index(const uint64_t& key_frame);
			void current_frame_index(const uint64_t& index);
			void current_key_frame_index(const uint64_t& index);
			std::vector<std::shared_ptr<class frame> >& frames();
			uint64_t current_frame_index();
			std::shared_ptr<class frame> current_frame();
			uint64_t duration();
			void reset_counter(const float& new_counter = 0);
			void stop();
			void update(const float& delta);

		private:
			float _counter;
			uint64_t _duration;

			uint64_t _current_frame_index;
			uint64_t _current_key_frame_index;

			std::vector<std::shared_ptr<class frame> > _timeline_frames;
		};
	}
}

#endif