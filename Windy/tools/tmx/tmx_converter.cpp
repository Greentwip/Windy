#include <iostream>
#include <memory>
#include <vector>
#include <sstream>

#include "tmx_converter.hpp"

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include "boost/gil/image.hpp"
#include "boost/gil/typedefs.hpp"
#include "boost/gil/extension/io/png_io.hpp"
#include "boost/gil/extension/numeric/sampler.hpp"
#include "boost/gil/extension/numeric/resample.hpp"

#undef png_infopp_NULL
#undef int_p_NULL

#include "tilemap.hpp"
#include "tileset.hpp"
#include "tilelayer.hpp"

#include "tools/crypto/sha256.hpp"
#include "tools/memory/mmap_allocator.hpp"
#include "tools/memory/mmap_pool.hpp"

template <template<class, class, class...> class C, typename K, typename V, typename... Args>
bool GetWithDef(C<K, V, Args...>& m, K const& key, V& value)
{
	typename C<K, V, Args...>::const_iterator it = m.find(key);
	if (it == m.end()) {
		m[key] = value;
		return false;
	}

	value = m[key];

	return true;
}


struct PixelInserter {
	std::vector<unsigned char>* storage;
	PixelInserter(std::vector<unsigned char>* s) : storage(s) {}
	void operator()(boost::gil::rgba8_pixel_t p) const {
		storage->push_back(boost::gil::at_c<0>(p));
		storage->push_back(boost::gil::at_c<1>(p));
		storage->push_back(boost::gil::at_c<2>(p));
		storage->push_back(boost::gil::at_c<3>(p));
	}
};

bool windy::tmx_converter::lookup(const boost::gil::image_view<boost::gil::rgba8c_planar_loc_t>& a, 
								  unsigned long long& index) {
	
	std::vector<uint8_t> storage;

	auto channels = boost::gil::num_channels<boost::gil::rgba8c_planar_loc_t>();
	
	ptrdiff_t data_length = a.width() * a.height() * channels.value;
	storage.reserve(data_length);
	
	boost::gil::for_each_pixel(a, PixelInserter(&storage));

	std::string a_sha = SHA256::to_sha256(storage.data(), data_length);

	bool exists = GetWithDef(computed_hashes, a_sha, index);

	return exists;
}

int windy::tmx_converter::run(const std::vector<std::string>& args){

	std::string source(args[0].begin(), args[0].end());
	std::string output(args[1].begin(), args[1].end());

	unsigned int tile_size = std::stoi(args[2]);
	unsigned int texture_size = std::stoi(args[3]);

	std::vector<std::string> tokens;

	std::string buffer;
	for (auto character : output) {

		if (character == '\\') {
			tokens.push_back(buffer);
			buffer.clear();
		}
		else {
			buffer.push_back(character);
		}
	}

	tokens.push_back(buffer);

	std::string out_path;

	for (unsigned int i = 0; i < tokens.size() - 1; ++i) {
		out_path = out_path + tokens[i] + "\\";
	}

	auto back = tokens.back();

	std::string raw_name = std::string(back.begin(), back.end() - 4);


	auto image = std::make_shared<
	boost::gil::image
		<boost::gil::rgba8_pixel_t,
		true,
		mmap_allocator<unsigned char> > > ();

	try {
		boost::gil::png_read_and_convert_image(source, *image);
	} catch (boost::exception & ex) {
		std::cerr << boost::diagnostic_information_what(ex) << std::endl;
	}
	
	auto width = image->width();
	auto height = image->height();

	tilemap map(raw_name,
		width / tile_size,
		height / tile_size,
		tile_size,
		texture_size);

	boost::gil::point2<ptrdiff_t> crop_location(0, 0);
	boost::gil::point2<ptrdiff_t> crop_dimensions(map.tile_size, map.tile_size);

	{

		boost::gil::rgba8_image_t empty_tile (map.tile_size, map.tile_size);

		unsigned long long tile_index = 0;
		lookup(boost::gil::const_view(empty_tile), tile_index);
	}

	unsigned long long sorted_tiles = 0;
	ptrdiff_t tile_count = 0;

	unsigned long long mb = 4096;
	unsigned long long pot = 20;

	unsigned long long res = (mb << pot);

	windy::mmap_pool<unsigned long long> allocator(res);

	for (unsigned int y = 0; y < height; y += map.tile_size) {
		for (unsigned int x = 0; x < width; x += map.tile_size) {

			crop_location.x = x;
			crop_location.y = y;

			unsigned long long tile_index = sorted_tiles + 1;

			auto tile = boost::gil::subimage_view(boost::gil::const_view(*image), crop_location, crop_dimensions);
			
			bool tile_exists = lookup(tile, tile_index);

			if (!tile_exists) {

				if ((sorted_tiles % (map.matrix_size * map.matrix_size)) == 0) {
					// time for another tileset
					map.push_tileset(tile_index);
				}

				auto tileset = map.current_tileset();

				++sorted_tiles;

				tileset->blit_tile(tile);

			}


			++tile_count;

			auto idx = allocator.allocate(sizeof(unsigned long long));

			memcpy(idx, &tile_index, sizeof(unsigned long long));
		}
	}

	map.tile_layer->index_keys.first = allocator.data();
	map.tile_layer->index_keys.second = tile_count;

	computed_hashes.clear();

	map.build_tmx(out_path);

	allocator.cleanup();

	return 0;  
}

