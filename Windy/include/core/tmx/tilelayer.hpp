#ifndef windy_tilelayer
#define windy_tilelayer

namespace windy{
	class tilelayer {
	public:
		tilelayer(const unsigned int& width,
			const unsigned int& height);

		unsigned int width;
		unsigned int height;

		std::vector<uint32_t> index_keys;
	};
}

#endif