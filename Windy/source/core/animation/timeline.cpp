#include <sstream>
#include <functional>
#include <memory>
#include <algorithm>

#include "core/animation/timeline.hpp"

#include "core/animation/trigger.hpp"

#include "core/animation/frame_set.hpp"

#include "core/animation/frame.hpp"

#include "core/number/digits.hpp"

windy::animation::timeline::timeline() : 
	_playing(false), _timeline_size_in_frames(60), _bar_value(0){
	this->_counter = 0;
}

void windy::animation::timeline::init(){

	this->_place.bind(*this);

	auto div = std::stringstream();

	div << ""	
			<< "<weight=12% horizontal"	
				<< "<buttons>"
			<< ">"
			<< "<vertical"
				<< "<weight=80% canvas>"	
				<< "<scroll>"
			<< ">"
		<< "";
					
	std::vector<std::wstring> command_labels{ L"|<-", L"<-", L">", L"->", L"->|" };
	std::vector < std::function<void(const nana::arg_mouse&)> > commands{
		[this](const nana::arg_mouse& arg) {
			this->_bar_value = 0;	
			this->_frame_set->current_frame_index(this->_bar_value);

			this->stop();
			this->recalculate_triggers();

			this->_scroll.value(0);
		}, 
		[this](const nana::arg_mouse& arg) {
			if (this->_bar_value > 0) {
				this->_bar_value--;

				if (this->_scroll.value() > 0) {
					if (this->_bar_value < this->_scroll.value()) {
						this->_scroll.value(this->_scroll.value() - 1);
					}
				}

			} 

			this->_frame_set->current_frame_index(this->_bar_value);

			this->stop();
			this->recalculate_triggers();
		}, 
		[this](const nana::arg_mouse& arg) {
			if (this->on_animation_trigger()) {
				this->play();
			} else {
				this->stop();
			}
		},
		[this](const nana::arg_mouse& arg) {
			if (this->_bar_value < this->_frame_set->frames().size() - 1) {
				this->_bar_value++;

				if (this->_bar_value - this->_scroll.value() == 
					this->_timeline_size_in_frames) {
					this->_scroll.value(this->_scroll.value() + 1);
				}
			}
			this->_frame_set->current_frame_index(this->_bar_value);

			this->stop();
			this->recalculate_triggers();
		}, 
		[this](const nana::arg_mouse& arg) {
			this->_bar_value = this->_frame_set->frames().size() - 1;
			this->_frame_set->current_frame_index(this->_bar_value);

			this->stop();
			this->recalculate_triggers();

			this->_scroll.value(this->_scroll.amount());
		}

	};

	for (auto i = 0; i < command_labels.size(); ++i) {
		this->_buttons.emplace_back(std::make_shared<nana::button>(*this));
		this->_buttons.back()->caption(command_labels[i]);
		this->_buttons.back()->events().mouse_down(commands[i]);
		this->_buttons.back()->enabled(false);
	}

	this->_canvas.create(*this);
	this->_scroll.create(*this);

	this->_place.div(div.str().c_str());

	for (auto button : this->_buttons) {
		this->_place["buttons"] << button->handle();
	}

	this->_place["canvas"] << this->_canvas.handle();
	this->_place["scroll"] << this->_scroll.handle();

	this->_place.collocate();

	this->_drawing = std::make_shared<nana::drawing>(this->_canvas.handle());
	this->_drawing->draw_diehard(std::bind(&timeline::on_draw_update,
											this,
											std::placeholders::_1));

	this->_canvas.events().mouse_down([this](const nana::arg_mouse& arg) {
		if (this->_frame_set != nullptr) {

			if (arg.button == nana::mouse::left_button) {

				this->stop();

				auto hit_point = arg.pos;

				auto round_width = std::floor(this->_canvas.size().width / this->_timeline_size_in_frames) *
												this->_timeline_size_in_frames;

				uint32_t drawing_border = uint32_t(this->_canvas.size().width - round_width);

				auto canvas_rect = nana::rectangle(drawing_border, 
													0, 
													this->_canvas.size().width - drawing_border, 
													this->_canvas.size().height);


				if (canvas_rect.is_hit(hit_point)) {
					hit_point.x -= canvas_rect.x;

					auto floor_hit_x = std::floor(long double(hit_point.x) /
													long double(canvas_rect.width) *
													long double(60.0));

					auto calculated_value = uint32_t(floor_hit_x + this->_scroll.value());

					if (calculated_value < this->_frame_set->frames().size()) {
						this->_bar_value = calculated_value;

						this->_frame_set->current_frame_index(this->_bar_value);

						this->recalculate_triggers();
					}

				}
			}
		}
	});

	this->_canvas.events().dbl_click([this](const nana::arg_mouse& arg) {
		if (arg.button == nana::mouse::left_button) {
			if (this->_frame_set != nullptr) {
				this->_frame_set->swap_key_frame(this->_bar_value);

				//@TODO remove triggers
			}
		}
	});

}

