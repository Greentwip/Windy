#include <fstream>
#include <string>

#include "tilemap.hpp"

#include "xmlwriter.hpp"

#include "tools/memory/mmap_pool.hpp"

windy::tilemap::tilemap(const std::string& raw_name,
						const unsigned int& width,
						const unsigned int& height,
						const unsigned int& tile_size,
						const unsigned int& texture_size) {

	this->raw_name = raw_name;

	this->width = width;
	this->height = height;
	this->tile_size = tile_size;
	this->texture_size = texture_size;
	this->spacing = 4;
	this->margin = this->spacing / 2; 

	unsigned int validation = validate_tileset();

	tile_layer = std::make_unique<tilelayer>(width, height);

}

void windy::tilemap::push_tileset(const unsigned int& first_gid) {
	this->tilesets.push_back
		(std::make_unique<tileset>
			(first_gid, this->tile_size, this->margin, this->spacing, this->matrix_size, this->texture_size));
}

windy::tileset* windy::tilemap::current_tileset() {
	return tilesets.back().get();
}

int windy::tilemap::build_tmx(const std::string& out_path) {


	auto map_width = std::to_string(this->width);
	auto map_height = std::to_string(this->height);
	auto map_tile_size = std::to_string(this->tile_size);

	using namespace	xmlw;

	std::ofstream f(out_path + raw_name + ".tmx");
	XmlStream xml(f);
	xml << prolog(); // write XML file declaration
	{	// open root tag
		xml << tag("map") // root tag

			<< attr("version") << "1.0"
			<< attr("orientation") << "orthogonal"
			<< attr("renderorder") << "right-down"

			<< attr("width") << map_width
			<< attr("height") << map_height

			<< attr("tilewidth") << map_tile_size
			<< attr("tileheight") << map_tile_size;
	}
	
	{ // fill tile sets
		for (unsigned int i = 0; i < tilesets.size(); ++i) {
			auto tileset = tilesets[i].get();

			tileset->extrude();

			std::string texture_path = out_path +
				raw_name +
				"_bank_" +
				std::to_string(i) +
				".png";

			tileset->save(texture_path);

			auto name = raw_name + "_bank_" + std::to_string(i);
			auto first_gid = std::to_string(tileset->first_gid);
			auto spacing = std::to_string(tileset->spacing);
			auto margin = std::to_string(tileset->margin);

			std::string source =
				raw_name +
				"_bank_" +
				std::to_string(i) +
				".png";

			auto texture_size = std::to_string(tileset->texture_size);

			xml << tag("tileset")
				<< attr("name") << name
				<< attr("firstgid") << first_gid
				<< attr("spacing") << spacing
				<< attr("margin") << margin
				<< attr("tilewidth") << map_tile_size
				<< attr("tileheight") << map_tile_size;

				xml << tag("image")
					<< attr("source") << source
					<< attr("width") << texture_size
					<< attr("height") << texture_size;
				xml << endtag("image");

			xml << endtag("tileset");
		}

	}
	

	{ // fill layer data
		xml << tag("layer")
			<< attr("name") << "layout"
			<< attr("width") << map_width
			<< attr("height") << map_height;

			xml << tag("data");

			for (unsigned long long i = 0; i < tile_layer->index_keys.second; ++i) {
				auto str = std::to_string(tile_layer->index_keys.first[i]);

				xml << tag("tile")

					<< attr("gid") << str; 

				xml	<< endtag("tile");

			}

			xml << endtag("data");

		xml << endtag("layer");
	}

	{// close root tag
		xml << endtag("map");
	}
	return 0;
}

unsigned int windy::tilemap::validate_tileset() {

	// a suitable texture is one of pot size that can be divided by tile_size
	// a suitable tile_size is one of pot size too
	assert(this->texture_size % this->tile_size == 0);

	unsigned int spaced_tile_size = this->tile_size + spacing;

	this->matrix_size = (unsigned int)std::floor(double(this->texture_size) / double(spaced_tile_size));

	// tilemap textures can't have 0 tiles, (tile_size == texture_size would be common here)
	assert(matrix_size != 0);

	// the margin must be an even number
	assert((this->texture_size - spaced_tile_size * this->matrix_size) % 2 == 0);

	return (this->texture_size - spaced_tile_size * this->matrix_size) / 2;
};
/*
unsigned int windy::tilemap::margin_for_tileset() {
}*/