#ifndef windy_sprite
#define windy_sprite

#include <string>
#include <memory>

#include "core/content/item.hpp"

namespace windy {
	namespace entity {
		class texture;
	}

	namespace content {
			class sprite : public item {
			public:
				sprite(const std::string& name,
					   const std::string& uuid, 
					   const std::string& path);
				std::string path();
				std::shared_ptr<instance> build();
				void remove(std::shared_ptr<class instance> instance);

			private:
				std::string _path;
				std::shared_ptr<entity::texture> _texture;
			};
	}
}

#endif