std::shared_ptr<nana::drawing> windy::animation::timeline::drawing() {
	return this->_drawing;
}

void windy::animation::timeline::drawing(std::shared_ptr<nana::drawing> drawing) {
	this->_drawing = drawing;
}

void windy::animation::timeline::frame_set(std::shared_ptr<windy::animation::frame_set> frame_set) {
	this->_frame_set = frame_set;

	auto functionality_enabled = this->_frame_set != nullptr;


	if (this->_frame_set != nullptr) {
		functionality_enabled = this->_frame_set->frames().size() != 0;
	}

	for (uint64_t i = 0; i < this->_buttons.size(); ++i) {
		auto button = this->_buttons[i];
		button->enabled(functionality_enabled);
	}

	if (functionality_enabled) {

		uint64_t scroll_value = 0;

		if (this->_timeline_size_in_frames >= this->_frame_set->frames().size()) {
			scroll_value = this->_timeline_size_in_frames - this->_frame_set->frames().size();
		} else {
			scroll_value = this->_frame_set->frames().size() - this->_timeline_size_in_frames;
		}

		this->_scroll.amount(scroll_value + 1);

	} else {
		this->_scroll.amount(1);
		this->_counter = 0;
		this->_bar_value = 0;

	}

}

std::shared_ptr<class windy::animation::frame_set> windy::animation::timeline::frame_set() {
	return this->_frame_set;
}

void windy::animation::timeline::on_draw_update(nana::paint::graphics& graphics) {

	if (this->_frame_set) {

		nana::rectangle graph_rect{ 0,
									0,
									graphics.size().width, 
									graphics.size().height };

		auto round_width = std::floor(graph_rect.width / this->_timeline_size_in_frames) * 
										this->_timeline_size_in_frames;

		uint32_t drawing_border = uint32_t(graph_rect.width - round_width);

		graph_rect.x += drawing_border;
		graph_rect.width = round_width;

		this->_timeline_cell_size = nana::size(round_width / this->_timeline_size_in_frames,
												graph_rect.height);

		nana::rectangle offset_rect(0,
									0,
									drawing_border,
									graph_rect.height);

		graphics.rectangle(offset_rect, true, nana::colors::dark_gray);

		std::size_t cell_count = std::min(this->_timeline_size_in_frames,
											this->_frame_set->frames().size());


		std::vector<nana::rectangle> cells;

		for (std::size_t i = 0; i < cell_count; ++i) {
			auto current_frame_rect =
				nana::rectangle(graph_rect.x + i * this->_timeline_cell_size.width,
					graph_rect.y,
					this->_timeline_cell_size.width,
					this->_timeline_cell_size.height);

			cells.push_back(current_frame_rect);
		}

		for (std::size_t i = 0; i < cell_count; ++i) {

			auto& current_frame_rect = cells[i];

			auto frame_value = i + 1 + this->_scroll.value();

			if (frame_value % 5 == 0 || frame_value == 1) {
				auto string_point = nana::point(current_frame_rect.x,
												current_frame_rect.y);

				graphics.string(string_point, std::to_wstring(frame_value));
			}

			current_frame_rect.y += graph_rect.height * 0.4;
			current_frame_rect.height -= current_frame_rect.y;

			graphics.rectangle(current_frame_rect, true, nana::colors::white);
			graphics.rectangle(current_frame_rect, false, nana::colors::black);

			if (this->_frame_set->frames()[i + this->_scroll.value()]->is_key_frame()) {

				current_frame_rect.x += current_frame_rect.width * 0.30;
				current_frame_rect.y += current_frame_rect.height * 0.75;

				current_frame_rect.width = current_frame_rect.width * 0.50;
				current_frame_rect.height = current_frame_rect.width;

				auto key_frame_color = nana::colors::black;

				if (this->_render_triggers.size() > 0) {
					auto it = std::find_if(this->_render_triggers.begin(), 
											this->_render_triggers.end(), 
											[&frame_value](auto trigger) {
											return trigger->frame() == frame_value - 1; // because of zero indexing
								});

					if (it != this->_render_triggers.end()) {
						switch ((*it)->get_kind()) {
							case trigger::kind::none: {
								key_frame_color = nana::colors::red;
							}
							break;
							case trigger::kind::single: {
								key_frame_color = nana::colors::yellow;
							}
							break;
							case trigger::kind::loop: {
								key_frame_color = nana::colors::green;
							}
							break;
						}
					}

				}

				graphics.round_rectangle(current_frame_rect,
											1,
											1,
											key_frame_color,
											true,
											key_frame_color);
								
			}
					
		}

		if (this->_bar_value >= this->_scroll.value() &&
			this->_bar_value < this->_timeline_size_in_frames + this->_scroll.value()) {

			// draw bar indicator
			auto bar_color = nana::color(nana::colors::blue);

			auto bar_rect = nana::rectangle(graph_rect.x,
											graph_rect.y,
											this->_timeline_cell_size.width,
											this->_timeline_cell_size.height);

			//@TODO fix warnings with numeric_limits
			bar_rect.x += (this->_bar_value - this->_scroll.value()) * bar_rect.width;
			bar_rect.height -= graph_rect.height * 0.5;

			graphics.rectangle(bar_rect, false, bar_color);

			auto bar_line_origin = 
				nana::point(bar_rect.x + bar_rect.width * 0.5, 
							bar_rect.y + bar_rect.height);
			auto bar_line_end = nana::point(bar_line_origin.x,
				bar_line_origin.y + this->_timeline_cell_size.height - bar_rect.height);

			graphics.line(bar_line_origin, bar_line_end, bar_color);
		}
	}

}

