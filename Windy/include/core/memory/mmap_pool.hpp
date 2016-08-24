#ifndef windy_mmap_pool
#define windy_mmap_pool

#include <memory>
#include <string>
#include <map>

namespace windy {
	template <typename T>
	class mmap_pool : public std::allocator<T> {
	public:
		typedef T value_type;

		template<typename _Tp1>
		struct rebind
		{
			typedef mmap_pool<_Tp1> other;
		};

		// default memory mapped pool size is 512 mb per mapping
		mmap_pool(std::size_t default_mapping_size = (512 << 20)){

			assert(default_mapping_size > 0);

			_consumed_bytes = 0;
			_default_mapping_size = default_mapping_size;
			map_pool(_default_mapping_size);
		}

		~mmap_pool() {

		}
		template <class U>
		mmap_pool(mmap_pool<U> const & other) {

			this->_consumed_bytes = other._consumed_bytes;
			this->_default_mapping_size = other._default_mapping_size;
			this->_current_mmapped_file = other._current_mmapped_file;
			this->_mapped_files = other._mapped_files;
		}

		T* address(T& r) const
		{
			auto x = 20;
			return &r;
		}

		T* allocate(std::size_t n) {

			bool needs_allocation = false;

			if (this->_consumed_bytes + n > _current_mmapped_file->size()) {
				needs_allocation = true;
			}

			if (needs_allocation) {
				if (n > this->_default_mapping_size) {
					map_pool(n);
				} else {
					map_pool(this->_default_mapping_size);
				}
				this->_consumed_bytes = 0;
			}

			T* current_ptr = 
				reinterpret_cast<T*>(_current_mmapped_file->data() + this->_consumed_bytes);

			this->_consumed_bytes += n;

			return current_ptr;
		}

		void deallocate(T* p, std::size_t n) {

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

					//@TODO erase the mapped file from the map 

				}

			}
		}

		void construct(T* p, const T& val){
			new (p) T(val);
		}

		void destroy(T* p){
			p->~T();
		}
		
		std::size_t max_size() const {
			return ULONG_MAX / sizeof(T);
		}

		void cleanup();

		T* data() {
			return reinterpret_cast<T*>(_current_mmapped_file->data());
		}

	private:
		T* map_pool(std::size_t n) {

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

			_current_mmapped_file = std::make_shared<boost::iostreams::mapped_file_sink>();

			_mapped_files[path] = _current_mmapped_file;

			try {
				_current_mmapped_file->open(param);
			}
			catch (boost::exception& e) {
				std::cerr << boost::diagnostic_information_what(e) << std::endl;
			}

			return reinterpret_cast<T*>(_current_mmapped_file->data());
		}

	public:
		std::string _file_name;
		std::size_t _default_mapping_size;
		std::size_t _consumed_bytes;

		typedef std::shared_ptr<boost::iostreams::mapped_file_sink> mapped_file_sink_ptr;
		
		std::map<std::string, mapped_file_sink_ptr>::iterator mapped_files_it;
		std::map<std::string, mapped_file_sink_ptr> _mapped_files;
		mapped_file_sink_ptr _current_mmapped_file;

	};
}

#endif 