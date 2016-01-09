/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/app.h>
#include <wx/log.h>

#include "replaylist.h"
#include "savegamelist.h"
#include "playbackthread.h"
#include "gui/playback/playbacktab.h"
#include <lslunitsync/unitsync.h>

PlaybackLoader::PlaybackLoader(PlaybackTab* parent, bool IsReplayType)
    : wxEvtHandler()
    , m_parent(parent)
    , m_thread_loader(NULL)
    , m_isreplaytype(IsReplayType)
{
	assert(m_parent != NULL);
}

const wxEventType PlaybackLoader::PlaybacksLoadedEvt = wxNewEventType();

PlaybackLoader::~PlaybackLoader()
{
	delete m_thread_loader;
}

void PlaybackLoader::Run()
{
	if (!LSL::usync().IsLoaded())
		return;
	if (m_thread_loader)
		return; // a thread is already running
	m_filenames = LSL::usync().GetPlaybackList(m_isreplaytype);
	m_thread_loader = new PlaybackLoaderThread(this, m_parent);
	m_thread_loader->Create();
	m_thread_loader->Run();
}

void PlaybackLoader::OnComplete()
{
	if (m_parent == NULL)
		return;
	wxCommandEvent notice(PlaybacksLoadedEvt, 1);
	wxPostEvent(m_parent, notice);
	m_thread_loader = NULL; // the thread object deleted itself
}

std::vector<std::string> PlaybackLoader::GetPlaybackFilenames()
{
	return m_filenames;
}

PlaybackLoader::PlaybackLoaderThread::PlaybackLoaderThread(PlaybackLoader* loader, PlaybackTab* parent)
    : m_parent(parent)
    , m_loader(loader)
{
	assert(m_parent != NULL);
}

void* PlaybackLoader::PlaybackLoaderThread::Entry()
{
	if (m_parent) {
		replaylist().LoadPlaybacks(m_loader->GetPlaybackFilenames());
		m_loader->OnComplete();
	}

	return NULL;
}
