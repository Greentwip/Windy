#include <map>
#include <memory>
#include <string>
#include <functional>

#include <ctype.h>
#include <clocale>
#include <cctype>

#include "nana/gui/wvl.hpp"

#include "tools/gui/functional_form.hpp"

#include "tools/image/cropper.hpp"
#include "tools/image/merger.hpp"
#include "tools/image/resizer.hpp"
#include "tools/tmx/tmx_converter.hpp"

#include "save/save.hpp"
#include "open/open.hpp"

class app {
public:

	app() :_form("Windy", 300, 300) {
#if defined(NANA_WINDOWS)
		nana::API::window_icon(*_form.get_window(), nana::paint::image(STR("res/windy.ico")));
#else
		nana::API::window_icon(*_form.get_window(), nana::paint::image(STR("res/windy.bmp")));
#endif

		_form.add("Number of pieces", 
				  "Split", 
				  "16", 
				  std::bind(&app::on_split, this, std::placeholders::_1),
				  windy::functional_form::input_constraints::number);

		_form.add("Open wxt",
				  "Merge",
				  "",
				  std::bind(&app::on_merge, this, std::placeholders::_1),
				  windy::functional_form::input_constraints::no_input);

		_form.add("Design width",
				  "Resize",
				  "256", 
				  std::bind(&app::on_resize, this, std::placeholders::_1),
				  windy::functional_form::input_constraints::number);

		_form.add("Tile size",
				  "TMX",
				  "128", 
				  std::bind(&app::on_tmx, this, std::placeholders::_1),
				  windy::functional_form::input_constraints::pot_number);
	}

	void on_split(const std::string& input) {
		windy::dialog::open open_dialog;
		windy::dialog::open open_folder_dialog;

		std::string input_file = "";
		std::string output_file = "";

		open_dialog.on_open_callback = [&input_file](const std::string& path) {
			input_file = path;
		};

		open_folder_dialog.on_open_callback = [&output_file](const std::string& path) {
			output_file = path;
		};

		open_dialog.show({ "png" });
		open_folder_dialog.show();

		if (input_file.size() > 0 && output_file.size() > 0) {
			windy::cropper cropper;
			cropper.run({ input_file,
				output_file,
				input });

			nana::msgbox m(*_form.get_window(), L"Information");
			m << std::wstring(L"Complete!");
			m();

		}
	}
	void on_merge(const std::string& input) {
		windy::dialog::open open_dialog;
		windy::dialog::save save_dialog;

		std::string input_file = "";
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

		open_dialog.show({ "wxt" });
		save_dialog.show({ "png" });

		if (input_file.size() > 0 && output_file.size() > 0) {
			windy::merger merger;
			merger.run({ input_file, output_file });

			nana::msgbox m(*_form.get_window(), L"Information");
			m << std::wstring(L"Complete!");
			m();

		}
	}

	void on_resize(const std::string& input) {
		windy::dialog::open open_dialog;
		windy::dialog::save save_dialog;

		std::string input_file = "";
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

		open_dialog.show({ "png" });
		save_dialog.show({ "png" });

		if (input_file.size() > 0 && output_file.size() > 0) {
			windy::resizer resizer;
			resizer.run({ input_file,
				output_file,
				input,
				"1920" });

			nana::msgbox m(*_form.get_window(), L"Information");
			m << std::wstring(L"Complete!");
			m();

		}
	}
	void on_tmx(const std::string& input) {
		windy::dialog::open open_dialog;
		windy::dialog::open folder_dialog;

		std::string input_file;
		std::string output_file;

		open_dialog.on_open_callback = [&input_file](const std::string& path) {
			input_file = path;
		};

		folder_dialog.on_open_callback = [&output_file](const std::string& path) {
			output_file = path;
		};

		open_dialog.show({ "wxt", "png" });
		folder_dialog.show();

		if (input_file.size() > 0 && output_file.size() > 0) {
			windy::tmx_converter tmx_converter;

			tmx_converter.run({ input_file, output_file, input, "2048" });

			nana::msgbox m(*_form.get_window(), L"Information");
			m << std::wstring(L"Complete!");
			m();

		}
	}
	
	unsigned int run() {
		return _form.run();
	}
private:
	windy::functional_form _form;
};

int __stdcall WinMain(void*, void*, char* cmdLine, int)
{
	return app().run();
}