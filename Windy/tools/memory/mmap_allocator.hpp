#ifndef windy_mmap_allocator
#define windy_mmap_allocator

#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "boost/iostreams/device/file.hpp"
#include "boost/iostreams/device/mapped_file.hpp"
#include "boost/exception/diagnostic_information.hpp"

#include "boost/filesystem.hpp"

#include "tools/number/uuid.hpp"

namespace windy {
	template <typename T>
	class mmap_allocator : public std::allocator<T> {
	public:

		template<typename _Tp1>
		struct rebind
		{
			typedef mmap_allocator<_Tp1> other;
		};

		mmap_allocator() {

		}

		T* allocate(std::size_t n) {

			auto id = uuid::generate();

			auto temp_path = boost::filesystem::temp_directory_path();

			temp_path /= id;

			auto path = temp_path.string();

			boost::iostreams::file_sink out(path,
				BOOST_IOS::binary || BOOST_IOS::trunc);

			boost::iostreams::seek(out, n, BOOST_IOS::end);
			out.write("", 1);
			out.close();

			boost::iostreams::mapped_file_params param; // template param

			param.length = n;
			param.path = path;

			auto mapped_file = std::make_shared<boost::iostreams::mapped_file_sink>();

			_mapped_files[path] = mapped_file;

			try {
				mapped_file->open(param);
			}
			catch (boost::exception& e) {
				std::cerr << boost::diagnostic_information_what(e) << std::endl;
			}

			return reinterpret_cast<T*>(mapped_file->data());
		}

		void deallocate(T* p, size_t n) {

			bool searching = true;

			for (mapped_files_it it = _mapped_files.begin();
			it != _mapped_files.end() && searching;
				++it) {
				auto file = it->second;

				auto file_data = reinterpret_cast<T*>(file->data());
				if (file_data == p) {
					searching = false;
					p = nullptr;
					file->close();

					boost::filesystem::remove(it->first);
				}

			}
		}

	private:
		std::string _file_name;

		typedef std::map<std::string,
			std::shared_ptr<boost::iostreams::mapped_file_sink> >::iterator mapped_files_it;

		std::map<std::string,
			std::shared_ptr<boost::iostreams::mapped_file_sink> > _mapped_files;

	};
}

#endif