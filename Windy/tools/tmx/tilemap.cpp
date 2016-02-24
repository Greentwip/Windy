#include <fstream>
#include <string>

#include "tilemap.hpp"

#include "xmlwriter.hpp"

#include "tools/memory/mmap_pool.hpp"
#include "tools/crypto/base64.hpp"

#include "zlib.h"

windy::tilemap::tilemap(const std::string& raw_name,
	const unsigned int& width,
	const unsigned int& height,
	const unsigned int& tile_size,
	const unsigned int& texture_size) {

	this->_raw_name = raw_name;

	this->_width = width;
	this->_height = height;
	this->_tile_size = tile_size;
	this->_texture_size = texture_size;
	this->_spacing = 4;
	this->_margin = this->_spacing / 2;

	auto validation = validate_tileset();
}


unsigned int windy::tilemap::validate_tileset() {

	// a suitable texture is one of pot size that can be divided by tile_size
	// a suitable tile_size is one of pot size too
	assert(this->_texture_size % this->_tile_size == 0);

	unsigned int spaced_tile_size = this->_tile_size + this->_spacing;

	this->_matrix_size = (unsigned int)std::floor(double(this->_texture_size) / double(spaced_tile_size));

	// tilemap textures can't have 0 tiles, (tile_size == texture_size would be common here)
	assert(this->_matrix_size != 0);

	// the margin must be an even number
	assert((this->_texture_size - spaced_tile_size * this->_matrix_size) % 2 == 0);

	return (this->_texture_size - spaced_tile_size * this->_matrix_size) / 2;
};

std::string windy::tilemap::compress(const std::vector<uint32_t>& input, const uint64_t& width, const uint64_t& height) {
	std::vector<unsigned char> data;
	data.reserve(width * height * 4);

	for (auto& gid : input) {

/*		unsigned char* u_data = (unsigned char*)(gid);

		for (unsigned int i = 0; i<sizeof(gid); ++i)
		{
			data.push_back(u_data[i]);
		}*/

		data.push_back((unsigned char)gid);
		data.push_back((unsigned char)gid >> 8);
		data.push_back((unsigned char)gid >> 16);
		data.push_back((unsigned char)gid >> 24);
	}

	
	std::vector<unsigned char> out;

	out.resize(1024);

	int err;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.next_in = (Bytef *)data.data();
	strm.avail_in = data.size();
	strm.next_out = (Bytef *)out.data();
	strm.avail_out = out.size();

	const int windowBits = 15;

	err = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits,
		8, Z_DEFAULT_STRATEGY);

	if (err != Z_OK) {
		//logZlibError(err);
		return std::string();
	}

	do {
		err = deflate(&strm, Z_FINISH);
		assert(err != Z_STREAM_ERROR);

		if (err == Z_OK) {
			// More output space needed
			int oldSize = out.size();
			out.resize(out.size() * 2);
			strm.next_out = (Bytef *)(out.data() + oldSize);
			strm.avail_out = oldSize;
		}
	} while (err == Z_OK);

	if (err != Z_STREAM_END) {
		//logZlibError(err);
		deflateEnd(&strm);
		return std::string();
	}

	const int outLength = out.size() - strm.avail_out;
	deflateEnd(&strm);

	out.resize(outLength);

	return windy::base64(out);
};

int windy::tilemap::build_tmx(const std::string& out_path, const std::string& map_index) {

	auto map_width = std::to_string(this->_width);
	auto map_height = std::to_string(this->_height);
	auto map_tile_size = std::to_string(this->_tile_size);

	using namespace	xmlw;

	auto output_path = boost::filesystem::path(out_path);

	output_path /= std::string(_raw_name + "_" + map_index + ".tmx");

	std::ofstream f(output_path.string());
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
		for (unsigned int i = 0; i < _tile_sets.size(); ++i) {
			auto tileset = _tile_sets[i].get();

			auto name = _raw_name + "_bank_" + std::to_string(i);
			auto first_gid = std::to_string(tileset->first_gid);
			auto spacing = std::to_string(tileset->spacing);
			auto margin = std::to_string(tileset->margin);

			std::string source =
				_raw_name +
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

		for (auto it = _tile_layers.begin(); it != _tile_layers.end(); ++it) {

			auto i = std::distance(_tile_layers.begin(), it);


			auto tile_layer = it->second;

			std::string layout_name = "layout_" + std::to_string(i);

			xml << tag("layer")
				<< attr("name") << layout_name
				<< attr("width") << map_width
				<< attr("height") << map_height;

			xml << tag("data")
				<< attr("encoding") << "base64"
				<< attr("compression") << "zlib";

			xml << chardata() << compress(tile_layer->index_keys, this->_width, this->_height);

/*			for (uint64_t i = 0; i < tile_layer->index_keys.size(); ++i) {
				auto str = std::to_string(tile_layer->index_keys[i]);

				xml << tag("tile")

					<< attr("gid") << str;

				xml << endtag("tile");

			}*/

			xml << endtag("data");

			xml << endtag("layer");
		}

	}

	{// close root tag
		xml << endtag("map");
	}
	return 0;
}


/*
unsigned int windy::tilemap::margin_for_tileset() {
}*/