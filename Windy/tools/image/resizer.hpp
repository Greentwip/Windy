#include <vector>
#include <string>

#include "boost/gil/image.hpp"
#include "boost/gil/typedefs.hpp"
#include "boost/gil/extension/io/png_io.hpp"
#include "boost/gil/extension/numeric/sampler.hpp"
#include "boost/gil/extension/numeric/resample.hpp"

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp" 

#include "boost/lexical_cast.hpp"

#include "tools/memory/mmap_allocator.hpp"

namespace windy {

	class resizer {
	public:
		int run(const std::vector<std::string>& arguments) {

			auto input = arguments[0];
			auto output = arguments[1];

			int design_resolution = atoi(arguments[2].c_str());
			int target_design_resolution = atoi(arguments[3].c_str());

			boost::gil::image
			<boost::gil::rgba8_pixel_t,
			true,
			mmap_allocator<unsigned char> > img;

			try {
				boost::gil::png_read_image(input, img);
			}
			catch (boost::exception & ex) {
				std::cerr << boost::diagnostic_information_what(ex) << std::endl;
			}

			unsigned int scale = unsigned int(std::ceil(1920.00 / double(design_resolution)));

			long long width = img.width() * scale;
			long long height = img.height() * scale;

			boost::gil::image
				<boost::gil::rgba8_pixel_t,
				true,
				mmap_allocator<unsigned char> > hd_tex(width, height);

			boost::gil::resize_view(boost::gil::const_view(img),
									boost::gil::view(hd_tex),
									boost::gil::nearest_neighbor_sampler());

			boost::gil::png_write_view(output, boost::gil::const_view(hd_tex));

			return 0;
		}
	};


}