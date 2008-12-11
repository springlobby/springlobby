#include "thread.h"
#include <wx/log.h>

Thread::Thread():
  wxThread(wxTHREAD_JOINABLE),
  m_thread_sleep_semaphore(0,0),
  m_must_exit(false)
{
}

Thread::~Thread(){
  Wait();
}

bool Thread::Sleep(int milliseconds){
  wxSemaError err=m_thread_sleep_semaphore.WaitTimeout(milliseconds);
  return err==wxSEMA_TIMEOUT;
}

void Thread::WakeUp(){
  m_thread_sleep_semaphore.Post();
}

Thread::ExitCode Thread::Wait(){
  m_must_exit=true;
  WakeUp();
  return wxThread::Wait();
}

wxThreadError Thread::Run(){
  m_must_exit=false;
  return wxThread::Run();
}

bool Thread::TestDestroy(){
  return wxThread::TestDestroy()||m_must_exit;
}


void WorkItemQueue::Push(WorkItem* item)
{
  if (item == NULL) return;
  wxCriticalSectionLocker lock(m_lock);
  queue.push(item);
}

WorkItem* WorkItemQueue::Pop()
{
  wxCriticalSectionLocker lock(m_lock);
  if (queue.empty()) return NULL;
  WorkItem* item = queue.top();
  queue.pop();
  return item;
}


void WorkerThread::DoWork(WorkItem* item)
{
  wxLogMessage( _T("scheduling WorkItem %p"), item );
  workItems.Push(item);
  WakeUp();
}

void* WorkerThread::Entry()
{
  WorkItem* item;

  wxLogMessage( _T("WorkerThread started") );

  while (!TestDestroy()) {
    // sleep an hour or until a new WorkItem arrives (DoWork() will wake us up).
    Sleep(3600 * 1000);
    while (!TestDestroy() && (item = workItems.Pop()) != NULL) {
      if (item->IsCancelled()) {
        wxLogMessage( _T("cancelling WorkItem %p, prio = %d"), item, item->priority );
        CleanupWorkItem(item);
        // no sleep, discard cancelled items at max rate
        continue;
      }
      try {
        wxLogMessage( _T("running WorkItem %p, prio = %d"), item, item->priority );
        item->Run();
      }
      catch (...) {
        // better eat all exceptions thrown by WorkItem::Run(),
        // don't want to let the thread die on a single faulty WorkItem.
        wxLogMessage( _T("WorkerThread caught exception thrown by WorkItem::Run") );
      }
      CleanupWorkItem(item);
      // sleep a moment to give other more important threads some air.
      Sleep(50);
    }
  }

  // cleanup leftover WorkItems
  while ((item = workItems.Pop()) != NULL) {
    CleanupWorkItem(item);
  }

  wxLogMessage( _T("WorkerThread stopped") );
  return 0;
}

void WorkerThread::CleanupWorkItem(WorkItem* item)
{
  if (item->toBeDeleted) {
    try {
      delete item;
    }
    catch (...) {
      // idem, eat exceptions from destructor
      wxLogMessage( _T("WorkerThread caught exception thrown by WorkItem::~WorkItem") );
    }
  }
}
