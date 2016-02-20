#ifndef windy_dialog_open
#define windy_dialog_open

#include <functional>
#include <vector>
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

	void show() {
		std::string result_path;

		dialog_operation result = open_folder_dialog("", result_path);

		if (result == dialog_operation::dialog_success) {
			on_open_callback(result_path);
		}
		else if (result == dialog_operation::dialog_cancel) {
			on_cancel_callback();
		}
		else {
			on_error_callback("Error");
		}
	}

	void show(const std::vector<std::string>& filter_list) {
		std::string result_path;

		dialog_operation result = open_file_dialog(filter_list, "", result_path);

		if (result == dialog_operation::dialog_success) {
			on_open_callback(result_path);
		}
		else if (result == dialog_operation::dialog_cancel) {
			on_cancel_callback();
		}
		else {
			on_error_callback("Error");
		}
	}

	std::function<void(const std::string&)> on_open_callback;
	std::function<void()> on_cancel_callback;
	std::function<void(const std::string&)> on_error_callback;
};

#endif