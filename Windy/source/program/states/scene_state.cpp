#include <algorithm>
#include <memory>

#include "program/states/scene_state.hpp"
#include "core/number/digits.hpp"

#include "core/2d/texture.hpp"

#include "core/content/assets.hpp"
#include "core/content/collection.hpp"


#include "core/content/environment.hpp"
#include "core/content/edition.hpp"
#include "core/content/project.hpp"
#include "core/content/properties.hpp"
#include "core/animation/timeline.hpp"

#include "core/content/game.hpp"
#include "core/content/container.hpp"
#include "core/content/groupable.hpp"

#include "core/geometry/rectangle.hpp"

#include "program/commands/drag_command.hpp"


windy::scene_state::scene_state() {
	this->_key_modifiers["ctrl"] = false;
}

void windy::scene_state::key_down_trigger(const nana::arg_keyboard& arg) {

	this->_key_modifiers["ctrl"] = arg.ctrl;

	if(arg.key == nana::keyboard::os_del){

		for (auto instance : this->_selection_targets) {
			this->_environment->project()->game()->container()->remove(instance);
			this->_environment->project()->assets()->collection()->remove(instance);

			switch (instance->kind()) {
					case layer::groupable: {
						auto groupable =
						std::dynamic_pointer_cast<content::groupable> (instance);

						for (auto trigger : groupable->triggers()) {
							this->_environment->timeline()->remove_trigger(trigger);
						}

					}
					break;
			}
		}

		this->_selection_targets.clear();

		this->_environment->edition()->refresh(this->_environment->project()->game()->container());
		this->_environment->properties()->clear();

		this->_environment->trigger_recalculation();
	}
}

void windy::scene_state::key_up_trigger(const nana::arg_keyboard& arg) {
	this->_key_modifiers["ctrl"] = arg.ctrl;
}


void windy::scene_state::mouse_down_trigger(const nana::arg_mouse& arg) {

	if (arg.button == nana::mouse::right_button) {

		if (this->_selection_targets.size() == 1) { // single selection only

			switch (this->_selection_targets.front()->kind()) {
			case layer::kind::groupable: {
				auto entity_rectangle =
					this->_selection_targets.front()->projection_rectangle();

				if (entity_rectangle.is_hit(arg.pos)) {
					animation_trigger_map();

					auto groupable =
						std::dynamic_pointer_cast<content::groupable>
						(this->_selection_targets.front());

					this->_environment->timeline()->render_triggers(groupable->triggers());
				}
			}
			break;
			}

		}

	} else if (arg.button == nana::mouse::middle_button) {

		auto queued_command = std::find_if(this->_movement_commands.begin(),
										   this->_movement_commands.end(),
										   [](auto a) {
												if (a->name() == "canvas") {
													return true;
												}
												else {
													return false;}
											});

		if (queued_command == this->_movement_commands.end()) {
			auto drag_command = std::make_shared<class drag_command>("canvas");
			drag_command->add_target(this->_environment->project()->game()->container());
			this->_movement_commands.push_back(drag_command);
		}

	}
	else if (arg.button == nana::mouse::left_button) {
		this->make_selection(arg.pos);
	}
}

void windy::scene_state::mouse_up_trigger(const nana::arg_mouse& arg) {
	std::string command_string = "none";

	switch (arg.button) {
		case nana::mouse::middle_button: {
			command_string = "canvas";
		}
		break;

		case nana::mouse::left_button:
			/*case nana::mouse::right_button: */ {
				command_string = "entities";
		}
		break;
	}

	auto closure_string = std::make_shared<std::string>("entities");
	auto predicate = [closure_string](auto a) {
		if (a->name() == *closure_string) {
			return true;
		}
		else {
			return false;
		}
	};


	if (command_string != "none") {
		auto queued_command = std::find_if(this->_movement_commands.begin(),
										   this->_movement_commands.end(),
										   [command_string](auto a) { 
												if (a->name() == command_string) {
													return true;
												}else {
														return false;
												}
											});

		if (queued_command != this->_movement_commands.end()) {
			this->_movement_commands.erase(queued_command);
		}
	}

}

void windy::scene_state::mouse_move_trigger(const nana::arg_mouse& arg) {
	this->_mouse_position = arg.pos;
}

