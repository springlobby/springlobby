/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "prdownloader.h"

#include <lslutils/globalsmanager.h>
#include "lib/src/Downloader/IDownloader.h"
#include "lib/src/FileSystem/FileSystem.h"
#include "lib/src/FileSystem/File.h"
#include "utils/uievents.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/slpaths.h"
#include "gui/mainwindow.h"
#include "downloadsobserver.h"
#include <list>

#include <wx/log.h>
#include <lslunitsync/unitsync.h>
#include <lslutils/thread.h>
#include <settings.h>
#include "utils/slconfig.h"


SLCONFIG("/Spring/PortableDownload", false, "true to download portable versions of spring, if false cache/settings/etc are shared (bogous!)");
SLCONFIG("/Spring/RapidMasterUrl", "http://repos.springrts.com/repos.gz", "master url for rapid downloads");

std::string PrDownloader::GetEngineCat()
{
#ifdef WIN32
	return "engine_windows";
#elif defined(__APPLE__)
	return "engine_macosx";
#elif defined(__x86_64__)
	return "engine_linux64";
#else
	return "engine_linux";
#endif
}

class DownloadItem : public LSL::WorkItem
{
public:
	DownloadItem(std::list<IDownload*> item, IDownloader* loader)
		: m_item(item)
		, m_loader(loader)
		, m_reload(false)
		, m_lobbydl(false)
	{
	}

	void Run()
	{
		if (m_item.empty()) {
			return;
		}
		UiEvents::ScopedStatusMessage msg("Downloading: " + m_item.front()->name, 0);
		//we create this in avance cause m_item gets freed
		wxString d(_("Download complete: "));
		d += TowxString(m_item.front()->name);
		bool downloadFailed = (m_loader->download(m_item, sett().GetHTTPMaxParallelDownloads()) != true);
		if (downloadFailed) {
			wxString errorMessage = wxString::Format(_("Failed to download %s!"), m_item.front()->name);
			wxLogError(errorMessage);
		}

		if (m_reload) {
			LSL::usync().ReloadUnitSyncLib();
			// prefetch map data after a download as well
			for (IDownload* dl: m_item) {
				switch (dl->cat) {
					case IDownload::CAT_MAPS: {
						LSL::usync().PrefetchMap(dl->name);
						break;
					}
					case IDownload::CAT_GAMES: {
						LSL::usync().PrefetchGame(dl->name);
						break;
					}
					default:
						continue;
				}
			}

			//notify about finished download
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnUnitsyncReloaded);
		}
		for (IDownload* dl: m_item) {
			DownloadFinished(dl);
		}

		UiEvents::ScopedStatusMessage msgcomplete(d, 0);
		if (m_lobbydl) {
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnLobbyDownloaded);
		}
		m_loader->freeResult(m_item);
	}

private:
	void DownloadFinished(IDownload* dl)
	{
			switch (dl->cat) {
				case IDownload::CAT_ENGINE_LINUX:
				case IDownload::CAT_ENGINE_WINDOWS:
				case IDownload::CAT_ENGINE_LINUX64:
				case IDownload::CAT_ENGINE_MACOSX:
					if (fileSystem->extractEngine(dl->name, dl->version) == false) {
						wxLogError(_("Failed to extract downloaded engine!"));
						break;
					}
					SlPaths::RefreshSpringVersionList(); //FIXME: maybe not thread-save!
					SlPaths::SetUsedSpringIndex(dl->version);
					//Inform all application components about new engine been available
					GlobalEventManager::Instance()->Send(GlobalEventManager::OnUnitsyncReloaded);
					break;

				case IDownload::CAT_LOBBYCLIENTS:
					m_lobbydl = true;
					if (fileSystem->extract(dl->name, SlPaths::GetUpdateDir(), true) == false) {
						wxLogError(_("Failed to extract downloaded lobby client!"));
					}
					break;

				case IDownload::CAT_MAPS:
				case IDownload::CAT_GAMES:
					m_reload = true;
					break;

				default:
					break;
			}
	}

private:
	std::list<IDownload*> m_item;
	IDownloader* m_loader;
	bool m_reload;
	bool m_lobbydl;
};

class SearchItem : public LSL::WorkItem
{
public:
	SearchItem(std::list<IDownloader*> loaders, std::string name, IDownload::category cat);
	void Run();

private:
	const std::list<IDownloader*> m_loaders;
	const std::string m_name;
	const IDownload::category m_cat;
	int m_result_size;
};

