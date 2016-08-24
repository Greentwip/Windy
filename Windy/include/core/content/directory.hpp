#ifndef windy_directory
#define windy_directory

#include <string>
#include <vector>

namespace windy {
	namespace content {
		class directory {
		public:
			directory(const std::string& path, 
					  const std::vector<std::string>& folders);
			bool make(const std::string& directory, const std::string& folder);
			bool store(const std::string& file, 
					   const std::string& directory, 
					   const std::string& name, 
					   const std::string& extension);
			std::string retrieve(const std::string& directory,
									const std::string& name, 
									const std::string& extension);
			std::string folder(const std::string& directory);
			bool remove(const std::string& directory,
						const std::string& name,
						const std::string& extension);
			bool remove_all(const std::string& directory, const std::string& folder);
			
		private:
			std::string path();

		private:
			std::string _path;
		};
	}
}

#endif