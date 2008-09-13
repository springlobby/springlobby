#ifndef SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H
#define SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H

#include <wx/thread.h>


class UnitSyncThread
{
  public:

    UnitSyncThread();
    ~UnitSyncThread();

    void Pause();
    void Resume();
    void Start();
    void Stop();

  protected:

    class UnitSyncThreadImpl : public wxThread
    {

      public:

        UnitSyncThreadImpl();
        void Init();
        void Stop();

        unsigned int GetCurrentMapIndex();
        unsigned int GetCurrentModIndex();

        void SetCurrentModIndex( unsigned int index );
        void SetCurrentMapIndex( unsigned int index );

      protected:

        void* Entry();
        bool TestDestroy();

        bool m_stop_thread;
        unsigned int m_current_map_index;
        unsigned int m_current_mod_index;
    };

    UnitSyncThreadImpl m_thread;

    int m_delay;

    wxCriticalSection m_lock;

};

UnitSyncThread& CacheThread();

#endif // SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H
