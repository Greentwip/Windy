#ifndef windy_container
#define windy_container

#include <memory>

#include "core/2d/drawable.hpp"
#include "core/2d/layer.hpp"

namespace windy {
	namespace content {
			class container : public drawable {
			public:
				container();
				void bake();
				void add(std::shared_ptr<class instance> instance);
				void remove(std::shared_ptr<class instance> instance);
				std::vector<std::shared_ptr<class node> >& get(layer::kind kind);

				/*void update(const float& delta) override {
					for (auto layer : this->children()) {
						layer->update(delta);
					}
				}*/

			};

	}
}

#endif