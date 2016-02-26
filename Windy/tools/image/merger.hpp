#ifndef windy_merger
#define windy_merger

#include <vector>
#include <string>
#include <memory>

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
#include "tools/filesystem/path.hpp"


namespace windy {

	class merger {
	public:
		int run(const std::vector<std::string>& arguments) {

			auto input = arguments[0];
			auto output = arguments[1];

			auto merged_output = merge_from_file(input);

			boost::gil::png_write_view(output, boost::gil::const_view(*merged_output));

			return 0;
		}

		std::shared_ptr<boost::gil::image<boost::gil::rgba8_pixel_t,
			true,
			mmap_allocator<unsigned char> > > merge_from_file(const std::string& input) {

			auto raw_name = path::raw_name(input);

			// open the archive
			std::ifstream ifs(input);
			boost::archive::text_iarchive ia(ifs);

			wxt_image container(0, 0);

			// restore the wxt_image from the archive
			ia >> container;

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

					boost::filesystem::path p(input);

					auto directory = p.parent_path();

					auto segment_path = raw_name +
						"_" +
						std::to_string(segment.idx_y()) +
						"_" +
						std::to_string(segment.idx_x()) +
						".png";

					directory /= segment_path;

					boost::gil::png_read_image(directory.string(), img_segment);
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