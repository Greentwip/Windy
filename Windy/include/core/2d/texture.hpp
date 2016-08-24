#ifndef windy_texture
#define windy_texture

#include <string>
#include <memory>

#include "nana/basic_types.hpp"

#include "core/image/mmap_image.hpp"

//@TODO namespace nana class declaration
#include "nana/paint/graphics.hpp"

namespace windy {
	namespace entity {
		class texture {
		public:
			texture();
			bool load(const std::string& path);
			std::shared_ptr<nana::paint::graphics> clip(const nana::rectangle& in_rectangle = nana::rectangle(0, 0, 0, 0),
					  bool stretch_to_scale = false,
					  float scale = 1.0f);
			nana::size size();

		private:
			std::shared_ptr<mmap_image> _image;
		};
	}
}

#endif