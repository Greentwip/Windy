#ifndef windy_collection
#define windy_collection

#include <memory>
#include <vector>

#include "core/2d/layer.hpp"

namespace windy {
	namespace content {
		class collection {
		public:
			void add_sprite(std::shared_ptr<class sprite> sprite);
			void add_group(std::shared_ptr<class group> group);
			void add_composition(std::shared_ptr<class composition> composition);
			void remove_sprite(std::shared_ptr<class sprite> sprite);
			void remove_group(std::shared_ptr<class group> group);
			void remove_composition(std::shared_ptr<class composition> composition);
			void remove(std::shared_ptr<class instance> instance);
			std::vector<std::shared_ptr<class item> >& get(layer::kind kind);
			//@TODO vector of item shared pointer vectors

		private:
			std::vector<std::shared_ptr<class item> >			_sprites;
			std::vector<std::shared_ptr<class item> >			_groups;
			std::vector<std::shared_ptr<class item> >			_compositions;
			std::vector<std::shared_ptr<class item> >			_stub;
		};

	}
}

#endif