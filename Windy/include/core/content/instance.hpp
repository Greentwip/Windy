#ifndef windy_instance
#define windy_instance

#include <memory>
#include <string>

#include "core/2d/drawable.hpp"
#include "core/2d/layer.hpp"

namespace windy {

	namespace entity {
		class texture;
	}

	namespace content {

		class instance : public drawable {
		public:
			instance();
			virtual std::shared_ptr<entity::texture> texture() = 0;
			layer::kind kind();
			void kind(layer::kind kind);
			std::string asset_uuid();
			void asset_uuid(const std::string& asset_uuid);

		private:
			layer::kind _kind;
			std::string _asset_uuid;
		};
	}
}

#endif