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

#include "tools/image/merger.hpp"

#include "tools/crypto/sha256.hpp"
#include "tools/memory/mmap_allocator.hpp"
#include "tools/memory/mmap_pool.hpp"
#include "tools/filesystem/path.hpp"



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

		auto alpha = boost::gil::at_c<3>(p);

		if (alpha) {
			storage->push_back(boost::gil::at_c<0>(p));
			storage->push_back(boost::gil::at_c<1>(p));
			storage->push_back(boost::gil::at_c<2>(p));
			storage->push_back(alpha);
		} else {
			storage->push_back(0);
			storage->push_back(0);
			storage->push_back(0);
			storage->push_back(0);
		}

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

	std::string input(args[0].begin(), args[0].end());
	std::string output(args[1].begin(), args[1].end());

	unsigned int tile_size = std::stoi(args[2]);
	unsigned int texture_size = std::stoi(args[3]);

	auto raw_name = path::raw_name(input);
	auto wxt_validation = path::extension(input);


	auto image = std::make_shared<
	boost::gil::image
		<boost::gil::rgba8_pixel_t,
		true,
		mmap_allocator<unsigned char> > > ();

	if (wxt_validation.compare(".wxt") == 0) {
		// merge first
		windy::merger merger;
		image = merger.merge_from_file(input);

	} else {
		// from png
		try {
			//boost::gil::png_read_and_convert_image(source, *image);
			boost::gil::png_read_image(input, *image);
		}
		catch (boost::exception & ex) {
			std::cerr << boost::diagnostic_information_what(ex) << std::endl;
		}
	}



	boost::gil::image<boost::gil::rgba8_pixel_t, true> empty_tile(tile_size, tile_size);

	{
		boost::gil::rgba8_pixel_t px(0, 0, 0, 0);

		boost::gil::fill_pixels(boost::gil::view(empty_tile), px);

		unsigned long long tile_index = 0;
		lookup(boost::gil::const_view(empty_tile), tile_index);
	}

	uint64_t image_h_tiles = uint64_t(std::ceil(long double(image->height()) / long double(tile_size)));
	uint64_t image_w_tiles = uint64_t(std::ceil(long double(image->width()) / long double(tile_size)));

	uint64_t image_h = image_h_tiles * tile_size;
	uint64_t image_w = image_w_tiles * tile_size;

	uint64_t map_size_tiles = 128; // max tmx map size due to rendering limitations, 128 x 128 tiles
	uint64_t map_size = map_size_tiles * tile_size;

	uint64_t map_index_y = 0;
	uint64_t map_index_x = 0;

	uint64_t matrix_y = uint64_t(std::ceil(long double(image_h) / long double(map_size)));
	uint64_t matrix_x = uint64_t(std::ceil(long double(image_w) / long double(map_size)));

	std::vector<std::shared_ptr<tileset> > tilesets;

	boost::gil::point2<ptrdiff_t> crop_location(0, 0);
	boost::gil::point2<ptrdiff_t> crop_dimensions(tile_size, tile_size);

	uint64_t sorted_tiles = 0;

	for (uint64_t map_y = 0; map_y < matrix_y; ++map_y) {
		for (uint64_t map_x = 0; map_x < matrix_x; ++map_x) {
			tilemap map(raw_name, map_size_tiles, map_size_tiles, tile_size, texture_size);
			uint64_t tile_count = 0;

			for (uint64_t tile_y = map_y * map_size_tiles;
				 tile_y < (map_y * map_size_tiles) + map_size_tiles;
				 ++tile_y) {

				for (uint64_t tile_x = map_x * map_size_tiles;
					 tile_x < (map_x * map_size_tiles) + map_size_tiles;
					 ++tile_x) {

					uint64_t tile_index = sorted_tiles + 1;

					if (tile_y < image_h_tiles && tile_x < image_w_tiles) {
						// exists inside the texture bounds
						crop_location.y = tile_y * tile_size;
						crop_location.x = tile_x * tile_size;

						bool needs_clean_tile = false;

						if (crop_location.y + tile_size > image->height()) {
							crop_dimensions.y -= (crop_location.y + tile_size) - (image->height());

							needs_clean_tile = true;
						}

						if (crop_location.x + tile_size > image->width()) {
							crop_dimensions.x -= (crop_location.x + tile_size) - (image->width());
							needs_clean_tile = true;
						}


						auto tile =
							boost::gil::subimage_view(boost::gil::const_view(*image),
								crop_location,
								crop_dimensions);

						if (needs_clean_tile) {

							boost::gil::rgba8_pixel_t px(0, 0, 0, 0);

							boost::gil::fill_pixels(boost::gil::view(empty_tile), px);

							auto view = boost::gil::subimage_view(boost::gil::view(empty_tile),
																  boost::gil::point2<ptrdiff_t>(0, 0),
																  crop_dimensions);
							boost::gil::copy_pixels(tile, view);

							tile = boost::gil::view(empty_tile);

							crop_dimensions.y = tile_size;
							crop_dimensions.x = tile_size; // we restore the crop dimensions
						}

						bool computed_tile = lookup(tile, tile_index);

						if (!computed_tile) { // this is a new-uncomputed tile, let's add it

							windy::tileset* tileset = nullptr;

							if ((sorted_tiles % (map._matrix_size * map._matrix_size)) == 0) {
								// time for another tileset
								tilesets.push_back
									(std::make_shared<class tileset>
										(tile_index,
											map._tile_size,
											map._margin,
											map._spacing,
											map._matrix_size,
											map._texture_size));

								tileset = tilesets.back().get();

							}

							tileset = tilesets.back().get();

							++sorted_tiles;

							tileset->blit_tile(tile);

						}

					} else {
						// does not exist inside the texture bounds, we need to blit max tiles + alpha blit
						tile_index = 0;
					}

					uint64_t largest_first_gid = 0;
					std::shared_ptr<class tileset> target_tileset = nullptr;

					for (uint64_t i = 0; i < tilesets.size(); ++i) {
						auto tileset = tilesets[i];

						if (tile_index >= tileset->first_gid) {
							if (tileset->first_gid > largest_first_gid) {
								largest_first_gid = tileset->first_gid;
								target_tileset = tileset;
							}
						}
					}

					if (target_tileset != nullptr) {

						if (map._tile_layers.find(target_tileset) == map._tile_layers.end()) {
							map._tile_layers[target_tileset] =
								std::make_shared<tilelayer>(map._width, map._height);
						}

						map._tile_layers[target_tileset]->index_keys[tile_count] = tile_index;

					}

					++tile_count;
				}
			}

			map._tile_sets = tilesets;
			map.build_tmx(output, std::to_string(map_y) + "_" + std::to_string(map_x));
		}
	}

	computed_hashes.clear();

	for (uint64_t i = 0; i < tilesets.size(); ++i) {

		auto tileset = tilesets[i].get();

		tileset->extrude();

		boost::filesystem::path texture_path(output);

		texture_path /=
			raw_name +
			"_bank_" +
			std::to_string(i) +
			".png";

		tileset->save(texture_path.string());

	}


	return 0;
}
