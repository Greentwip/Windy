#ifndef windy_frame
#define windy_frame

namespace windy {
	namespace animation {
		class frame {
		public:
			frame();
			bool is_key_frame();
			void is_key_frame(const bool& is_key_frame);

		private:
			bool _is_key_frame;
		};
	}
}

#endif