#ifndef THREAD_H
#define THREAD_H
#include <wx/thread.h>

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


#endif // THREAD_H
