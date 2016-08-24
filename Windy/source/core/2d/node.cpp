#include "core/2d/node.hpp"

windy::node::node() : _scale(1.0f), _anchor(0.5f, 0.5f) {

}

windy::node::node(const std::string& name) : 
	_scale(1.0f), _anchor(0.5f, 0.5f) {
	this->name(name);
}

std::string windy::node::name() {
	return this->_name;
}

void windy::node::name(const std::string& name) {
	this->_name = name;
}

windy::anchor windy::node::anchor() {
	return this->_anchor;
}

void windy::node::anchor(const class windy::anchor anchor) {
	this->_anchor = anchor;
}

nana::point windy::node::location(location_kind kind) {

	nana::point location = this->_location;

	switch (kind) {
		case location_kind::local:
		break;
		case location_kind::global: {
			if (this->parent()) {
				nana::point parent_location
					= this->parent()->location(windy::node::location_kind::global);

				location = parent_location + location;
			}
		}
		break;
	}

	return location;
}

void windy::node::location(const nana::point& location) {
	this->_location = location;
}

float windy::node::scale() {
	return this->_scale;
}

nana::point windy::node::zoom_focus() {
	return this->_zoom_focus;
}

void windy::node::zoom(const nana::point& focus, const float& scale) {

	if (scale != this->_scale) {
		this->_scale = scale;
	}

	if (focus.x != this->_zoom_focus.x || focus.y != this->_zoom_focus.y) {
		this->_zoom_focus = focus;
	}

	for (auto child : this->_children) {
		child->zoom(focus, scale);
	}
}

nana::size windy::node::size() {
	return nana::size(this->_size.width, this->_size.height);
}

void windy::node::size(const nana::size& size) {
	this->_size = size;
}

void windy::node::add_child(std::shared_ptr<node> child) {
	child->parent(shared_from_this());
	this->_children.push_back(child);
}

bool windy::node::remove_child_by_name(const std::string& name) {
	bool searching = true;
	bool success = false;

	for (unsigned int i = 0; i < this->_children.size() && searching; ++i) {
		if (this->_children[i]->name() == name) {
			this->_children.erase(this->_children.begin() + i);
			searching = false;
			success = true;
		}
	}

	return success;
}

std::vector<std::shared_ptr<windy::node> >& windy::node::children() {
	return this->_children;
}

void windy::node::update(const float& delta) {

}

const std::shared_ptr<windy::node> windy::node::parent() {
	return this->_parent;
}


void windy::node::parent(std::shared_ptr<node> parent) {
	this->_parent = parent;
}
 