#ifndef windy_dialog_open
#define windy_dialog_open

#include <functional>
#include <string>

#include "internal/dialogs_common.hpp"
#include "internal/dialogs.hpp"

namespace windy {
namespace dialog {
	class open;
}
}

class windy::dialog::open {
public:

	open() {
		on_open_callback = [](const std::string&) {};
		on_cancel_callback = []() {};
		on_error_callback = [](const std::string&) {};
	}

	void show(const std::string& filter_list) {
		std::string result_path;

		nfdresult_t result = NFD_OpenDialog(filter_list.c_str(), NULL, result_path);

		if (result == NFD_OKAY) {
			on_open_callback(result_path);
		}
		else if (result == NFD_CANCEL) {
			on_cancel_callback();
		}
		else {
			on_error_callback(NFD_GetError());
		}
	}

	std::function<void(const std::string&)> on_open_callback;
	std::function<void()> on_cancel_callback;
	std::function<void(const std::string&)> on_error_callback;
};

#endif