void windy::scene_state::mouse_wheel_trigger(const nana::arg_wheel& arg) {
	// set new scale values
	auto zoom_scale = 0.0f;

	if (arg.upwards) {
		zoom_scale += 0.0625f;
	} else {
		zoom_scale -= 0.0625f;
	}

	auto scale = this->_environment->project()->game()->container()->scale();
	zoom_scale = windy::digits::clamp(scale + zoom_scale,
									  0.125f,
									  4.0f);

	// scale canvas
	this->_environment->project()->game()->container()->zoom(this->_mouse_position, zoom_scale);
}

void windy::scene_state::on_draw_update(nana::paint::graphics& graphics) {
	// prepare the render batch
	this->_render_batch.clear();

	// draw the canvas with the scene size at its position
	auto canvas_rectangle = this->_environment->project()->game()->container()->projection_rectangle();

	graphics.rectangle(canvas_rectangle, true, nana::colors::black);

	for (auto layer : this->_environment->project()->game()->container()->children()) {
		for (auto child : layer->children()) {

			auto instance = std::dynamic_pointer_cast<content::instance>(child);
			auto entity_rectangle = instance->projection_rectangle();

			nana::rectangle window_rectangle(0,
											 0,
											 graphics.size().width,
											 graphics.size().height);

			auto intersection = rectangle::intersects(window_rectangle, entity_rectangle);

			if (intersection.first &&
				intersection.second->width != 0 &&
				intersection.second->height != 0) {

				auto clipping_region =
					nana::rectangle(intersection.second->x,
						intersection.second->y,
						intersection.second->width,
						intersection.second->height);

				// subtract the entity offset from the clipping region
				clipping_region.x -= entity_rectangle.x;
				clipping_region.y -= entity_rectangle.y;

				// normalize the clipping region
				auto normalized_clipping_region = instance->normalize_rectangle(clipping_region);

				if (normalized_clipping_region.width != 0 &&
					normalized_clipping_region.height != 0) {

					auto clipped_region
						= instance->texture()->clip(normalized_clipping_region,
							true,
							instance->scale());

					auto render_location = nana::point(intersection.second->x, intersection.second->y);
					this->_render_batch.push_back(std::make_pair(render_location, clipped_region));
				}

			}

		}
	}

	for (auto render_region : this->_render_batch) {
		//graphics.bitblt(render_region.first, *render_region.second);
		//@TODO fix black background instead of alpha values within render target
		graphics.bitblt(render_region.first.x, render_region.first.y, *render_region.second);
	}

	nana::rectangle selection_rectangle;

	//@TODO selection rectangle shall be within the drawing canvas if bigger (memory)
	for (auto selection_target : this->_selection_targets) {

		auto target_rectangle = selection_target->projection_rectangle();

		if (selection_rectangle.empty()) {
			selection_rectangle = target_rectangle;
		}
		else {
			selection_rectangle = rectangle::merge(selection_rectangle, target_rectangle);
		}

	}

	if (!selection_rectangle.empty()) {

		graphics.rectangle(selection_rectangle, false, nana::colors::alice_blue);

		if (this->_selection_targets.size() == 1) {

			auto instance = this->_selection_targets.front();

			auto anchor_rectangle = instance->projection_anchor();

			nana::rectangle window_rectangle(0,
				0,
				graphics.size().width,
				graphics.size().height);

			auto intersection = rectangle::intersects(window_rectangle, anchor_rectangle);

			if (intersection.first &&
				intersection.second->width != 0 &&
				intersection.second->height != 0) {

				auto clipping_region =
					nana::rectangle(intersection.second->x,
						intersection.second->y,
						intersection.second->width,
						intersection.second->height);


				graphics.round_rectangle(clipping_region,
					2,
					2,
					nana::colors::white,
					true,
					nana::colors::white);

				graphics.round_rectangle(clipping_region,
					2,
					2,
					nana::colors::black,
					false,
					nana::colors::black);

			}

		}

	}
}

void windy::scene_state::on_mouse_command_update() {
	if (this->_movement_commands.size() > 0) {
		this->_movement_commands.back()->update(this->_mouse_position);

		if (this->_selection_targets.size() == 1) { // single selection only
		this->_environment->properties()->load(this->_selection_targets.front());
		}

	}
}