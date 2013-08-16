#ifndef THREAD_H
#define THREAD_H
#include <wx/thread.h>
#include <vector>
#include "utils/mixins.hh"

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

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

