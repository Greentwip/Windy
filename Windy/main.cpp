#include "core/forms/app_form.hpp"

int __stdcall WinMain(void*, void*, char* cmdLine, int)
{

	windy::app_form app;
	app.display();
	return 0;
}