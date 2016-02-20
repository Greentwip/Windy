/*
  Native File Dialog

  http://www.frogtoss.com/labs
 */

/* only locally define UNICODE in this compilation unit */
#ifndef UNICODE
#define UNICODE
#endif

#include <wchar.h>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <atlbase.h>
#include <windows.h>
#include <ShObjIdl.h>

#include "internal/dialogs_common.hpp"

class dialog {
public:
	dialog(CLSID dialog_clsid, IID dialog_iid) {
		::CoInitializeEx(NULL, ::COINIT_APARTMENTTHREADED | ::COINIT_DISABLE_OLE1DDE );

		::CoCreateInstance(dialog_clsid, NULL, CLSCTX_ALL, dialog_iid, 
			reinterpret_cast<void**>(&file_dialog));
	}

	~dialog() {
		::CoUninitialize();
	}


	dialog_operation show(const std::vector<std::string>& filterList,
 						  const std::string& defaultPath,
						  std::string& outPath, bool is_folder = false) {

		dialog_operation operation_result;

		add_filters(filterList);

		if (is_folder) {
			file_dialog->SetOptions(FOS_PICKFOLDERS);
		} 

		long result = file_dialog->Show(NULL);

		if (SUCCEEDED(result)) {
			::IShellItem *shellItem(NULL);

			file_dialog->GetResult(&shellItem);

			wchar_t *filePath(NULL);
			shellItem->GetDisplayName(::SIGDN_FILESYSPATH, &filePath);

			std::wstring buffer(filePath);
			outPath = std::string(buffer.begin(), buffer.end());

			CoTaskMemFree(filePath);

			operation_result = dialog_operation::dialog_success;
			shellItem->Release();

		} else if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
			operation_result = dialog_operation::dialog_cancel;
		} else {
			operation_result = dialog_operation::dialog_error;
		}

		return operation_result;

	}

	dialog_operation add_filters(const std::vector<std::string>& filterList) {
		
		if (filterList.empty())
			return dialog_operation::dialog_success;

		std::vector<COMDLG_FILTERSPEC> filters; 
		std::vector<std::wstring> wstrings_name;
		std::vector<std::wstring> wstrings_spec;

		for (auto filter : filterList) {

			wstrings_name.push_back(std::wstring(filter.begin(), filter.end()));
			wstrings_spec.push_back(L"*." + std::wstring(filter.begin(), filter.end()));


			COMDLG_FILTERSPEC filter_spec;

			filter_spec.pszName = wstrings_name.back().c_str();
			filter_spec.pszSpec = wstrings_spec.back().c_str();

			filters.push_back(filter_spec);
		}


		file_dialog->SetFileTypes(filters.size(), filters.data());

		return dialog_operation::dialog_success;
	}

private:
	::IFileDialog* file_dialog;
};

dialog_operation open_file_dialog(const std::vector<std::string>& filterList,
 								  const std::string& defaultPath,
								  std::string& outPath)
{
	dialog dlg(::CLSID_FileOpenDialog, ::IID_IFileOpenDialog);
	return dlg.show(filterList, defaultPath, outPath);
}

dialog_operation open_folder_dialog(const std::string& defaultPath,
									std::string& outPath)
{
	dialog dlg(::CLSID_FileOpenDialog, ::IID_IFileOpenDialog);
	return dlg.show({}, defaultPath, outPath, true);
}


dialog_operation save_file_dialog(const std::vector<std::string>& filterList,
								  const std::string& defaultPath,
								  std::string& outPath)
{
	dialog dlg(::CLSID_FileSaveDialog, ::IID_IFileSaveDialog);
	return dlg.show(filterList, defaultPath, outPath);
}