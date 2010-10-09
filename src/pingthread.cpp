#include "pingthread.h"

#include <wx/thread.h>


PingThread::PingThread( iNetClass& parent, unsigned int interval ):
m_parent(parent),
m_interval(interval)
{
}

void PingThread::Init()
{
	Create();
	SetPriority( WXTHREAD_MAX_PRIORITY );
	Run();
}


void* PingThread::Entry()
{
	while ( !TestDestroy() )
	{
		m_parent.Ping();
		// break if woken
		if(!Sleep(m_interval))break;
	}
	return 0;
}

void PingThread::OnExit()
{
}
