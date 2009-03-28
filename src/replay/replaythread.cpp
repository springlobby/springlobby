/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
//

#include <wx/app.h>
#include <wx/log.h>

#include "replaythread.h"
#include "../settings++/custom_dialogs.h"
#include "../utils.h"

extern const wxEventType ReplaysLoadedEvt = wxNewEventType();

wxWindow* ReplayLoader::m_parent = 0;

ReplayLoader::ReplayLoader( wxWindow* parent, ReplayList& list, const std::vector<wxString>& filenames  )
    : m_thread_loader( new ReplayLoaderThread ( list, filenames ) )
{
    m_parent = parent;
}

ReplayLoader::~ReplayLoader()
{
}

ReplayLoader::ReplayLoaderThread::ReplayLoaderThread( ReplayList& list, const std::vector<wxString>& filenames  )
    :   m_destroy(false),
        m_replays( list ),
        m_filenames( filenames )
{
    Init();
}

ReplayLoader::ReplayLoaderThread::~ReplayLoaderThread()
{
}

void ReplayLoader::ReplayLoaderThread::Init()
{
    Create();
    Run();
}


void* ReplayLoader::ReplayLoaderThread::Entry()
{
    m_replays.LoadReplays( m_filenames );
//
    if ( m_parent ) {
        wxCommandEvent notice( ReplaysLoadedEvt ,GetId());
        m_parent->ProcessEvent( notice );
    }

    return NULL;
}

bool ReplayLoader::ReplayLoaderThread::TestDestroy()
{
    return m_destroy;
}


void ReplayLoader::ReplayLoaderThread::CloseThread()
{
    m_destroy = true;
}
