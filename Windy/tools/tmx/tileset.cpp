#include "tileset.hpp"

#include "boost/gil/extension/numeric/sampler.hpp"

#include "boost/gil/image.hpp"
#include "boost/gil/typedefs.hpp"
#include "boost/gil/extension/io_new/png_all.hpp"
#include "boost/gil/extension/numeric/sampler.hpp"
#include "boost/gil/extension/numeric/resample.hpp"



windy::tileset::tileset(const unsigned int& first_gid,
						const unsigned int& tile_size,
						const unsigned int& margin,
						const unsigned int& spacing,
						const unsigned int& matrix_size,
						const unsigned int& texture_size) {

	this->column_index = 0;
	this->row_index = 0;

	this->first_gid = first_gid;
	this->tile_size = tile_size;
	this->margin = margin;
	this->spacing = spacing;
	this->matrix_size = matrix_size;
	this->texture_size = texture_size;

	unsigned int validation = validate_tileset();

	texture = std::make_unique<boost::gil::image<boost::gil::rgba8c_planar_loc_t, true> >
		(this->texture_size, this->texture_size);
	
}


void windy::tileset::blit_tile(const boost::gil::image_view<boost::gil::rgba8c_planar_loc_t>& tile) {


	boost::gil::point2<ptrdiff_t> tile_location(0, 0);
	boost::gil::point2<ptrdiff_t> tile_dimensions(this->tile_size, this->tile_size);

	tile_location.x = this->column_index * this->tile_size;
	tile_location.y = this->row_index * this->tile_size;

	//x_pos += this->tile_offset;
	//y_pos += this->tile_offset;

	if (this->column_index == 0) {
		tile_location.x += this->margin * this->column_index + 1 * 2;
	}
	else {
		tile_location.x += this->margin;
	}

	if (this->row_index == 0) {
		tile_location.y += this->margin * this->row_index + 1 * 2;
	}
	else {
		tile_location.y += this->margin;
	}

	tile_location.x += this->margin * this->column_index * 2;
	tile_location.y += this->margin * this->row_index * 2;


    auto view = boost::gil::subimage_view(boost::gil::view(*texture), tile_location, tile_dimensions);
	boost::gil::copy_pixels(tile, view);
		

	++this->column_index;

	if (this->column_index == this->matrix_size) {
		this->row_index++;
		this->column_index = 0;
	}

	assert(this->row_index <= this->matrix_size);

}


void windy::tileset::extrude() {
	/*
	unsigned int extrude_column_index = 0;
	unsigned int extrude_row_index = 0;

	for (unsigned int i = 0; i < this->matrix_size * this->matrix_size; ++i) {

	++extrude_column_index;

	if (extrude_column_index == this->matrix_size) {
	extrude_row_index++;
	extrude_column_index = 0;
	}

	assert(extrude_row_index <= this->matrix_size);


	}


	for (unsigned int y = 0; y < texture->GetHeight(); ++y) {
	for (unsigned int x = 0; x < texture->GetWidth(); ++x) {

	}
	}*/
}

void windy::tileset::save(const std::string& path) {
	boost::gil::write_view(path, boost::gil::const_view(*texture), boost::gil::png_tag());
}

unsigned int windy::tileset::validate_tileset() {

	// a suitable texture is one of pot size that can be divided by tile_size
	// a suitable tile_size is one of pot size too
	assert(this->texture_size % this->tile_size == 0);

	unsigned int spaced_tile_size = this->tile_size + spacing;

	this->matrix_size = (unsigned int)std::floor(double(this->texture_size) / double(spaced_tile_size));

	// tilemap textures can't have 0 tiles, (tile_size == texture_size would be common here)
	assert(matrix_size != 0);

	// the margin must be an even number
	assert((this->texture_size - spaced_tile_size * this->matrix_size) % 2 == 0);

	return (this->texture_size - spaced_tile_size * this->matrix_size) / 2;
};

