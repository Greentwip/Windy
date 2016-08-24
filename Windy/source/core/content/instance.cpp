#include "core/content/instance.hpp"

#include "core/2d/layer.hpp"
#include "core/2d/texture.hpp"

windy::content::instance::instance() :
	_kind(layer::kind::none) {
}

windy::layer::kind windy::content::instance::kind() {
	return this->_kind;
}

void windy::content::instance::kind(layer::kind kind) {
	this->_kind = kind;
}

std::string windy::content::instance::asset_uuid() {
	return this->_asset_uuid;
}

void windy::content::instance::asset_uuid(const std::string& asset_uuid) {
	this->_asset_uuid = asset_uuid;
}