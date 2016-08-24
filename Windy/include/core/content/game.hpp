#ifndef windy_game
#define windy_game

#include <memory>
#include <string>

#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/listbox.hpp"

namespace windy {
	namespace content {

		class game : public nana::group {
		public:
			game();
			void init();
			void on_item_selected(const nana::arg_listbox& arg);
			void add_container(std::shared_ptr<class container> container);
			void remove_container(std::shared_ptr<class container> container);
			//@TODO arrange predicate builders
			void container(const std::string& name);
			std::shared_ptr<class container> container();

		private:
			nana::place _place;
			nana::label _label;
			nana::listbox _content;

		private:
			std::shared_ptr<class container> _container;

		private:
			std::vector<std::shared_ptr<class container> > _containers; 

		private:
			std::string _current_item;

		};
	}
}

#endif