#ifndef windy_graphicable
#define windy_graphicable

#include <memory>

#include "nana/basic_types.hpp"

#include "core/content/instance.hpp"

namespace windy {
	namespace entity {
		class texture;
	}

	namespace content {
		class graphicable : public instance {
		public:
			graphicable();
			void texture(std::shared_ptr<entity::texture> texture);
			std::shared_ptr<entity::texture> texture();
			nana::size size() override;

		private:
			std::shared_ptr<entity::texture> _texture;
		};

	}
}

#endif