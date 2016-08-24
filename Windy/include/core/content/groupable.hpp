#ifndef windy_groupable
#define windy_groupable

#include <memory>
#include <vector>

#include "nana/basic_types.hpp"

#include "core/content/instance.hpp"

namespace windy {

	namespace entity {
		class texture;
	}

	namespace animation {
		class frame_set;
		class trigger;
	}

	namespace content {
		class groupable : public instance {
		public:
			groupable();
			std::shared_ptr<entity::texture> texture();
			auto& textures();
			void textures(const std::vector<std::shared_ptr<entity::texture> >& textures);
			void frame_set(std::shared_ptr<animation::frame_set> frame_set);
			std::shared_ptr<animation::frame_set> frame_set();
			std::vector<std::shared_ptr<class animation::trigger> >& triggers();
			nana::size size() override;

		private:
			std::vector<std::shared_ptr<animation::trigger> > _triggers;
			std::vector<std::shared_ptr<entity::texture> > _textures;
			std::shared_ptr<animation::frame_set> _frame_set;
		};

	}
}

#endif