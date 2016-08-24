#ifndef windy_trigger
#define windy_trigger

#include <memory>
#include <cstdint>

namespace windy {
	namespace animation {
		class trigger {
		public:
			enum kind {
				none,
				single,
				loop
			};

			trigger(kind kind,
					uint64_t frame,
					std::shared_ptr<class frame_set> frame_set);
			kind get_kind();
			void set_kind(kind kind);
			uint64_t frame();
			void playing(const bool& playing);
			bool playing();
			std::shared_ptr<class frame_set> frame_set();
			std::shared_ptr<trigger> previous();
			void previous(std::shared_ptr<trigger> previous);
			std::shared_ptr<trigger> next();
			void next(std::shared_ptr<trigger> next);

		private:
			bool _playing;
			kind _kind;
			uint64_t _frame;
			std::shared_ptr<class frame_set> _frame_set;

			std::shared_ptr<trigger> _previous;
			std::shared_ptr<trigger> _next;
		};
	}
}

#endif