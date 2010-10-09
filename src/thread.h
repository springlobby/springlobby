#ifndef THREAD_H
#define THREAD_H
#include <wx/thread.h>
#include <vector>

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


class WorkItemQueue;


/** @brief Abstraction of a piece of work to be done by WorkerThread
    Inherit this class to define concrete work items. */
class WorkItem
{
  public:

    /** @brief Construct a new WorkItem */
    WorkItem() : m_priority(0), m_toBeDeleted(true), m_queue(NULL) {}

    /** @brief Destructor */
    virtual ~WorkItem() {}

    /** @brief Implement this in derived class to do the work */
    virtual void Run() = 0;

    /** @brief Cancel this WorkItem, remove it from queue
        @return true if it was removed, false otherwise */
    bool Cancel();

    int GetPriority() const { return m_priority; }

  private:
    int m_priority;              ///< Priority of item, highest is run first
    volatile bool m_toBeDeleted; ///< Should this item be deleted after it has run?
    WorkItemQueue* m_queue;

    friend class WorkItemQueue;
    friend class WorkerThread;
};


/** @brief Priority queue of work items */
class WorkItemQueue
{
  public:
    /** @brief Push more work onto the queue */
    void Push(WorkItem* item);

    /** @brief Pop one work item from the queue
        @return A work item or NULL when the queue is empty */
    WorkItem* Pop();

    /** @brief Remove a specific workitem from the queue
        @return true if it was removed, false otherwise */
    bool Remove(WorkItem* item);

  private:
    wxCriticalSection m_lock;
    // this is a priority queue maintained as a heap stored in a vector :o
    std::vector<WorkItem*> m_queue;
};


/** @brief Thread which processes WorkItems in it's WorkItemQueue */
class WorkerThread : public Thread
{
  public:
    /** @brief Adds a new WorkItem to the queue */
    void DoWork(WorkItem* item, int priority = 0, bool toBeDeleted = true);
    /** @brief Overrides wxThread::Entry, thread entry point */
    void* Entry();

  private:
    void CleanupWorkItem(WorkItem* item);

    WorkItemQueue m_workItems;
};

#endif // THREAD_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

