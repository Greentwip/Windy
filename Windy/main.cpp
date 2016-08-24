#include "app.hpp"

#include "nana/gui/wvl.hpp"

int __stdcall WinMain(void*, void*, char* cmdLine, int)
{

	windy::app app;
	app.show();
	nana::exec();

	return 0;
}