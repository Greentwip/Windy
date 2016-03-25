#ifndef windy_resizer
#define windy_resizer 

#include <vector>
#include <string>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

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

#include "tools/image/mmap_image.hpp"

namespace windy {

	class resizer {
	public:
		int run(const std::vector<std::string>& arguments) {

			auto input = arguments[0];
			auto output = arguments[1];

			int design_resolution = atoi(arguments[2].c_str());
			int target_design_resolution = atoi(arguments[3].c_str());

			auto image = std::make_shared<mmap_image>();

			try {
				boost::gil::read_image(input, image->raw(), boost::gil::png_tag());
			}
			catch (boost::exception & ex) {
				std::cerr << boost::diagnostic_information_what(ex) << std::endl;
			}

			double scale = double(target_design_resolution) / double(design_resolution);

			auto width = uint64_t(std::ceil(long double(image->raw().width()) * scale));
			auto height = uint64_t(std::ceil(long double(image->raw().height()) * scale));

			// this defaults to the minimum tile size we can process with good performance

			auto tile_size = 8;

			width = uint64_t(std::ceil(long double(width) / double(tile_size)) * tile_size);
			height = uint64_t(std::ceil(long double(height) / double(tile_size)) * tile_size);

			boost::gil::image
				<boost::gil::rgba8_pixel_t,
				true,
				mmap_allocator<unsigned char> > hd_tex(width, height);

			boost::gil::resize_view(boost::gil::const_view(image->raw()),
									boost::gil::view(hd_tex),
									boost::gil::nearest_neighbor_sampler());

			boost::gil::write_view(output, boost::gil::const_view(hd_tex), boost::gil::png_tag());

			return 0;
		}
	};

}

#endif