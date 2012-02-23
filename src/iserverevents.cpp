#include "iserverevents.h"
#include "serverevents.h"
#include "qt/simpleserverevents.h"

IServerEvents* IServerEvents::getInstance( Server& server, ServerEventsMode mode  )
{
	switch (mode) {
		default: return new ServerEvents( server );
	#ifdef SL_QT_MODE
		case IServerEvents::simple: return new SimpleServerEvents( server );
	#endif //SL_QT_MODE
	}
}
