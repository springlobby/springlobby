#include "thread.h"

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
