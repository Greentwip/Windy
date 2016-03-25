#ifndef windy_merger
#define windy_merger

#include <vector>
#include <string>
#include <memory>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/device/array.hpp"

#include "boost/archive/binary_iarchive.hpp"

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

	class merger {
	public:
		int run(const std::vector<std::string>& arguments) {

			auto input = arguments[0];
			auto output = arguments[1];

			auto merged_output = merge_from_file(input);

			boost::gil::write_view(output, boost::gil::const_view(*merged_output), boost::gil::png_tag());

			return 0;
		}

		std::shared_ptr<boost::gil::image<boost::gil::rgba8_pixel_t,
			true,
			mmap_allocator<unsigned char> > > merge_from_file(const std::string& input) {

			auto raw_name = path::raw_name(input);

			// open the archive
			std::ifstream input_file(input, std::ios::binary);
			// copies all data into buffer
			std::string z_data((
				std::istreambuf_iterator<char>(input_file)),
				(std::istreambuf_iterator<char>()));

			auto archive_data = z::decompress_from(z_data);
			
			std::stringstream archive_input_stream;

			archive_input_stream << archive_data;

			boost::archive::binary_iarchive input_archive(archive_input_stream);

			wxt_image container(0, 0);

			// restore the wxt_image from the archive
			input_archive >> container;

			auto merged_output = std::make_shared<
				boost::gil::image<boost::gil::rgba8_pixel_t,
				true,
				mmap_allocator<unsigned char> > >(container.width(), container.height());
			
			for (auto segment : container.segments_vector()) {

				boost::gil::image
					<boost::gil::rgba8_pixel_t,
					true,
					mmap_allocator<unsigned char> > img_segment;
				try {

					/*boost::filesystem::path p(input);

					auto directory = p.parent_path();

					auto segment_path = raw_name +
						"_" +
						std::to_string(segment.idx_y()) +
						"_" +
						std::to_string(segment.idx_x()) +
						".png";

					directory /= segment_path;

					boost::gil::read_image(directory.string(), img_segment, boost::gil::png_tag());*/

//@NOTE decompression from each segment data should be done here if compression was done like that
					std::stringstream segment_input_stream;
						
					segment_input_stream << segment.data();

					boost::gil::read_image(segment_input_stream, img_segment, boost::gil::png_tag());

				}
				catch (boost::exception & ex) {
					std::cerr << boost::diagnostic_information_what(ex) << std::endl;
				}

				boost::gil::point2<ptrdiff_t> tile_location(segment.x(), segment.y());
				boost::gil::point2<ptrdiff_t> tile_dimensions(img_segment.width(), img_segment.height());


				auto view = boost::gil::subimage_view(boost::gil::view(*merged_output), tile_location, tile_dimensions);

				boost::gil::copy_pixels(boost::gil::const_view(img_segment), view);
			}

			return merged_output;
		}
	};

}

#endif