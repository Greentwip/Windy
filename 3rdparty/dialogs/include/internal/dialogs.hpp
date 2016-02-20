#ifndef windy_dialogs
#define windy_dialogs


#include <stddef.h>
#include <string>
#include <vector>

enum dialog_operation {
    dialog_error = 0,   // programmatic error 
    dialog_success,     // user pressed okay, or successful return 
    dialog_cancel       // user pressed cancel 
};

// single file open dialog 
dialog_operation open_file_dialog(const std::vector<std::string>& filterList,
								  const std::string& defaultPath,
								  std::string& outPath);

// single folder open dialog 
dialog_operation open_folder_dialog(const std::string& defaultPath,
								    std::string& outPath);


// single file save dialog 
dialog_operation save_file_dialog(const std::vector<std::string>& filterList,
								  const std::string& defaultPath,
								  std::string& outPath );

#endif