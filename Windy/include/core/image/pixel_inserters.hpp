#ifndef windy_pixel_inserters
#define windy_pixel_inserters

#include <cstdint>

#include "boost/gil/typedefs.hpp"
#include "boost/gil/color_base.hpp"


namespace windy {
	struct pixel_count_wrapper {
		pixel_count_wrapper();
		void increase();
		uint64_t _value;
	};


	template<typename PixelType>
	struct PixelRGBAInserter {
		PixelType* _pixel_buffer;
		pixel_count_wrapper* _count_wrapper;

		PixelRGBAInserter(PixelType* pixel_buffer, pixel_count_wrapper* count_wrapper) {
			this->_pixel_buffer = pixel_buffer;
			this->_count_wrapper = count_wrapper;
		}

		void operator()(boost::gil::rgba8_pixel_t p) const {

			this->_pixel_buffer[this->_count_wrapper->_value].element.red = boost::gil::at_c<0>(p);
			this->_pixel_buffer[this->_count_wrapper->_value].element.green = boost::gil::at_c<1>(p);
			this->_pixel_buffer[this->_count_wrapper->_value].element.blue = boost::gil::at_c<2>(p);
			this->_pixel_buffer[this->_count_wrapper->_value].element.alpha_channel = boost::gil::at_c<3>(p);

			this->_count_wrapper->increase();

		}
	};
}

#endif