bool windy::animation::timeline::on_animation_trigger() {
	bool is_play = true;

	if (this->_buttons[2]->caption() == L">") {
		if (this->_bar_value == this->_frame_set->frames().size() - 1) {
			this->_bar_value = 0;
			this->_counter = 0;
		}
		this->_buttons[2]->caption(L"||");
	}
	else if (this->_buttons[2]->caption() == L"||") {
		is_play = false;
		this->_buttons[2]->caption(L">");
	}

	return is_play;
}

void windy::animation::timeline::play() {
	this->_playing = true;
}

void windy::animation::timeline::stop() {
	this->_playing = false;

	if (this->_buttons[2]->caption() == L"||") {
		this->_buttons[2]->caption(L">");
	}

}

void windy::animation::timeline::update(const float& delta) {
	if (this->_playing) {

		auto idx = 
			windy::digits::clamp(static_cast<uint64_t>
									(std::round(this->_counter * 60)), 
									static_cast<uint64_t>(0),
									static_cast<uint64_t>
									(this->_frame_set->frames().size()));

		if (idx == this->_frame_set->frames().size()) {

			stop();

			this->_counter = 0;

			for (auto& vector : this->_triggers) {
				for (auto trigger : vector) {
					trigger->playing(false);
				}
								
			}

		} else {
			this->_bar_value = idx;
			this->_frame_set->current_frame_index(this->_bar_value);

			// update the triggers
			for (auto& vector : this->_triggers) {
				for (auto trigger : vector) {
					if (trigger->frame() == this->_bar_value) {

						if (trigger->previous() != nullptr) {
							trigger->previous()->frame_set()->reset_counter();
							trigger->previous()->playing(false);
						}

						switch (trigger->get_kind()) {
							case trigger::kind::none:
								trigger->frame_set()->reset_counter();
								trigger->playing(false);
							break;

							case trigger::kind::loop:
							case trigger::kind::single:
								trigger->frame_set()->reset_counter();
								trigger->playing(true);
							break;
									
						}
					}

					if (trigger->playing()) {

						trigger->frame_set()->update(delta);

						if (trigger->get_kind() == trigger::kind::single) {
							if (trigger->frame_set()->current_frame_index() >=
								trigger->frame_set()->frames().size() - 1) {
								trigger->frame_set()->stop();
								trigger->playing(false);
							}
						}

								
					}

				}
			}

			this->_counter += delta;
		}
	}
}

