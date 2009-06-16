/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */


/*
#include <wx/image.h>

#include "unitsyncthread.h"
#include "iunitsync.h"
#include "utils.h"
#include "settings.h"
#include "springunitsynclib.h"
#include "globalsmanager.h"

#define LOCK_CACHE wxCriticalSectionLocker lock_criticalsection(m_lock)


UnitSyncThread& CacheThread()
{
  static GlobalObjectHolder<UnitSyncThread> m_cache_thread;
  return m_cache_thread;
}

UnitSyncThread::UnitSyncThread()
{
  LoadSettingsFromFile();
}


UnitSyncThread::~UnitSyncThread()
{
  sett().SetMapCachingThreadProgress( m_map_thread.GetCurrentIndex() );
  sett().SetModCachingThreadProgress( m_mod_thread.GetCurrentIndex() );
  Stop();
}


void UnitSyncThread::Pause()
{
  if ( m_map_thread.IsRunning() ) m_map_thread.Pause();
  if ( m_mod_thread.IsRunning() ) m_mod_thread.Pause();
  wxLogMessage( _T("caching thread paused") );
}


void UnitSyncThread::Resume()
{
  if ( !m_map_thread.IsRunning() ) m_map_thread.Resume();
  if ( !m_mod_thread.IsRunning() ) m_mod_thread.Resume();
  wxLogMessage( _T("caching thread resumed") );
}


void UnitSyncThread::Start()
{
  m_map_thread.Init();
  m_mod_thread.Init();
  wxLogMessage( _T("caching thread started") );
}


void UnitSyncThread::Stop()
{
  if( m_map_thread.IsAlive() ) m_map_thread.Stop();
  if( m_mod_thread.IsAlive() ) m_mod_thread.Stop();
  wxLogMessage( _T("caching thread stopped") );
}


void UnitSyncThread::LoadSettingsFromFile()
{
  m_map_thread.SetCurrentIndex( sett().GetModCachingThreadProgress() );
  m_mod_thread.SetCurrentIndex( sett().GetMapCachingThreadProgress() );
}


void UnitSyncThread::UnitSyncThreadImpl::Init()
{
  Create();
  SetPriority( WXTHREAD_MIN_PRIORITY );
  Run();
}


void* UnitSyncThread::MapCacheThread::Entry()
{
  // crashes - here for test to see why
  susynclib().GetMapCount();
  while ( !TestDestroy() )
  {
    if(!Sleep( 20000 ))break;
    // cache map infos
    if( usync().IsLoaded() )
    {
      wxArrayString totalmaps = usync().GetMapList();
      if ( m_current_index > totalmaps.GetCount() ) m_current_index = 0;
      wxString mapname = totalmaps[m_current_index];
      try
      {
        usync().GetMapOptions( mapname );
        usync().GetMapEx( m_current_index );
        usync().GetMinimap( mapname, -1, -1 );
      } catch (...) {}
      m_current_index++;
    }
  }
  return 0;
}


void* UnitSyncThread::ModCacheThread::Entry()
{
  while ( !TestDestroy() )
  {
    if(!Sleep( 67000 ))break;
    // cache mod infos
    if( usync().IsLoaded() )
    {
      wxArrayString totalmods = usync().GetModList();
      if ( m_current_index > totalmods.GetCount() ) m_current_index = 0;
      wxString modname = totalmods[m_current_index];
      try
      {
        usync().GetModOptions( modname );
        usync().GetUnitsList( modname );
      } catch (...) {}
      m_current_index++;
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
  return Thread::TestDestroy() || m_stop_thread;
}


unsigned int UnitSyncThread::UnitSyncThreadImpl::GetCurrentIndex()
{
  return m_current_index;
}


void UnitSyncThread::UnitSyncThreadImpl::SetCurrentIndex( unsigned int index )
{
  m_current_index = index;
}

*/
