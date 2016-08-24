#ifndef windy_composition
#define windy_composition

#include <utility>
#include <string>
#include <memory>
#include <vector>

#include "core/content/item.hpp"

namespace windy {
	namespace content {
		class composition : public item{
			typedef std::pair<std::string, std::shared_ptr<class item> > item_pair;
		public:
			composition(const std::string& name, const std::string& uuid);
			std::shared_ptr<class instance> build();
			void remove(std::shared_ptr<class instance> instance);
			void map(const std::pair<std::string, std::shared_ptr<class item> > map);
			auto map(const std::string& name);
			void unmap(std::shared_ptr<class item> item);
			void maps(const std::vector<item_pair>& maps);
			std::vector<item_pair>& maps();

		private:
			std::vector<item_pair> _maps;
		};
	}
}

#endif