#ifndef windy_wxt
#define windy_wxt

#include <vector>
#include <string>

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/serialization/vector.hpp"

namespace windy {
	class wxt_segment
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) 
		{
			ar & _idx_x;
			ar & _idx_y;
			ar & _x;
			ar & _y;
		}

		uint64_t _idx_x;
		uint64_t _idx_y;

		uint64_t _x;
		uint64_t _y;

	public:
		wxt_segment() {
			this->_idx_x	= -1;
			this->_idx_y	= -1;
			this->_x		= -1;
			this->_y		= -1;
		}

		wxt_segment(const uint64_t& idx_x, const uint64_t& idx_y, const uint64_t& x, const uint64_t& y) {
			this->_idx_x = idx_x;
			this->_idx_y = idx_y;
			this->_x = x;
			this->_y = y;
		}

		wxt_segment(const wxt_segment& other) {
			this->_idx_x = other._idx_x;
			this->_idx_y = other._idx_y;
			this->_x = other._x;
			this->_y = other._y;
		}

		const uint64_t& idx_x() { return this->_idx_x;  }
		const uint64_t& idx_y() { return this->_idx_y;  }

		const uint64_t& x() { return this->_x; }
		const uint64_t& y() { return this->_y; }
	};

	class wxt_image
	{

		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) 
		{
			ar & _width;
			ar & _height;
			ar & _segments;
		}

		uint64_t _width;
		uint64_t _height;

		std::vector<wxt_segment> _segments;

	public:
		wxt_image() {
			this->_width = -1;
			this->_height = -1;
		}

		wxt_image(const uint64_t& width, const uint64_t& height) {
			this->_width	= width;
			this->_height	= height;
		}

		const std::vector<wxt_segment>& segments_vector() { return this->_segments;  }

		void add_segment(const wxt_segment& image) {
			_segments.push_back(image);
		}

		const uint64_t& width() { return this->_width; }
		const uint64_t& height() { return this->_height; }
	};

}
#endif