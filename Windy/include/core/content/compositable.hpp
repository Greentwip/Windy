#ifndef windy_compositable
#define windy_compositable

#include <memory>

#include "nana/basic_types.hpp"

#include "core/content/instance.hpp"

namespace windy {
	namespace entity {
		class texture;
	}

	namespace content {
		class compositable : public instance {
		public:
			compositable();
			std::shared_ptr<entity::texture> texture();
			nana::size size() override;
		};
	}
}

#endif