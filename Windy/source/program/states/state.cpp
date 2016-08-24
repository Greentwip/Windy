
#include "program/states/state.hpp"

#include "nana/gui/msgbox.hpp"

#include "core/2d/texture.hpp"

#include "core/animation/frame.hpp"
#include "core/animation/frame_set.hpp"
#include "core/animation/timeline.hpp"
#include "core/animation/trigger.hpp"

#include "core/content/assets.hpp"
#include "core/content/collection.hpp"
#include "core/content/compositable.hpp"
#include "core/content/composition.hpp"
#include "core/content/container.hpp"

#include "core/content/edition.hpp"
#include "core/content/game.hpp"

#include "core/content/group.hpp"
#include "core/content/groupable.hpp"

#include "core/content/instance_creator.hpp"

#include "core/content/item.hpp"

#include "core/content/project.hpp"
#include "core/content/properties.hpp"

#include "core/content/sprite.hpp"

#include "core/geometry/rectangle.hpp"

#include "core/number/uuid.hpp"
#include "core/number/digits.hpp"

#include "program/commands/movement_command.hpp"

#include "program/commands/drag_command.hpp"
#include "program/commands/anchor_command.hpp"

#include "core/content/environment.hpp"

void windy::state::bake(std::shared_ptr<class windy::content::environment> environment) {
	this->_environment = environment;
}

void windy::state::on_key_up(const nana::arg_keyboard& arg) {
	this->key_up_trigger(arg);
}


void windy::state::on_key_down(const nana::arg_keyboard& arg) {
	this->key_down_trigger(arg);
}

void windy::state::on_mouse_wheel(const nana::arg_wheel & arg)
{
	this->mouse_wheel_trigger(arg);
}

void windy::state::on_mouse_down(const nana::arg_mouse& arg) {
	this->mouse_down_trigger(arg);
}

void windy::state::on_mouse_move(const nana::arg_mouse & arg)
{
	this->mouse_move_trigger(arg);
}

void windy::state::on_mouse_up(const nana::arg_mouse & arg)
{
	this->mouse_up_trigger(arg);
}

void windy::state::make_selection(const nana::point& position) {

	std::vector<std::shared_ptr<content::instance> > targets_hit;
	std::shared_ptr<content::instance> top_target = nullptr;

	std::shared_ptr<content::instance> anchor_target = nullptr;

	if (this->_selection_targets.size() == 1) {		// front target
		anchor_target = this->_selection_targets.front();
	}

	unsigned int target_top_layer = 0;
	unsigned int target_top_z_order = 0;
	for (unsigned int i = 0;
	i < this->_environment->project()->game()->container()->children().size();
		++i) {
		//layers
		auto layer = this->_environment->project()->game()->container()->children()[i];

		for (unsigned int j = 0; j < layer->children().size(); ++j) {
			// children

			auto child = layer->children()[j];

			auto instance = std::dynamic_pointer_cast<content::instance>(child);

			auto entity_rectangle = instance->projection_rectangle();

			if (entity_rectangle.is_hit(position)) {

				targets_hit.push_back(instance);

				if (top_target == nullptr) {
					top_target = instance;
				}
				else {
					if (i >= target_top_layer && j >= target_top_z_order) {
						top_target = instance;

						target_top_layer = 0;
						target_top_z_order = j;
					}
				}
			}

		}
	}

	auto ctrl_down = this->_key_modifiers["ctrl"];

	for (auto target_hit : targets_hit) {
		auto target_hit_iterator = std::find(this->_selection_targets.begin(),
			this->_selection_targets.end(),
			target_hit);

		if (target_hit_iterator != this->_selection_targets.end()) { // already selected

			if (ctrl_down) { // ctrl pressed, multiple selection
							 // if top target, unselect
				if (target_hit == top_target) {
					anchor_target = nullptr;
					this->_selection_targets.erase(target_hit_iterator);
					this->_environment->properties()->clear();
					this->_environment->timeline()->render_triggers().clear();
				}
			}

		}
		else {													 // not selected

			if (ctrl_down) { // ctrl pressed, multiple selection
							 // if top target, select
				if (target_hit == top_target) {
					anchor_target = nullptr;
					this->_selection_targets.push_back(target_hit);
					this->_environment->properties()->clear();
					this->_environment->timeline()->render_triggers().clear();
				}
			}
			else { // no ctrl pressed, single selection only
				   // only if top target
				if (target_hit == top_target) {
					this->single_selection(target_hit);
				}
			}

		}

	}

	if (targets_hit.empty() && !ctrl_down) {

		this->_selection_targets.clear();
		this->_environment->properties()->clear();
		this->_environment->timeline()->render_triggers().clear();

	}

	bool is_anchor = false;

	if (anchor_target != nullptr) {


		auto anchor = anchor_target->projection_anchor();

		if (anchor.is_hit(position)) {
			this->single_selection(anchor_target);
			is_anchor = true;
		}

	}

	auto queued_command = std::find_if(this->_movement_commands.begin(),
									   this->_movement_commands.end(),
									   [](auto a) {
											if (a->name() == "entities") {
												return true;
											}
											else {
												return false;
											}
									   });

	if (queued_command == this->_movement_commands.end()) {
		if (is_anchor) {
			auto anchor_command = std::make_shared<class anchor_command>("entities");

			anchor_command->add_target(anchor_target);

			this->_movement_commands.push_back(anchor_command);

		} else {
			auto drag_command = std::make_shared<class drag_command>("entities");

			for (auto selection_target : this->_selection_targets) {
				drag_command->add_target(selection_target);
			}

			this->_movement_commands.push_back(drag_command);
		}
	}

}

