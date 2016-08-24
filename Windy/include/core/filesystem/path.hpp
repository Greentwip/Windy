#ifndef windy_filesystem
#define windy_filesystem

#include <string>

namespace windy {
	class path {
	public:
		static std::string file_name(const std::string& input);
		static std::string raw_name(const std::string& input);
		static std::string extension(const std::string& input);
		static std::string generate_temp_path();
		static bool remove(const std::string& path);
		static bool copy(const std::string& from, const std::string& to);
		static std::string directory(const std::string& file);
		static bool directory(const std::string& directory, const std::string& folder);
		static std::string join(const std::string& directory, const std::string& file);
		static bool remove_all(const std::string& directory);
	};
}

#endif