#ifndef windy_cropper
#define windy_cropper

#include <vector>
#include <string>
#include <iostream>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL


#include "boost/archive/binary_oarchive.hpp"
#include "boost/iostreams/stream_buffer.hpp"
#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/device/back_inserter.hpp"
#include "boost/serialization/vector.hpp"

#include "boost/gil/image.hpp"
#include "boost/gil/typedefs.hpp"
#include "boost/gil/extension/io_new/png_all.hpp"
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

#include "tools/filesystem/path.hpp"
#include "tools/crypto/z64.hpp"


namespace windy {

	class cropper {
	public:
		int run(const std::vector<std::string>& arguments) {

			auto input = arguments[0];
			auto output = arguments[1];

			auto raw_name = path::raw_name(input);

			uint64_t pieces = atoi(arguments[2].c_str());

			assert(pieces % 2 == 0);

			boost::gil::image
				<boost::gil::rgba8_pixel_t,
				true,
				mmap_allocator<unsigned char> > img;
			try {
				boost::gil::read_image(input, img, boost::gil::png_tag());
			} catch (boost::exception & ex) {
				std::cerr << boost::diagnostic_information_what(ex) << std::endl;
			}

			uint64_t num_rows = uint64_t(std::floor(std::sqrt(pieces)));
			auto num_columns = pieces / num_rows;

			boost::gil::point2<ptrdiff_t> tile_location(0, 0);
			boost::gil::point2<ptrdiff_t>
				tile_dimensions(ptrdiff_t(std::ceil(img.width() / num_columns)),
								ptrdiff_t(std::ceil(img.height() / num_rows)));

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

//					auto texture_output_path = boost::filesystem::path(output);

//					texture_output_path /= std::string(raw_name +
//												"_" + 
//												std::to_string(y) + 
//												"_" +
//												std::to_string(x) + 
//												".png");
				
					std::stringstream output_segment_stream;

//@NOTE compression achieves smaller files by doing z::from for each segment's data
					boost::gil::write_view(output_segment_stream, boost::gil::const_view(tile), boost::gil::png_tag());

					wxt_segment wxt_segment(x, y, tile_location.x, tile_location.y, output_segment_stream.str());

					wxt_container.add_segment(wxt_segment);
				}
			}

//@NOTE read above

#ifndef z_compression
#define z_compression
#endif

			auto wxt_output_path = boost::filesystem::path(output);
			wxt_output_path /= raw_name + ".wxt";

#ifdef z_compression
			std::stringstream output_archive_stream;

			boost::archive::binary_oarchive oa(output_archive_stream);
			oa << wxt_container;

			auto stream_data = output_archive_stream.str();

			auto z_data = z::compress_from(stream_data);

			std::stringstream z_data_stream;

			z_data_stream << z_data;

			std::ofstream output_file(wxt_output_path.string(), std::ios::binary | std::ios::trunc);

			std::copy(
				std::istreambuf_iterator<char>(z_data_stream),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(output_file));

#else
			std::ofstream ofs(wxt_output_path.string());

			boost::archive::binary_oarchive oa(ofs);
			oa << wxt_container;

#endif

			return 0;
		}
	};

}

#endif