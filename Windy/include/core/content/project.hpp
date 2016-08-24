#ifndef windy_project
#define windy_project

#include <memory>
#include <string>

#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/label.hpp"

namespace windy {
	namespace content {
		class sprite;
		class group;
		class composition;
		class assets;
		class game;
	}

	namespace content {

		class project : public nana::group {
		public:
			project();
			void init();
			bool construct(const std::string& path, 
						   std::shared_ptr<class instance_creator> instance_creator);
			void open(const std::string& path);
			void close();
			std::shared_ptr<class assets> assets();
			std::shared_ptr<class game>	 game();
			void remove(std::shared_ptr<class instance> instance);
			void remove_sprite(std::shared_ptr<class content::sprite> sprite);
			void remove_group(std::shared_ptr<class content::group> group);
			void remove_composition(std::shared_ptr<class content::composition> composition);

		private:
			nana::place _place;
			nana::label _label;
			std::shared_ptr<class assets>	_assets;
			std::shared_ptr<class game>		_game;
		};
	}
}

#endif