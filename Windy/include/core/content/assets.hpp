#ifndef windy_assets
#define windy_assets

#include <memory>
#include <string>
#include <vector>

#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/listbox.hpp"

namespace windy {
	namespace content {
		class sprite;
		class group;
		class composition;
	}

	namespace content {
		class assets : public nana::group {
			enum class import_kind {
				background,
				animation
			};

		public:
			assets();
			void init();
			void construct(std::shared_ptr<class directory> directory, 
						   std::shared_ptr<class collection> collection, 
						   std::shared_ptr<class instance_creator> instance_creator);
			auto directory();
			void refresh();
			void on_item_selected(const nana::arg_listbox& arg);
			void on_mouse_down(const nana::arg_mouse& arg);
			void on_mouse_move(const nana::arg_mouse& arg);
			void on_mouse_up(const nana::arg_mouse& arg);
			void on_mouse_drop_files(const nana::arg_dropfiles& arg);
			void import(const std::vector<std::string>& files);
			bool verify_import(const std::vector<std::string>& files, 
							   const import_kind& kind);
			void import(const std::vector<std::string>& files, 
						const std::string& name,
						const import_kind& kind);
			void remove_sprite(std::shared_ptr<class content::sprite> sprite);
			void remove_group(std::shared_ptr<class content::group> group);
			void remove_composition(std::shared_ptr<class content::composition> composition);
			std::shared_ptr<class collection> collection();

		private:
			std::shared_ptr<class directory>			_directory;
			std::shared_ptr<class collection>			_collection;
			std::shared_ptr<class instance_creator>		_instance_creator;

		private:
			nana::place _place;
			nana::label _label;
			nana::listbox _content;
		};
	}
}

#endif