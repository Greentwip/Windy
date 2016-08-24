#ifndef windy_mmap_image
#define windy_mmap_image

#include <memory>

#include "boost/gil/image.hpp"
#include "boost/gil/typedefs.hpp"

#include "core/memory/mmap_allocator.hpp"

namespace windy {
	class mmap_image {
		typedef boost::gil::rgba8_pixel_t gil_rgba;
		typedef mmap_allocator<unsigned char> mmap_byte_allocator;
		typedef boost::gil::image<gil_rgba,
								  true,
								  mmap_byte_allocator> gil_mmap_rgba_image;

	public:
		mmap_image();
		mmap_image(const uint64_t& width, const uint64_t& height);
		gil_mmap_rgba_image& raw();
		std::shared_ptr<gil_mmap_rgba_image> data();

	private:
		std::shared_ptr<gil_mmap_rgba_image> _data;
	};
}

#endif