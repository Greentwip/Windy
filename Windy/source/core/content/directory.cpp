#include "core/content/directory.hpp"

#include "core/filesystem/path.hpp"

windy::content::directory::directory(const std::string& path,
			const std::vector<std::string>& folders) {
	this->_path = path;

	for (auto folder : folders) {
		path::directory(path, folder);
	}
}

bool windy::content::directory::make(const std::string& directory, const std::string& folder) {
	return path::directory(this->folder(directory), folder);
}

bool windy::content::directory::store(const std::string& file,
			const std::string& directory, 
			const std::string& name, 
			const std::string& extension) {
	auto path = path::join(this->folder(directory), 
							name + "." + extension);
	return path::copy(file, path);
}

std::string windy::content::directory::retrieve(const std::string& directory,
						const std::string& name, 
						const std::string& extension) {
	return path::join(folder(directory), name + "." + extension);
}

std::string windy::content::directory::folder(const std::string& directory) {
	return path::join(this->path(), directory);
}

bool windy::content::directory::remove(const std::string& directory,
					const std::string& name,
					const std::string& extension) {
	auto path = this->retrieve(directory, name, extension);
	return path::remove(path);
}

bool windy::content::directory::remove_all(const std::string& directory, const std::string& folder) {
	auto path = path::join(this->folder(directory), folder);
	return path::remove_all(path);
}

std::string windy::content::directory::path() {
	return this->_path;
}
