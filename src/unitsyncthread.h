#ifndef SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H
#define SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H

#include <wx/thread.h>


class wxArrayString;

class UnitSyncThread: public wxThread
{

  public:

    UnitSyncThread();
    ~UnitSyncThread();

    int Entry();
    void OnExit();

    void AddMapInfoOrder( const wxString& map, int startdelay = -1 );
    void AddMinimapOrder( const wxString& map, int startdelay = -1 );
    void AddModUnitsOrder( const wxString& mod, int startdelay = -1 );

    void Pause();
    void Resume();

  protected:

    void _CacheLoop();

    wxArrayString m_orders;
    wxCriticalSection m_lock;

}


#endif // SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H
