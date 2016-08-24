#ifndef windy_tilemap
#define windy_tilemap

namespace windy {
	class tilemap {
	public:
		tilemap(const std::string& raw_name,
			const unsigned int& width,
			const unsigned int& height,
			const unsigned int& tile_size = 32,
			const unsigned int& texture_size = 1024);

	public:
		int build_tmx(const std::string& out_path, const std::string& map_index);

	private:
		unsigned int validate_tileset();
		std::string compress(const std::vector<uint32_t>& input, const uint64_t& width, const uint64_t& height);

	public:
		std::string _raw_name;

		unsigned int _width;
		unsigned int _height;

		unsigned int _tile_size;
		unsigned int _texture_size;
		unsigned int _spacing;
		unsigned int _margin;
		unsigned int _matrix_size;

		std::map<std::shared_ptr<tileset>, std::shared_ptr<tilelayer> >  _tile_layers;
		std::vector<std::shared_ptr<tileset> > _tile_sets;
	};
}

#endif

