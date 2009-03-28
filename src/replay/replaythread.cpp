/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
//

#include <wx/app.h>
#include <wx/log.h>

#include "replaythread.h"
#include "../utils.h"
#include "replaylist.h"
#include "iunitsync.h"

extern const wxEventType ReplaysLoadedEvt = wxNewEventType();


ReplayLoader::ReplayLoader( wxWindow* parent ):
m_parent( parent )
{
	m_thread_loader.SetParent( this );
}

ReplayLoader::~ReplayLoader()
{
}

void ReplayLoader::Run()
{
		if ( !usync().IsLoaded() ) return;
		m_filenames = usync().GetReplayList();
		replaylist().RemoveAll();
	  m_thread_loader.Create();
    m_thread_loader.Run();
}

void ReplayLoader::OnComplete()
{
		if ( !m_parent ) return;
		wxCommandEvent notice( ReplaysLoadedEvt, 1 );
		m_parent->ProcessEvent( notice );
}

wxArrayString ReplayLoader::GetReplayFilenames()
{
	return m_filenames;
}


ReplayLoader::ReplayLoaderThread::ReplayLoaderThread():
m_parent(0)
{
}

void ReplayLoader::ReplayLoaderThread::SetParent( ReplayLoader* parent )
{
	m_parent = parent;
}

void* ReplayLoader::ReplayLoaderThread::Entry()
{
		if( m_parent )
		{
			replaylist().LoadReplays( m_parent->GetReplayFilenames() );
			m_parent->OnComplete();
		}

    return NULL;
}

