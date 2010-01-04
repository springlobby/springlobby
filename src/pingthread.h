#ifndef PINGTHREAD_H
#define PINGTHREAD_H


#include "inetclass.h"
#include "thread.h"


/** A thread class that sends pings to socket.
 * Implemented as joinable thread.
 * When you want it started, construct it then call Init()
 * When you want it killed, call Wait() method.
 * Dont call other methods, especially the Destroy() method.
 */
class PingThread: public Thread
{
  public:
	PingThread( iNetClass& parent, unsigned int interval );
	void Init();
	private:
	void* Entry();
	void OnExit();
	iNetClass& m_parent;
	unsigned int m_interval;
};

#endif // PINGTHREAD_H
