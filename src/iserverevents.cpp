/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

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
