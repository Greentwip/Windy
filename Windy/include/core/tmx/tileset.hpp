#ifndef windy_tileset
#define windy_tileset

namespace windy{
	class tileset {
		typedef boost::gil::image_view<boost::gil::rgba8c_planar_loc_t> tile_image_view;
		typedef boost::gil::image<boost::gil::rgba8c_planar_loc_t, true> tile_image;

	public:
		tileset(const unsigned int& first_gid, 
				const unsigned int& tile_size, 
				const unsigned int& margin, 
				const unsigned int& spacing, 
				const unsigned int& matrix_size, 
				const unsigned int& texture_size);
	
		void blit_tile(const tile_image_view& tile);
		void extrude();
		void save(const std::string& path);

	private:
		unsigned int validate_tileset();

	private:
		unsigned int column_index;
		unsigned int row_index;

		std::unique_ptr<tile_image> texture;
		//std::unique_ptr<Gdiplus::Graphics> graphics;

	public:
		unsigned int first_gid;
		unsigned int tile_size;
		unsigned int texture_size;
		unsigned int spacing;
		unsigned int margin;
		unsigned int matrix_size;


	};
}

#endif

