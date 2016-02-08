#ifndef windy_tilemap
#define windy_tilemap

#include <cmath>

#include <string>
#include <cassert>

#include "tileset.hpp"
#include "tilelayer.hpp"

namespace windy{
	class tilemap;
}

class windy::tilemap {
public:
	tilemap(const std::string& raw_name,
			const unsigned int& width, 
			const unsigned int& height,
			const unsigned int& tile_size = 32, 
			const unsigned int& texture_size = 1024);

public:
	void push_tileset(const unsigned int& first_gid);

	tileset* current_tileset();

	int build_tmx(const std::string& out_path);

private:
	unsigned int validate_tileset();

public:
	std::string raw_name;

	unsigned int width;
	unsigned int height;

	unsigned int tile_size;
	unsigned int texture_size;
	unsigned int spacing;
	unsigned int margin;
	unsigned int matrix_size;

	std::unique_ptr<tilelayer> tile_layer;
	std::vector<std::unique_ptr<tileset> > tilesets;
};

#endif

