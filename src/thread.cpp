#include "thread.h"
#include <algorithm>
#include <wx/log.h>

Thread::Thread():
		wxThread( wxTHREAD_JOINABLE ),
		m_thread_sleep_semaphore( 0, 0 ),
		m_must_exit( false )
{
}

/** TODO this causes a segfault on exit for me (koshi) sometimes
http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadwait
says to only call wxThread::Wait from another thread context
**/
Thread::~Thread() {
	if ( IsAlive() )Wait();
}

bool Thread::Sleep( int milliseconds ) {
	wxSemaError err = m_thread_sleep_semaphore.WaitTimeout( milliseconds );
	return err == wxSEMA_TIMEOUT;
}

void Thread::WakeUp() {
	m_thread_sleep_semaphore.Post();
}

Thread::ExitCode Thread::Wait() {
	m_must_exit = true;
	WakeUp();
	return wxThread::Wait();
}

wxThreadError Thread::Run() {
	m_must_exit = false;
	return wxThread::Run();
}

bool Thread::TestDestroy() {
	return wxThread::TestDestroy() || m_must_exit;
}


namespace
{
	struct WorkItemCompare
	{
		bool operator()( const WorkItem* a, const WorkItem* b )
		{
			return a->GetPriority() < b->GetPriority();
		}
	};
};


bool WorkItem::Cancel()
{
	wxLogMessage( _T( "cancelling WorkItem %p" ), this );
	if ( m_queue == NULL ) return false;
	return m_queue->Remove( this );
}


void WorkItemQueue::Push( WorkItem* item )
{
	if ( item == NULL ) return;
	wxCriticalSectionLocker lock( m_lock );
	m_queue.push_back( item );
	std::push_heap( m_queue.begin(), m_queue.end(), WorkItemCompare() );
	item->m_queue = this;
}

WorkItem* WorkItemQueue::Pop()
{
	wxCriticalSectionLocker lock( m_lock );
	if ( m_queue.empty() ) return NULL;
	WorkItem* item = m_queue.front();
	std::pop_heap( m_queue.begin(), m_queue.end(), WorkItemCompare() );
	m_queue.pop_back();
	item->m_queue = NULL;
	return item;
}

bool WorkItemQueue::Remove( WorkItem* item )
{
	wxCriticalSectionLocker lock( m_lock );
	if ( m_queue.empty() ) return false;
	// WARNING: this destroys the heap...
	std::vector<WorkItem*>::iterator new_end =
	    std::remove( m_queue.begin(), m_queue.end(), item );
	// did a WorkerThread process the item just before we got here?
	if ( new_end == m_queue.end() ) return false;
	m_queue.erase( new_end, m_queue.end() );
	// recreate the heap...
	std::make_heap( m_queue.begin(), m_queue.end(), WorkItemCompare() );
	item->m_queue = NULL;
	return true;
}


void WorkerThread::DoWork( WorkItem* item, int priority, bool toBeDeleted )
{
	wxLogMessage( _T( "scheduling WorkItem %p, prio = %d" ), item, priority );
	item->m_priority = priority;
	item->m_toBeDeleted = toBeDeleted;
	m_workItems.Push( item );
	WakeUp();
}

void* WorkerThread::Entry()
{
	WorkItem* item;

	wxLogMessage( _T( "WorkerThread started" ) );

	while ( !TestDestroy() ) {
		// sleep an hour or until a new WorkItem arrives (DoWork() will wake us up).
		Sleep( 3600 * 1000 );
		while ( !TestDestroy() && ( item = m_workItems.Pop() ) != NULL ) {
			try {
				wxLogMessage( _T( "running WorkItem %p, prio = %d" ), item, item->m_priority );
				item->Run();
			}
			catch ( ... ) {
				// better eat all exceptions thrown by WorkItem::Run(),
				// don't want to let the thread die on a single faulty WorkItem.
				wxLogMessage( _T( "WorkerThread caught exception thrown by WorkItem::Run" ) );
			}
			CleanupWorkItem( item );
			// give other threads some air
			Yield();
		}
	}

	// cleanup leftover WorkItems
	while ( ( item = m_workItems.Pop() ) != NULL ) {
		CleanupWorkItem( item );
	}

	wxLogMessage( _T( "WorkerThread stopped" ) );
	return 0;
}

void WorkerThread::CleanupWorkItem( WorkItem* item )
{
	if ( item->m_toBeDeleted ) {
		try {
			delete item;
		}
		catch ( ... ) {
			// idem, eat exceptions from destructor
			wxLogMessage( _T( "WorkerThread caught exception thrown by WorkItem::~WorkItem" ) );
		}
	}
}
