#include "iserverevents.h"
#include "serverevents.h"

IServerEvents* IServerEvents::getInstance( Server& server, ServerEventsMode mode  )
{
	switch (mode) {
		default: return new ServerEvents( server );
	}
	return NULL;
}
