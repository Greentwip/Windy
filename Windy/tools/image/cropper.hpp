#ifndef windy_cropper
#define windy_cropper

#include <vector>
#include <string>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include "boost/gil/image.hpp"
#include "boost/gil/typedefs.hpp"
#include "boost/gil/extension/io/png_io.hpp"
#include "boost/gil/extension/numeric/sampler.hpp"
#include "boost/gil/extension/numeric/resample.hpp"

#undef png_infopp_NULL
#undef int_p_NULL

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp" 

#include "boost/lexical_cast.hpp"

#include "tools/memory/mmap_allocator.hpp"

#include "tools/image/structures/wxt.hpp"


namespace windy {

	class cropper {
	public:
		int run(const std::vector<std::string>& arguments) {

			auto input = arguments[0];
			auto output = arguments[1];

			std::vector<std::string> tokens;

			std::string buffer;
			for (auto character : input) {

				if (character == '\\') {
					tokens.push_back(buffer);
					buffer.clear();
				}
				else {
					buffer.push_back(character);
				}
			}

			tokens.push_back(buffer);

			auto back = tokens.back();

			std::string raw_name = std::string(back.begin(), back.end() - 4);


			uint64_t pieces = atoi(arguments[2].c_str());

			assert(pieces % 2 == 0);

			boost::gil::image
				<boost::gil::rgba8_pixel_t,
				true,
				mmap_allocator<unsigned char> > img;
			try {
				boost::gil::png_read_and_convert_image(input, img);
			} catch (boost::exception & ex) {
				std::cerr << boost::diagnostic_information_what(ex) << std::endl;
			}

			uint64_t num_rows = uint64_t(std::floor(std::sqrt(pieces)));
			auto num_columns = pieces / num_rows;

			boost::gil::point2<ptrdiff_t> tile_location(0, 0);
			boost::gil::point2<ptrdiff_t>
				tile_dimensions(std::ceil(img.width() / num_columns), 
								std::ceil(img.height() / num_rows));

			auto wxt_output_path = boost::filesystem::path(output);

			wxt_output_path /= raw_name + ".wxt";

			// make an archive
			wxt_image wxt_container(img.width(), img.height());

			for (uint64_t y = 0; y < num_rows; ++y) {
				for (uint64_t x = 0; x < num_columns; ++x) {
					
					tile_location.y = y * tile_dimensions.y;
					tile_location.x = x * tile_dimensions.x; 

					boost::gil::image<boost::gil::rgba8_pixel_t, 
									  true,
									  mmap_allocator<unsigned char> >
						tile(tile_dimensions.x, tile_dimensions.y);

					boost::gil::rgba8_pixel_t px(0, 0, 0, 0);
					boost::gil::fill_pixels(boost::gil::view(tile), px);

					if (tile_location.y + tile_dimensions.y > img.height()) {
						tile_dimensions.y = (tile_location.y + tile_dimensions.y) - img.height();
					}

					if (tile_location.x + tile_dimensions.x > img.width()) {
						tile_dimensions.x = (tile_location.x + tile_dimensions.x) - img.width();
					}

					auto view = boost::gil::subimage_view(boost::gil::view(img), tile_location, tile_dimensions);
					boost::gil::copy_pixels(view, boost::gil::view(tile));

					auto texture_output_path = boost::filesystem::path(output);

					texture_output_path /= std::string(raw_name +
												"_" + 
												std::to_string(y) + 
												"_" +
												std::to_string(x) + 
												".png");

					wxt_segment wxt_segment(x, y, tile_location.x, tile_location.y);

					wxt_container.add_segment(wxt_segment);

					boost::gil::png_write_view(texture_output_path.string(), boost::gil::const_view(tile));

				}
			}

			std::ofstream ofs(wxt_output_path.string());
			boost::archive::text_oarchive oa(ofs);
			oa << wxt_container;

			return 0;
		}
	};

}

#endif