SearchItem::SearchItem(std::list<IDownloader*> loaders, const std::string name, IDownload::category cat)
    : m_loaders(loaders)
    , m_name(name)
    , m_cat(cat)
    , m_result_size(0)
{
}

void SearchItem::Run()
{
	rapidDownload->setOption("forceupdate", "");
	std::list<IDownload*> results;
	for (IDownloader* loader: m_loaders) {
		loader->search(results, m_name, m_cat);
		if (!results.empty()) {
			DownloadItem* dl_item = new DownloadItem(results, loader);
			prDownloader().m_dl_thread->DoWork(dl_item);
			m_result_size = results.size();
			return;
		}
	}
	wxLogError(wxString::Format("Item (%s) requested for download could not be found. Sorry.", m_name.c_str()));
	return;
}


PrDownloader::PrDownloader()
    : wxEvtHandler()
    , m_dl_thread(new LSL::WorkerThread())
{
	IDownloader::Initialize(&downloadsObserver());
	UpdateSettings();
	m_game_loaders.push_back(rapidDownload);
	m_game_loaders.push_back(httpDownload);
	m_map_loaders.push_back(httpDownload);
	m_engine_loaders.push_back(httpDownload);
	GlobalEventManager::Instance()->Subscribe(this, GlobalEventManager::OnSpringStarted, wxObjectEventFunction(&PrDownloader::OnSpringStarted));
	GlobalEventManager::Instance()->Subscribe(this, GlobalEventManager::OnSpringTerminated, wxObjectEventFunction(&PrDownloader::OnSpringTerminated));
}

PrDownloader::~PrDownloader()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);

	delete m_dl_thread;
	m_dl_thread = nullptr;
	IDownloader::Shutdown();
}

void PrDownloader::ClearFinished()
{
}

void PrDownloader::UpdateSettings()
{
	fileSystem->setWritePath(SlPaths::GetDownloadDir());
	fileSystem->setEnginePortableDownload(cfg().ReadBool(_T("/Spring/PortableDownload")));
	rapidDownload->setOption("masterurl", STD_STRING(cfg().ReadString(_T("/Spring/RapidMasterUrl"))));
}

void PrDownloader::RemoveTorrentByName(const std::string& /*name*/)
{
}

int PrDownloader::GetDownload(const std::string& category, const std::string& name)
{
	std::string cat = category;
	if (category == "engine") { //automaticly select engine from current platform
		cat = PrDownloader::GetEngineCat();
	}
	const IDownload::category prcat = IDownload::getCatFromStr(cat);
	switch(prcat) {
		case IDownload::CAT_GAMES:
			return Get(m_game_loaders, name, prcat);
		case IDownload::CAT_MAPS:
			return Get(m_map_loaders, name, prcat);
		case IDownload::CAT_ENGINE_LINUX:
		case IDownload::CAT_ENGINE_WINDOWS:
		case IDownload::CAT_ENGINE_LINUX64:
		case IDownload::CAT_ENGINE_MACOSX:
			return Get(m_engine_loaders, name, prcat);
		default:
			break;
	}
	wxLogWarning("Unhandled category: %s", category.c_str());
	return false;
}

bool PrDownloader::Download(const std::string& filename, const std::string& url)
{
	assert(!filename.empty());
	assert(!url.empty());
	std::list<IDownload*> results;
	IDownload* dl = new IDownload();
	dl->addMirror(url);
	dl->name = filename;
	dl->cat = IDownload::CAT_LOBBYCLIENTS;

	results.push_back(dl);
	DownloadItem* dl_item = new DownloadItem(results, httpDownload);
	m_dl_thread->DoWork(dl_item);
	return true;
}


void PrDownloader::OnSpringStarted(wxCommandEvent& /*data*/)
{
	//FIXME: pause downloads
}

void PrDownloader::OnSpringTerminated(wxCommandEvent& /*data*/)
{
	//FIXME: resume downloads
}

int PrDownloader::Get(std::list<IDownloader*> loaders, const std::string& name, IDownload::category cat)
{
	SearchItem* searchItem = new SearchItem(loaders, name, cat);
	m_dl_thread->DoWork(searchItem);
	return 1;
}

PrDownloader& prDownloader()
{
	static LSL::Util::LineInfo<PrDownloader> m(AT);
	static LSL::Util::GlobalObjectHolder<PrDownloader, LSL::Util::LineInfo<PrDownloader> > s_PrDownloader(m);
	return s_PrDownloader;
}

bool PrDownloader::IsRunning()
{
	return !downloadsObserver().IsEmpty();
}