void windy::animation::timeline::recalculate_triggers() {

	if (this->_frame_set->frames().empty()) {
		assert(false); // something really bad could happen
	} else {
		this->_bar_value = std::min(this->_bar_value, 
								    this->_frame_set->frames().size() - 1);

	}

	this->_counter = 1.0f / 60.0f * this->_bar_value;

	std::vector<std::shared_ptr<trigger> > previous_triggers;
	for (auto& vector : this->_triggers) { // triggers is already sorted

		// warning, triggers must be sorted/balanced
		std::shared_ptr<trigger> previous_trigger = nullptr;
		for (auto& trigger : vector) {
			if (trigger->frame() <= this->_bar_value) {
				previous_trigger = trigger;
			}
			trigger->frame_set()->reset_counter();
			trigger->playing(false);
		}

		if (previous_trigger != nullptr) {
			previous_triggers.push_back(previous_trigger);
		}
	}

	for (auto trigger : previous_triggers) {
		switch (trigger->get_kind()) {
			case trigger::kind::none:
				trigger->playing(false);
				trigger->frame_set()->reset_counter(0.0f);
			break;
			case trigger::kind::single:
			case trigger::kind::loop: {
				trigger->playing(true);

				float new_counter_value = this->_bar_value - trigger->frame();
				new_counter_value *= 1.0f / 60.0f;
				trigger->frame_set()->reset_counter(new_counter_value);

			}
			break;
		}
	}
}


void windy::animation::timeline::recalculate() {
	uint64_t timeline_max_size = 0;

	for (auto& vector : this->_triggers) {
		for (auto trigger : vector) {
			auto size =
				trigger->frame() + trigger->frame_set()->frames().size();

			// find the max value
			if (size >= timeline_max_size) {
				timeline_max_size = size;
			}
		}
	}

	if (timeline_max_size > this->_frame_set->frames().size()) {
		auto& frames = this->_frame_set->frames();

		auto frames_size = frames.size();

		for (uint64_t i = frames_size; i<timeline_max_size; ++i) {
			frames.push_back(std::make_shared<animation::frame>());
		}

	} else if(timeline_max_size < this->_frame_set->frames().size()) {
		auto& frames = this->_frame_set->frames();

		auto frames_size = frames.size();
		for (uint64_t i = timeline_max_size; i < frames_size; ++i) {
			frames.pop_back();
		}
	}

	this->frame_set(this->_frame_set);
}

auto windy::animation::timeline::trigger_vector_iterator(std::shared_ptr<trigger> trigger) {

	auto it = std::find_if(this->_triggers.begin(), this->_triggers.end(),
		[trigger](auto vector) {

		bool searching = true;
		bool found = false;

		for (unsigned int i = 0; i < vector.size() && searching; ++i) {
			if (vector[i]->frame_set() == trigger->frame_set()) {
				searching = false;
				found = true;
			}
		}

		return found;
	});

	return it;
}

void windy::animation::timeline::add_trigger(std::shared_ptr<trigger> trigger) {

	auto it = trigger_vector_iterator(trigger);

	if (it != this->_triggers.end()) {
		it->push_back(trigger);
	}
	else {
		this->_triggers.push_back({});
		this->_triggers.back().push_back(trigger);
	}

	balance_triggers();

}

void windy::animation::timeline::remove_trigger(std::shared_ptr<trigger> trigger) {

	auto it = trigger_vector_iterator(trigger);

	auto trigger_it = std::find(it->begin(), it->end(), trigger);

	it->erase(trigger_it);
					
	if (it->empty()) {
		this->_triggers.erase(it);
	}

	balance_triggers();

}

void windy::animation::timeline::balance_triggers() {

	for (auto& vector : this->_triggers) {
		std::sort(vector.begin(), vector.end(), 
			[](auto a, auto b) {

			bool left = true;

			if (a->frame() < b->frame()) {
				a->next(b);
				b->previous(a);
			} else {
				a->previous(b);
				b->next(a);

				left = false;
			}

			return left;
		});

		vector.front()->previous(nullptr);
		vector.back()->next(nullptr);
	}

}

std::vector<std::vector<std::shared_ptr<class windy::animation::trigger > > >& windy::animation::timeline::triggers() {
	return this->_triggers;
}

void windy::animation::timeline::render_triggers(const std::vector<std::shared_ptr<trigger> >& triggers) {
	this->_render_triggers = triggers;
}

std::vector<std::shared_ptr<class windy::animation::trigger> >& windy::animation::timeline::render_triggers() {
	return this->_render_triggers;
}
