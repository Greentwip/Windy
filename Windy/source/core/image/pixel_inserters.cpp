#include "core/image/pixel_inserters.hpp"

windy::pixel_count_wrapper::pixel_count_wrapper() {
		this->_value = 0;
	}

void windy::pixel_count_wrapper::increase() {
	_value++;
}
 