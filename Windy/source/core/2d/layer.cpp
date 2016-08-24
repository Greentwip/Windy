#include "core/2d/layer.hpp"

windy::layer::layer() {
}

void windy::layer::update(const float& delta){
	for (auto child : this->_children) {
		child->update(delta);
	}
}