void windy::state::single_selection(std::shared_ptr<content::instance> instance) {
	this->_selection_targets.clear();
	this->_selection_targets.push_back(instance);
	this->_environment->properties()->load(instance);

	switch (instance->kind()) {
		case layer::kind::groupable: {
			auto groupable =
				std::dynamic_pointer_cast<content::groupable>(instance);
			this->_environment->timeline()->render_triggers(groupable->triggers());
		}
		break;
	}

}

void windy::state::trigger_recalculation() {
		
	if (this->_environment->timeline()->triggers().empty()) {
		this->_environment->timeline()->frame_set(nullptr);
	} else {

		auto& frames = this->_environment->timeline()->frame_set()->frames();

		for (unsigned int i = 0; i < frames.size(); ++i) {
					
			auto frame = frames[i];
			// amount of triggers on the current frame, if zero, swaps key frame
			uint64_t trigger_timeline_counter = 0;
			for (auto vector : this->_environment->timeline()->triggers()) {
				for (auto trigger : vector) {
					if (trigger->frame() == i) {
						trigger_timeline_counter++;
					}
				}
			}

			if (trigger_timeline_counter == 0) { // zero triggers, swap key frame
				frame->is_key_frame(false);
			}

		}

		this->_environment->timeline()->recalculate();
		this->_environment->timeline()->recalculate_triggers();
	}

}


void windy::state::animation_trigger_map() {

	auto target = this->_selection_targets.front();
	auto groupable = std::dynamic_pointer_cast<content::groupable>(target);


	std::shared_ptr<animation::trigger> action_trigger;

	if (this->_environment->timeline()->frame_set() == nullptr) {
		this->_environment->timeline()->frame_set(std::make_shared<animation::frame_set>(1));
	}

	auto timeline_frame = this->_environment->timeline()->frame_set()->current_frame_index();

	for (auto trigger : groupable->triggers()) {
		if (trigger->frame() == timeline_frame) {
			action_trigger = trigger;
		}
	}

	auto current_timeline_frame = this->_environment->timeline()->frame_set()->current_frame();
	current_timeline_frame->is_key_frame(true);

	if (action_trigger == nullptr) {
				
		auto trigger_kind = animation::trigger::none;
			
		auto frame_set = groupable->frame_set();

		action_trigger = std::make_shared<animation::trigger>(trigger_kind,
																timeline_frame,
																frame_set);

		this->_environment->timeline()->add_trigger(action_trigger);
		groupable->triggers().push_back(action_trigger);

	} else {
		switch (action_trigger->get_kind()) {
			case animation::trigger::none:
				action_trigger->set_kind(animation::trigger::single);
			break;

			case animation::trigger::single:
				action_trigger->set_kind(animation::trigger::loop);
			break;

			case animation::trigger::loop: {
				this->_environment->timeline()->remove_trigger(action_trigger);

				auto& triggers = groupable->triggers();
				auto it = std::find(triggers.begin(), 
									triggers.end(), 
									action_trigger);

				groupable->triggers().erase(it);
			}
						
			break;

		}

	}

	trigger_recalculation();
}