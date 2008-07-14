#ifndef SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H
#define SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H

#include <wx/thread.h>
#include <wx/event.h>


class wxArrayString;
class Ui;

typedef wxChar JobType;
#define JT_PAUSE ' '
#define JT_MAPINFO 'i'
#define JT_MINIMAP 'm'
#define JT_UNITS 'u'
#define JT_RELOAD 'r'


BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE( wxEVT_UNITSYNC_CACHE, 1 )
END_DECLARE_EVENT_TYPES()


class UnitSyncThread: public wxEvtHandler
{
  public:

    UnitSyncThread( Ui& ui );
    ~UnitSyncThread();

    void Init() {
      m_thread.Create();
      m_thread.SetPriority( WXTHREAD_MIN_PRIORITY );
      m_thread.Run();
    }

    void OnExit();

    void AddMapInfoOrder( const wxString& map );
    void AddMinimapOrder( const wxString& map );
    void AddModUnitsOrder( const wxString& mod );
    void AddReloadOrder();

    void Pause();
    void Resume();

    void Delete();

    void OnMapInfoCached( wxCommandEvent& event );
    void OnMinimapCached( wxCommandEvent& event );
    void OnModUnitsCached( wxCommandEvent& event );
    void OnReloadComplete( wxCommandEvent& event );

  protected:

    friend class UnitSyncThreadImpl;
    class UnitSyncThreadImpl : public wxThread
    {
      protected:
        UnitSyncThread* m_parent;
        void* Entry();
      public:
        UnitSyncThreadImpl(UnitSyncThread* parent) : m_parent(parent) {}
    };

    void _CacheLoop();
    bool _GetNextJob( JobType& jobtype, wxString& params );

    void _AddJob( const wxChar& prefix, const wxString& params );

    void _DoMapInfoJob( const wxString& map );
    void _DoMinimapJob( const wxString& map );
    void _DoUnitsJob( const wxString& mod );
    void _DoReloadJob();

    UnitSyncThreadImpl m_thread;

    Ui& m_ui;

    int m_delay;
    bool m_should_terminate;
    bool m_should_pause;

    wxArrayString m_orders;
    wxCriticalSection m_lock;
    wxCriticalSection m_wait;
    wxString m_last_job;


    DECLARE_EVENT_TABLE()

};


enum {
  CACHE_MINIMAP = wxID_HIGHEST,
  CACHE_MAPINFO,
  CACHE_UNITS,
  CACHE_RELOAD
};


#endif // SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H
