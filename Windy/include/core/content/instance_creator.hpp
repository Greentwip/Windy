#ifndef windy_instance_creator
#define windy_instance_creator

#include <functional>
#include <memory>
#include <vector>

#include "nana/gui/widgets/label.hpp"

#include "core/2d/layer.hpp"

namespace windy {
	namespace content {
		class instance_creator : public nana::label {

			typedef std::function<bool(const nana::point&)> check_callback;
			typedef std::function<void(std::shared_ptr<class instance>, const nana::point&)>
				bake_callback;
		public:
			instance_creator(check_callback check_callback, bake_callback bake_callback);
			void bake_instance(const nana::point& location);
			void dragging(const bool& dragging);
			bool dragging();
			void items(std::vector<std::shared_ptr<class item> > items);
			std::vector<std::shared_ptr<class item> >& items();
			void category(layer::kind category);
			layer::kind category();

		private:
			layer::kind _category;

			bool _dragging;
			std::function<bool(const nana::point&)> _instance_validation_callback;
			std::function<void(std::shared_ptr<class instance>, const nana::point&)> _instance_bake_callback;
			std::vector<std::shared_ptr<class item> > _items;
		};
	}
}

#endif