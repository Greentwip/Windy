#ifndef windy_layers
#define windy_layers

#include <memory>

#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/listbox.hpp"

namespace windy {
	namespace content {

		class edition : public nana::group {
		public:
			edition();
			void init();
			void refresh(std::shared_ptr<class container> container);
			void clear();

		private:
			nana::place _place;
			nana::label _label;
			nana::listbox _content;
		};
	}
}

#endif