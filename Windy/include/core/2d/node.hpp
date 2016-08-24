#ifndef windy_node
#define windy_node

#include <memory>
#include <string> 
#include <vector>

#include "nana/basic_types.hpp"

namespace windy {
	class anchor {
	public:
		anchor(const float& x, const float& y) {
			this->x = x;
			this->y = y;
		}

		anchor() {
			x = 0.0f;
			y = 0.0f;
		}

		float x;
		float y;
	};

	class node : public std::enable_shared_from_this<node> {
		// default anchor is 0.5, 0.5 
		// @TODO make anchors
		// @TODO inverse y
		// @TODO for exports, anchor and positions shall be inverse

		// inverse y is calculated for every property for exports
		/*
		auto layer = instance->parent();	// top parent

		auto container = layer->parent();	// stub

		auto instance_location = instance->location();
		auto instance_anchor = instance->anchor();
		auto instance_size = instance->size();

		instance_location.y = container->size().height - instance_location.y;

		auto anchor_point_in_points =
		nana::point(instance_size.width * instance_anchor.x, instance_size.height * instance_anchor.y);

		anchor_point_in_points.y = instance_size.height - anchor_point_in_points.y;

		instance_anchor.x = float(anchor_point_in_points.x) / float(instance_size.width);
		instance_anchor.y = float(anchor_point_in_points.y) / float(instance_size.height);
		*/
	public:
		enum class location_kind {
			local,
			global
		};

	public:
		node();
		node(const std::string& name);

	public:
		std::string name();
		void name(const std::string& name);
		anchor anchor();
		void anchor(const class anchor anchor);
		nana::point location(location_kind kind = location_kind::local);
		void location(const nana::point& location);
		float scale();
		nana::point zoom_focus();
		void zoom(const nana::point& focus, const float& scale);
		virtual nana::size size();
		virtual void size(const nana::size& size);
		void add_child(std::shared_ptr<node> child);
		bool remove_child_by_name(const std::string& name);
		std::vector<std::shared_ptr<node> >& children();
		virtual void update(const float& delta);
		const std::shared_ptr<node> parent();

	private:
		void parent(std::shared_ptr<node> parent);

	private:
		std::shared_ptr<node> _parent;

	protected:
		std::string _name;

		class anchor _anchor;
		nana::point _location;
		nana::size	_size;

		float		_scale;
		nana::point _zoom_focus;

		std::vector<std::shared_ptr<node> > _children;

	};
}

#endif