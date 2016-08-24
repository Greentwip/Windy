#include "core/2d/texture.hpp"

#include "nana/paint/graphics.hpp"

#include "boost/gil/extension/io_new/png_all.hpp"

#include "core/image/pixel_inserters.hpp"

windy::entity::texture::texture() {

}

bool windy::entity::texture::load(const std::string& path) {

	this->_image = std::make_shared <mmap_image>();

	bool fine = false;

	try {
		boost::gil::read_image(path, this->_image->raw(), boost::gil::png_tag());
	}
	catch (std::exception&) {
		//std::cerr << boost::diagnostic_information_what(ex) << std::endl;
		fine = false;
	}

	return fine;
}

std::shared_ptr<nana::paint::graphics> windy::entity::texture::clip(const nana::rectangle& in_rectangle,	
								  bool stretch_to_scale,	
								  float scale) {
	auto graphics = std::make_shared<nana::paint::graphics>();

	if (this->_image == nullptr) {
		assert(false); // you need to init the graphic first
	}
	else {

		auto out_rectangle = in_rectangle;

		if (out_rectangle.empty()) {
			assert(false); // the rectangle must contain valid information
		}

		// the clipping rectangle must be inside the image graphic bounds
		// assert(out_rectangle.right() <=  )

		auto image_view = boost::gil::view(this->_image->raw());
		auto clipped_region =
			boost::gil::subimage_view(image_view,
			{ out_rectangle.x, out_rectangle.y },
			{ out_rectangle.width, out_rectangle.height });

		nana::paint::pixel_buffer pixel_buffer(out_rectangle.width, out_rectangle.height);

		pixel_buffer.alpha_channel(true);

		auto argb_ptr = pixel_buffer.raw_ptr(0);

		pixel_count_wrapper wrapper;
		boost::gil::for_each_pixel(clipped_region,
			PixelRGBAInserter<nana::pixel_color_t>(argb_ptr,
				&wrapper));

		if (stretch_to_scale) {
			graphics->make(nana::size(nana::size::value_type(out_rectangle.width * scale),
				nana::size::value_type(out_rectangle.height * scale)));


			graphics->set_color(nana::color(0, 0, 0, 0));

			nana::rectangle source_rectangle(0,
				0,
				out_rectangle.width,
				out_rectangle.height);

			nana::rectangle destination_rectangle(0,
				0,
				graphics->width(),
				graphics->height());

			pixel_buffer.stretch(source_rectangle, graphics->handle(), destination_rectangle);

		}
		else {
			graphics->make(nana::size(out_rectangle.width, out_rectangle.height));
			pixel_buffer.paste(graphics->handle(), nana::point(0, 0));
		}

	}

	return graphics;
}

nana::size windy::entity::texture::size() {
	return nana::size(this->_image->data()->width(), this->_image->data()->height());
}
 