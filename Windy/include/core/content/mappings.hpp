#ifndef windy_mappings
#define windy_mappings

#include <memory>

#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/listbox.hpp"

namespace windy {
	namespace content {
	class mappings : public nana::group {
		public:
			mappings();
			void init();
			void load(std::shared_ptr<class instance> instance);
			void clear();

		private:
			nana::place _place;
			nana::label _label;
			nana::listbox _content;
		};
	}
}

#endif