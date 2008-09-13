/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */


#include <wx/image.h>

#include "unitsyncthread.h"
#include "iunitsync.h"
#include "utils.h"
#include "settings.h"

#define LOCK_CACHE wxCriticalSectionLocker lock_criticalsection(m_lock)


UnitSyncThread& CacheThread()
{
  static UnitSyncThread m_cache_thread;
  return m_cache_thread;
}

UnitSyncThread::UnitSyncThread()
{
  LoadSettingsFromFile();
}


UnitSyncThread::~UnitSyncThread()
{
  sett().SetMapCachingThreadProgress( m_thread.GetCurrentMapIndex() );
  sett().SetModCachingThreadProgress( m_thread.GetCurrentModIndex() );
  if( m_thread.IsAlive() ) Stop();
}


void UnitSyncThread::Pause()
{
  if ( m_thread.IsRunning() )
  {
    m_thread.Pause();
    wxLogMessage( _T("caching thread paused") );
  }
}


void UnitSyncThread::Resume()
{
  if ( !m_thread.IsRunning() )
  {
     m_thread.Resume();
     wxLogMessage( _T("caching thread resumed") );
  }
}


void UnitSyncThread::Start()
{
  wxLogMessage( _T("caching thread started") );
  m_thread.Init();
}


void UnitSyncThread::Stop()
{
  wxLogMessage( _T("caching thread stopped") );
  m_thread.Stop();
}


void UnitSyncThread::LoadSettingsFromFile()
{
  m_thread.SetCurrentModIndex( sett().GetModCachingThreadProgress() );
  m_thread.SetCurrentMapIndex( sett().GetMapCachingThreadProgress() );
}


UnitSyncThread::UnitSyncThreadImpl::UnitSyncThreadImpl():
m_stop_thread( false )
{
}

void UnitSyncThread::UnitSyncThreadImpl::Init()
{
  Create();
  SetPriority( WXTHREAD_MIN_PRIORITY );
  Run();
}

void* UnitSyncThread::UnitSyncThreadImpl::Entry()
{
  while ( !TestDestroy() )
  {
    Sleep( 30000 );
    /// cache map infos
    if( usync().IsLoaded() )
    {
      wxArrayString totalmaps = usync().GetMapList();
      if ( m_current_map_index > totalmaps.GetCount() ) m_current_map_index = 0;
      wxString mapname = totalmaps[m_current_map_index];
      try
      {
        usync().GetMapOptions( mapname );
        usync().GetMapEx( m_current_map_index );
        usync().GetMinimap( mapname, -1, -1 );
      } catch (...) {}
      m_current_map_index++;
    }

    if ( TestDestroy() ) return 0; /// this check is added so if the thread is paused or stopped it won't attempt to try the next step before closing

    Sleep( 30000 );
    /// cache mod infos
    if( usync().IsLoaded() )
    {
      wxArrayString totalmods = usync().GetModList();
      if ( m_current_mod_index > totalmods.GetCount() ) m_current_mod_index = 0;
      wxString modname = totalmods[m_current_mod_index];
      try
      {
        usync().GetModOptions( modname );
        unsigned int sidecount = usync().GetSideCount( modname );
        for ( unsigned int i = 0; i < sidecount; i++ )
        {
          usync().GetSidePicture( modname, usync().GetSideName( modname, i ) );
        }
        usync().GetAIList( modname );
        usync().GetUnitsList( modname );
      } catch (...) {}
      m_current_mod_index++;
    }

  }
  return 0;
}


void UnitSyncThread::UnitSyncThreadImpl::Stop()
{
  m_stop_thread = true;
}


bool UnitSyncThread::UnitSyncThreadImpl::TestDestroy()
{
  return m_stop_thread;
}


unsigned int UnitSyncThread::UnitSyncThreadImpl::GetCurrentMapIndex()
{
  return m_current_map_index;
}


unsigned int UnitSyncThread::UnitSyncThreadImpl::GetCurrentModIndex()
{
  return m_current_mod_index;
}


void UnitSyncThread::UnitSyncThreadImpl::SetCurrentMapIndex( unsigned int index )
{
  m_current_map_index = index;
}


void UnitSyncThread::UnitSyncThreadImpl::SetCurrentModIndex( unsigned int index )
{
  m_current_mod_index = index;
}

