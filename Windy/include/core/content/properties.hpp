#ifndef windy_properties
#define windy_properties

#include <memory>

#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/listbox.hpp"

namespace windy {
	namespace content {

		class properties : public nana::group {
		public:
			properties();

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