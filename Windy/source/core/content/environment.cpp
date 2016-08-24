#include "core/content/environment.hpp"

#include "core/content/edition.hpp"
#include "core/content/project.hpp"
#include "core/content/properties.hpp"
#include "core/animation/timeline.hpp"
#include "core/animation/frame.hpp"
#include "core/animation/frame_set.hpp"
#include "core/animation/trigger.hpp"


windy::content::environment::environment(){
	this->_edition = std::make_shared<content::edition>();
	this->_project = std::make_shared<content::project>();
	this->_properties = std::make_shared<content::properties>();
	this->_timeline = std::make_shared<animation::timeline>();
}

void windy::content::environment::trigger_recalculation() {
		
	if (this->_timeline->triggers().empty()) {
		this->_timeline->frame_set(nullptr);
	} else {

		auto& frames = this->_timeline->frame_set()->frames();

		for (unsigned int i = 0; i < frames.size(); ++i) {
					
			auto frame = frames[i];
			// amount of triggers on the current frame, if zero, swaps key frame
			uint64_t trigger_timeline_counter = 0;
			for (auto vector : this->_timeline->triggers()) {
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

		this->_timeline->recalculate();
		this->_timeline->recalculate_triggers();
	}

}


std::shared_ptr<class windy::content::edition> windy::content::environment::edition()
{
	return this->_edition;
}

std::shared_ptr<class windy::content::project> windy::content::environment::project()
{
	return this->_project;
}

std::shared_ptr<class windy::content::properties> windy::content::environment::properties()
{
	return this->_properties;
}

std::shared_ptr<class windy::animation::timeline> windy::content::environment::timeline()
{
	return this->_timeline;
}
