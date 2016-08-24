#ifndef windy_group
#define windy_group

#include <utility>
#include <string>
#include <memory>
#include <vector>

#include "core/content/item.hpp"

namespace windy {
	namespace entity {
		class texture;
	}

	namespace animation {
		class frame_set;
	}

	namespace content {
		class group : public item {
			typedef std::pair<std::string, 
							  std::shared_ptr<entity::texture> > texture_pair;
		public:
			group(const std::string& name, const std::string& uuid);
			void textures(const std::vector <texture_pair>& textures);
			std::shared_ptr<instance> build();
			void remove(std::shared_ptr<class instance> instance);
			void separation(const unsigned int& separation);

		private:
			unsigned int _separation;
			std::vector<std::pair
						<std::string, std::shared_ptr<entity::texture> > > _textures;

			std::shared_ptr<animation::frame_set> _frame_set;

		};
	}
}

#endif