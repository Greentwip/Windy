#ifndef windy_layer
#define windy_layer

#include "core/2d/node.hpp"

//@TODO enable strict rules
// strict rules consist on making layers specific for each instance kind, graphicable, groupable, compositable
// these rules shall define the workflow and disallow any kind of mix up that may slow down the game.
// otherwise, development may be confusing and delaying
namespace windy {
	class layer : public node {
	public:
		enum kind {
			none = -1,
			graphicable = 0,
			groupable = 1,
			compositable = 2
		};

	public:
		layer();
		void update(const float& delta) override;
	};
}

#endif