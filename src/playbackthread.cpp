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

	m_thread_loader = new PlaybackLoaderThread(this, m_parent, m_isreplaytype);
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

PlaybackLoader::PlaybackLoaderThread::PlaybackLoaderThread(PlaybackLoader* loader, PlaybackTab* parent, bool isreplaytype)
    : m_parent(parent)
    , m_loader(loader)
	, m_isreplaytype(isreplaytype)
{
	assert(m_parent != NULL);
}

void* PlaybackLoader::PlaybackLoaderThread::Entry()
{
	if (m_parent) {
		std::set<std::string> filenames;
		if (!LSL::usync().GetPlaybackList(filenames, m_isreplaytype)) {
			wxLogWarning("Couldn't load list of playbacks.");
			return nullptr;
		}
		replaylist().LoadPlaybacks(filenames);
		m_loader->OnComplete();
	}

	return nullptr;
}
