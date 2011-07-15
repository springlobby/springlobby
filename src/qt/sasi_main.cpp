#include "sasi_app.h"
#include <globalsmanager.h>

int main(int argc, char *argv[])
{
	SasiApp app(argc, argv);
	if ( !app.CmdInit() )
		return 1;
	const int ret = app.exec();
	DestroyGlobals();
	return ret;
}

