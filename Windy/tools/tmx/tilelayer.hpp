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
	}

	unsigned int width;
	unsigned int height;

	std::pair<unsigned long long*, unsigned long long> index_keys;
};

#endif