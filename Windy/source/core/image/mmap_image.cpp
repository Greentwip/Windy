#include "core/image/mmap_image.hpp"

windy::mmap_image::mmap_image() {
	_data = std::make_shared<gil_mmap_rgba_image>();
}

windy::mmap_image::mmap_image(const uint64_t& width, const uint64_t& height) {
	_data = std::make_shared<gil_mmap_rgba_image>(width, height);
}

windy::mmap_image::gil_mmap_rgba_image& windy::mmap_image::raw() { return *this->_data; }

std::shared_ptr<windy::mmap_image::gil_mmap_rgba_image> windy::mmap_image::data() { return this->_data; }

