#ifndef windy_tileset
#define windy_tileset

#include <cmath>

#include <memory>
#include <string>
#include <cassert>

#include "boost/gil/image.hpp"
#include "boost/gil/typedefs.hpp"

namespace windy{
	class tileset;
}

class windy::tileset {
public:

	tileset(const unsigned int& first_gid, 
			const unsigned int& tile_size, 
			const unsigned int& margin, 
			const unsigned int& spacing, 
			const unsigned int& matrix_size, 
			const unsigned int& texture_size);
	
	void blit_tile(const boost::gil::image_view<boost::gil::rgba8c_planar_loc_t>& tile);
	void extrude();
	void save(const std::string& path);

private:
	unsigned int validate_tileset();


private:
	unsigned int column_index;
	unsigned int row_index;

	std::unique_ptr<boost::gil::image<boost::gil::rgba8c_planar_loc_t, true> > texture;
	//std::unique_ptr<Gdiplus::Graphics> graphics;

public:
	unsigned int first_gid;
	unsigned int tile_size;
	unsigned int texture_size;
	unsigned int spacing;
	unsigned int margin;
	unsigned int matrix_size;


};
 

#endif

