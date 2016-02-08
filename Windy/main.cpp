#include <map>
#include <memory>
#include <string>
#include <functional>

#include <ctype.h>
#include <clocale>
#include <cctype>


#include "nana/gui/wvl.hpp"
#include "nana/gui/msgbox.hpp"

#include "nana/gui/widgets/button.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/textbox.hpp"

#include "tools/number/uuid.hpp"
#include "tools/number/digits.hpp"

#include "tools/image/resizer.hpp"
#include "tools/tmx/tmx_converter.hpp"

#include "save/save.hpp"
#include "open/open.hpp"

namespace windy {

	class functional_form {
	public:
		enum input_constraints {
			text = 0,
			number, 
			pot_number
		};

		functional_form(const std::string& caption,
			const unsigned int& width = 300,
			const unsigned int& height = 200): cmd_count(0){
			auto window_rect = nana::API::make_center(width, height);
			_window = std::make_shared<nana::form>(window_rect);
			_window->caption(caption);
		}

		void add(const std::string& label_caption,
			const std::string& button_caption,
			const std::string& text_caption,
			const std::function<void(const std::string)>& function,
			input_constraints constraints = input_constraints::text) {

			unsigned int button_w = 120;
			unsigned int button_h = 60;

			unsigned int fm_cx = _window->size().width / 2;
			unsigned int fm_cy = _window->size().height / 2;

			unsigned int button_cx = button_w / 2;


			// labels, input boxes and command button

			nana::rectangle label_rect(0, 
									   0, 
									   button_w, 
									   unsigned int(button_h * 0.5));

			nana::rectangle textbox_rect(0, 
										 0, 
										 unsigned int(button_w * 0.80), 
										 unsigned int(button_h * 0.30));

			nana::rectangle btn_rect(0, 0, button_w, button_h);

			label_rect.x = int(fm_cx * 0.5 - button_cx);
			label_rect.y = button_h * cmd_count; 

			textbox_rect.x = int(fm_cx * 0.5 - button_cx);
			textbox_rect.y = int(button_h * cmd_count + button_h * 0.5);

			btn_rect.x = int(fm_cx * 1.5 - button_cx);
			btn_rect.y = button_h * cmd_count;

			auto label = std::make_shared<nana::label>(*_window, label_rect);
			
			auto textbox = std::make_shared<nana::textbox>(*_window, textbox_rect);

			auto button = std::make_shared<nana::button>(*_window, btn_rect);

			label->caption(label_caption);
			textbox->caption(text_caption);
			button->caption(button_caption);

			auto constraints_wrapper = [this, constraints, textbox, function]() {
				auto nana_str = textbox->caption();
				std::string caption(nana_str.begin(),
					nana_str.end());

				bool validated = false;

				if (constraints == input_constraints::number) {

					
					if (validate_number(caption)) {
						validated = true;
					}

				} else if(constraints == input_constraints::pot_number) {

					if (validate_pot_number(caption)) {
						validated = true;
					} 

				} else {
					validated = true;
				}

				if (validated) {
					function(caption);
				}
			};

			button->events().click(constraints_wrapper);

			labels.push_back(label);
			textboxes.push_back(textbox);
			buttons.push_back(button);

			++cmd_count;
		}

		int run() {
			_window->show();
			nana::exec();
			return 0;
		}

	private:
		void display_message_box(const std::string& caption) {
			nana::msgbox m(*this->_window, L"ERROR");
			m << std::wstring(caption.begin(), caption.end());
			m();
		}

		bool validate_number(const std::string& text) {
			if (windy::digits::is_number(text)) {
				return true;
			} else {
				display_message_box("Please introduce an integer number.");
				return false;
			}
		}

		bool validate_pot_number(const std::string& text) {

			bool validation = true;

			if (validate_number(text)) {

				if (!windy::digits::is_power_of_two(std::atoi(text.c_str()))) {
					validation = false;
					display_message_box("Please introduce a power of two number.");
				} 

			} else {
				validation = false;
			}

			return validation;
		}


	public: 
		auto get_window() {
			return this->_window;
		}

	private:

		unsigned int cmd_count;

		std::shared_ptr<nana::form> _window;

		std::vector<std::shared_ptr<nana::label> > labels;
		std::vector<std::shared_ptr<nana::textbox> > textboxes;

		std::vector<std::shared_ptr<nana::button> > buttons;


	};

}

int __stdcall WinMain(void*, void*, char* cmdLine, int)
{

	windy::functional_form form("Windy");

#if defined(NANA_WINDOWS)
	nana::API::window_icon(*form.get_window(), nana::paint::image(STR("res/windy.ico")));
#else
	nana::API::window_icon(*form.get_window(), nana::paint::image(STR("res/windy.bmp")));
#endif

	form.add("Design width",
			 "Resize",
			 "256",
			 [&form](const std::string input) {

				windy::dialog::open open_dialog;
				windy::dialog::save save_dialog;

				std::string input_file  = "";
				std::string output_file = "";

				open_dialog.on_open_callback = [&input_file](const std::string& path) {
					input_file = path;
				};

				save_dialog.on_save_callback = [&output_file](const std::string& path) {
					output_file = path;

					if (output_file.size() < 4) {
						output_file += ".png";
					}

					std::string extension(output_file.end() - 4, output_file.end());

					for (unsigned int i = 0; i < extension.length(); ++i) {
						extension[i] = std::tolower(extension[i]);
					}

					if (extension.compare(".png") != 0) {
						output_file = output_file + ".png";
					}

				};

				open_dialog.show("png");
				save_dialog.show("png");

				if (input_file.size() > 0 && output_file.size() > 0) {
					windy::resizer resizer;
					resizer.run({ input_file,
									output_file,
									input,
									"1920" });

					nana::msgbox m(*form.get_window(), L"Information");
					m << std::wstring(L"Complete!");
					m();

				}

			 },
			 windy::functional_form::input_constraints::number);

	form.add("Tile size",
			 "TMX",
			 "32",
			 [&form](const std::string input) {
				
				windy::dialog::open open_dialog;
				windy::dialog::save save_dialog;

				std::string input_file;
				std::string output_file;

				open_dialog.on_open_callback = [&input_file](const std::string& path) {
					input_file = path;
				};

				save_dialog.on_save_callback = [&output_file](const std::string& path) {
					output_file = path;

					if (output_file.size() < 4) {
						output_file += ".tmx";
					} 

					std::string extension(output_file.end() - 4, output_file.end());
					
					for (unsigned int i = 0; i < extension.length(); ++i) {
						extension[i] = std::tolower(extension[i]);
					}

					if (extension.compare(".tmx") != 0) {
						output_file = output_file + ".tmx";
					}
				};

				open_dialog.show("png");
				save_dialog.show("tmx");

				if (input_file.size() > 0 && output_file.size() > 0) {
					windy::tmx_converter tmx_converter;

					tmx_converter.run({input_file, output_file, input, "1024"});

					nana::msgbox m(*form.get_window(), L"Information");
					m << std::wstring(L"Complete!");
					m();

				}
			 },
			 windy::functional_form::input_constraints::pot_number);

	form.run();



	// cleanup?

}