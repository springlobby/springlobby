#include "iserverevents.h"
#include "serverevents.h"

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/networking/iserver.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


IServerEvents* IServerEvents::getInstance( Server& server, ServerEventsMode mode  )
{
	switch (mode) {
		default: return new ServerEvents( server );
	}
	return NULL;
}
