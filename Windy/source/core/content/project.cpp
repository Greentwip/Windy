#include <sstream>

#include "nana/gui/detail/basic_window.hpp"

#include "core/content/project.hpp"

#include "core/content/assets.hpp"
#include "core/content/game.hpp"

#include "core/content/directory.hpp"

#include "core/content/collection.hpp"
#include "core/content/container.hpp"

#include "core/filesystem/path.hpp"

windy::content::project::project() {
	this->_assets	= std::make_shared<class assets>();
	this->_game		= std::make_shared<class game>();
}

void windy::content::project::init() {
		this->_place.bind(*this);

		auto div = std::stringstream();

		div << ""	
				<< "<vertical"
					<< "<weight=16"
						<< "<label>"
					<< ">"
					<< "<"
						<< "<weight=50% assets>"
					<< ">"
					<< "<"
						<< "<game>"
					<< ">"
				<< ">"
			<< "";

		this->_place.div(div.str().c_str());
					
		this->_label.create(*this);
		this->_assets->create(*this);
		this->_game->create(*this);

		std::string content_caption = "\tProject";
		this->_label.caption(nana::string(content_caption.begin(), content_caption.end()));
		this->_label.bgcolor(nana::colors::dark_gray);

		this->_place["label"]	<< this->_label.handle();
		this->_place["assets"]	<< this->_assets->handle();
		this->_place["game"]	<< this->_game->handle();

		this->_place.collocate();

		// init groups
		this->_assets->init();
		this->_game->init();
}

bool windy::content::project::construct(const std::string& path,
				std::shared_ptr<class instance_creator> instance_creator) {


	/*			auto fiore = std::make_shared<background>("fiore", "c:/local/flower/fiore0000.png");
	auto fiore2 = std::make_shared<background>("fiore2", "c:/local/flower/fiore0025.png");

	animation_layer->add_child(fiore);
	animation_layer->add_child(fiore2);

	fiore->z_order(0);
	fiore2->z_order(1);

	fiore2->location(nana::point(120, 95));
	*/

	// assets

	// assets_directory path is relative to project info parent path file location
	auto parent_path = path::directory(path);

	std::vector<std::string> folders = {
		"Backgrounds",
		"Animations",
		"Compositions"
	};

	auto directory = std::make_shared<class directory>(parent_path, folders);

	// let's load the assets from the project info file

	auto assets_collection = std::make_shared<class collection>();

	this->_assets->construct(directory,
							 assets_collection, 
							 instance_creator);

	// game, per container
	auto container = std::make_shared<class container>();

	container->bake();
	container->name("main");

	this->_game->add_container(container);
	this->_game->container("main");

	this->_game->container()->size(nana::size(852, 480));

	auto parent = reinterpret_cast<nana::detail::basic_window*>(this->parent());

	nana::point container_location;
	container_location.x =  int(parent->dimension.width * 0.5 -
								this->_game->container()->size().width * 0.5);

	container_location.y =	int(parent->dimension.height * 0.5 -
								this->_game->container()->size().height * 0.5);

	this->_game->container()->anchor({ 0, 0 });

	this->_game->container()->location(container_location);
				

	return true;
}

void windy::content::project::open(const std::string& path) {

}

void windy::content::project::close() {

}

std::shared_ptr<class windy::content::assets>	windy::content::project::assets() {
	return this->_assets;
}

std::shared_ptr<class windy::content::game>	windy::content::project::game() {
	return this->_game;
}

void windy::content::project::remove(std::shared_ptr<class instance> instance) {
	this->_game->container()->remove(instance);
	this->_assets->collection()->remove(instance);
}

void windy::content::project::remove_sprite(std::shared_ptr<content::sprite> sprite) {
	this->_assets->remove_sprite(sprite);
}

void windy::content::project::remove_group(std::shared_ptr<content::group> group) {
	this->_assets->remove_group(group);
}
void windy::content::project::remove_composition(std::shared_ptr<content::composition> composition) {
	this->_assets->remove_composition(composition);
}
