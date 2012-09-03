#include "sasi_app.h"
#include <lslutils/globalsmanager.h>

int main(int argc, char *argv[])
{
	SasiApp app(argc, argv);
	if ( !app.CmdInit() )
		return 1;
	const int ret = app.exec();
    LSL::Util::DestroyGlobals();
	return ret;
}

