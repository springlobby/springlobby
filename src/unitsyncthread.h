#ifndef SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H
#define SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H

/*
#include "thread.h"


class UnitSyncThread
{
  public:

    UnitSyncThread();
    ~UnitSyncThread();

    void Pause();
    void Resume();
    void Start();
    void Stop();

    void LoadSettingsFromFile();

  protected:

    class UnitSyncThreadImpl : public Thread
    {
      public:
        UnitSyncThreadImpl() : m_stop_thread( false ) {}
        void Init();
        void Stop();

        unsigned int GetCurrentIndex();
        void SetCurrentIndex( unsigned int index );

      protected:
        bool TestDestroy();

        bool m_stop_thread;
        unsigned int m_current_index;
    };

    class MapCacheThread : public UnitSyncThreadImpl
    {
      public:
        MapCacheThread() {}
      protected:
        void* Entry();
    };

    class ModCacheThread : public UnitSyncThreadImpl
    {
      public:
        ModCacheThread() {}
      protected:
        void* Entry();
    };

    MapCacheThread m_map_thread;
    ModCacheThread m_mod_thread;

    wxCriticalSection m_lock;
};

UnitSyncThread& CacheThread();
*/

#endif // SPRINGLOBBY_HEADERGUARD_UNITSYNCTHREAD_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

