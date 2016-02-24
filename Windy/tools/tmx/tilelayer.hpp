#ifndef windy_tilelayer
#define windy_tilelayer

#include <memory>
#include <vector>
#include <string>
#include <utility>

namespace windy{
class tilelayer;
}

class windy::tilelayer{
public:
	tilelayer(const unsigned int& width,
			  const unsigned int& height) {
		this->width = width;
		this->height = height;
		index_keys = std::vector<uint32_t>(this->width * this->height, 0);
	}

	unsigned int width;
	unsigned int height;

	std::vector<uint32_t> index_keys;
};

#endif