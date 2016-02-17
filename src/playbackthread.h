/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD
#define SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD

#include <wx/string.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <set>

class PlaybackTab;


class PlaybackLoader : public wxEvtHandler
{
private:
	class PlaybackLoaderThread : public wxThread
	{
	public:
		PlaybackLoaderThread(PlaybackLoader* loader, PlaybackTab* parent, bool isreplaytype);
		void* Entry();

	protected:
		PlaybackTab* m_parent;
		PlaybackLoader* m_loader;
		bool m_isreplaytype;
	};

public:
	static const wxEventType PlaybacksLoadedEvt;

	PlaybackLoader(PlaybackTab* parent, bool IsReplayType);
	~PlaybackLoader();
	void OnComplete();
	void Run();

private:
	std::set<std::string> m_filenames;
	PlaybackTab* m_parent;
	PlaybackLoaderThread* m_thread_loader;
	bool m_isreplaytype;
};

#endif // SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD
