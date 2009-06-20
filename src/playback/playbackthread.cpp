/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
//

#include <wx/app.h>
#include <wx/log.h>

#include "../utils.h"
#include "replaylist.h"
#include "savegamelist.h"
#include "../iunitsync.h"

template <class PlaybackTabImp >
PlaybackLoader<PlaybackTabImp>::PlaybackLoader( ParentType* parent )
    : m_parent( parent ),
    m_thread_loader( 0 )
{
}

template <class PlaybackTabImp >
PlaybackLoader<PlaybackTabImp>::~PlaybackLoader()
{
}

template <class PlaybackTabImp >
void PlaybackLoader<PlaybackTabImp>::Run()
{
    if ( !usync().IsLoaded() ) return;
    if ( m_thread_loader ) return; // a thread is already running
    m_filenames = usync().GetPlaybackList( IsReplayType );
    playbacklist<ListType>().RemoveAll();
    m_thread_loader = new ThreadType();
    m_thread_loader->SetParent( this );
    m_thread_loader->Create();
    m_thread_loader->Run();
}

template <class PlaybackTabImp >
void PlaybackLoader<PlaybackTabImp>::OnComplete()
{
    if ( !m_parent ) return;
        wxCommandEvent notice( PlaybacksLoadedEvt, 1 );
    m_parent->ProcessEvent( notice );
    m_thread_loader = 0; // the thread object deleted itself
}

template <class PlaybackTabImp >
wxArrayString PlaybackLoader<PlaybackTabImp>::GetPlaybackFilenames()
{
	return m_filenames;
}

template <class PlaybackTabImp >
PlaybackLoader<PlaybackTabImp>::PlaybackLoaderThread::PlaybackLoaderThread():
m_parent(0)
{
}

template <class PlaybackTabImp >
void* PlaybackLoader<PlaybackTabImp>::ThreadType::Entry()
{
    if( m_parent )
    {
        playbacklist<ListType>().LoadPlaybacks( m_parent->GetPlaybackFilenames() );
        m_parent->OnComplete();
    }

    return NULL;
}

template <class PlaybackTabImp >
void PlaybackLoader<PlaybackTabImp>::PlaybackLoaderThread::SetParent( ParentType* parent )
{
	m_parent = parent;
}
