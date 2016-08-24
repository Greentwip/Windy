#include <vector>

#include "core/filesystem/path.hpp"

#include "boost/filesystem.hpp"

#include "core/number/uuid.hpp"

std::string windy::path::file_name(const std::string& input) {
	std::vector<std::string> tokens;

	std::string buffer;
	for (auto character : input) {

		if (character == '\\') {
			tokens.push_back(buffer);
			buffer.clear();
		}
		else {
			buffer.push_back(character);
		}
	}

	tokens.push_back(buffer);

	auto back = tokens.back();

	return back;
}

std::string windy::path::raw_name(const std::string& input){
	auto name = path::file_name(input);
	return std::string(name.begin(), name.end() - 4);
}

std::string windy::path::extension(const std::string& input) {
	auto name = path::file_name(input);
	auto position = name.find_last_of('.');
	std::string ext = "";
	if (position != std::string::npos) {
		ext = std::string(name.begin() + position, name.end());
	}
	return ext;
}

std::string windy::path::generate_temp_path() {

	auto id = uuid::generate();
	auto temp_path = boost::filesystem::temp_directory_path();
	temp_path /= id;
	auto path = temp_path.string();
	return path;

}

bool windy::path::remove(const std::string& path) {
	boost::system::error_code ec;

	boost::filesystem::remove(path, ec);

	return ec.value() == 0;
}

bool windy::path::copy(const std::string& from, const std::string& to) {
	boost::system::error_code ec;
	boost::filesystem::copy({ from }, { to }, ec);
	return ec.value() == 0;
}

std::string windy::path::directory(const std::string& file) {
	boost::filesystem::path path(file);

	return path.parent_path().string();
}

bool windy::path::directory(const std::string& directory, const std::string& folder) {
	boost::filesystem::path path(directory);
	path /= folder;
	return boost::filesystem::create_directory(path);
}

std::string windy::path::join(const std::string& directory, const std::string& file) {
	boost::filesystem::path path(directory);
	path /= file;
	return path.string();
}

bool windy::path::remove_all(const std::string& directory) {
	boost::filesystem::path path(directory);

	boost::system::error_code ec;

	boost::filesystem::remove_all(path, ec);

	return ec.value() == 0;
}
	