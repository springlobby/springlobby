#include "iserverevents.h"
#include "serverevents.h"
#include "simpleinterface/simpleserverevents.h"

IServerEvents* IServerEvents::getInstance( Server& server, ServerEventsMode mode  )
{
	switch (mode) {
		default: return new ServerEvents( server );
		case IServerEvents::simple: return new SimpleServerEvents( server );
	}
}
