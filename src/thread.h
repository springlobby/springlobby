#ifndef THREAD_H
#define THREAD_H
#include <wx/thread.h>
#include <queue>

/// joinable thread, with overridden Sleep and Wait methods.
/// Sleep wakes up when you call Wait()
class Thread: public wxThread
{
  public:
    Thread();
    /// destructor Waits for thread to complete
    ~Thread();
    /// overrides wxThread::Wait . Wakes up from sleep if sleeping, to prevent delay.
    ExitCode Wait();
    /// returns false when woken up
    bool Sleep(int milliseconds);
    /// call this from outside thread to wake up Sleep - ing thread
    void WakeUp();

    /// overrides wxThread::TestDestroy , which can fail on Wait
    bool TestDestroy();
    wxThreadError Run();
  protected:
    wxSemaphore m_thread_sleep_semaphore;
    /// workaround for old wxWidgets bug
    bool m_must_exit;
};


/// Abstraction of a piece of work to be done by WorkerThread
/// Inherit this class to define concrete work items
class WorkItem
{
  public:
    WorkItem(int priority = 0, bool toBeDeleted = true)
      : priority(priority), toBeDeleted(toBeDeleted), cancel(false) {}
    virtual ~WorkItem() {}

    virtual void Run() = 0;

    void Cancel() { cancel = true; }
    bool IsCancelled() const { return cancel; }

    const int priority;     ///< Priority of item, highest is run first
    const bool toBeDeleted; ///< Should this item be deleted after it has run?

  private:
    volatile bool cancel;
};


/// Priority queue of work items
class WorkItemQueue
{
  public:
    void Push(WorkItem* item);
    WorkItem* Pop();

  private:
    struct WorkItemCompare
    {
      bool operator()(const WorkItem* a, const WorkItem* b) {
        return a->priority < b->priority;
      }
    };
    wxCriticalSection m_lock;
    std::priority_queue<WorkItem*, std::vector<WorkItem*>, WorkItemCompare> queue;
};


class WorkerThread : public Thread
{
  public:
    /// adds a new WorkItem to the queue
    void DoWork(WorkItem* item);
    /// overrides wxThread::Entry
    void* Entry();

  private:
    void CleanupWorkItem(WorkItem* item);

    WorkItemQueue workItems;
};

#endif // THREAD_H
