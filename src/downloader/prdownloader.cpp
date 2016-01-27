/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "prdownloader.h"

#include <lslutils/globalsmanager.h>
#include "lib/src/pr-downloader.h"
#include "lib/src/Downloader/IDownloader.h" //FIXME: remove this include
#include "lib/src/FileSystem/FileSystem.h"  //FIXME
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
#include "settings.h"
#include "utils/slconfig.h"


SLCONFIG("/Spring/PortableDownload", false, "true to download portable versions of spring, if false cache/settings/etc are shared (bogous!)");
SLCONFIG("/Spring/RapidMasterUrl", "http://repos.springrts.com/repos.gz", "master url for rapid downloads");

class DownloadItem : public LSL::WorkItem
{
private:
	DownloadEnum::Category m_category;
	std::string m_name;
	std::string m_filename;
	bool m_reload;
public:
	DownloadItem(const DownloadEnum::Category cat, const std::string& name, const std::string& filename)
		: m_category(cat)
		, m_name(name)
		, m_filename(filename)
		, m_reload(false)
	{
	}

	void Run()
	{
		const bool force = true;
		DownloadSetConfig(CONFIG_RAPID_FORCEUPDATE, &force);
		int results = 0;
		switch(m_category) {
			case DownloadEnum::CAT_SPRINGLOBBY:
			case DownloadEnum::CAT_HTTP:
				results = DownloadAddByUrl(m_category, m_filename.c_str(), m_name.c_str());
				break;
			default:
				results = DownloadSearch(m_category, m_name.c_str());
		}
		for (int i=0; i < results; i++) {
			DownloadAdd(i);
			break; //only add one result
		}

		downloadInfo info;
		const bool hasdlinfo = DownloadGetInfo(0, info);
		//In case if something gone wrong
		if (!hasdlinfo) {
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadFailed);
			return;
		}

		UiEvents::ScopedStatusMessage msg("Downloading: " + m_name, 0);

		const bool downloadFailed = DownloadStart();
		//we create this in avance cause m_item gets freed
		wxString d(_("Download complete: " + m_name));

		UiEvents::ScopedStatusMessage msgcomplete(d, 0);

		if (downloadFailed) {
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadFailed);
		} else {
			DownloadFinished(m_category, info);
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadComplete);
		}
	}

private:
	void DownloadFinished(DownloadEnum::Category cat, const downloadInfo& info)
	{
		switch (cat) {
			case DownloadEnum::CAT_ENGINE:
			case DownloadEnum::CAT_ENGINE_LINUX:
			case DownloadEnum::CAT_ENGINE_WINDOWS:
			case DownloadEnum::CAT_ENGINE_LINUX64:
			case DownloadEnum::CAT_ENGINE_MACOSX:
				SlPaths::RefreshSpringVersionList(); //FIXME: maybe not thread-save!
				SlPaths::SetUsedSpringIndex(info.filename);
				//Inform all application components about new engine been available
				LSL::usync().ReloadUnitSyncLib();
				GlobalEventManager::Instance()->Send(GlobalEventManager::OnUnitsyncReloaded);
				break;

			case DownloadEnum::CAT_SPRINGLOBBY:
				fileSystem->extract(info.filename, SlPaths::GetUpdateDir());
				GlobalEventManager::Instance()->Send(GlobalEventManager::OnLobbyDownloaded);
				break;
			case DownloadEnum::CAT_MAP:
			case DownloadEnum::CAT_GAME:
				LSL::usync().ReloadUnitSyncLib();
				if (cat == DownloadEnum::CAT_MAP) {
					LSL::usync().PrefetchMap(m_name);
				} else {
					LSL::usync().PrefetchGame(m_name);
				}
				GlobalEventManager::Instance()->Send(GlobalEventManager::OnUnitsyncReloaded);
				break;
			default:
				wxLogError("Unknown category: %d", cat);
				assert(false);
				break;
		}
	}
};

static int m_filesize = 0;
static int m_downloaded = 0;

void PrDownloader::GetProgress(DownloadProgress& progress)
{
	//TODO: add mutex
	progress.downloaded = m_downloaded;
	progress.filesize = m_filesize;
}

void updatelistener(int downloaded, int filesize)
{
	//TODO: add mutex
	m_filesize = filesize;
	m_downloaded = downloaded;
	GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadProgress);
}

PrDownloader::PrDownloader()
    : wxEvtHandler()
    , m_dl_thread(new LSL::WorkerThread())
{
	IDownloader::setProcessUpdateListener(updatelistener);
	UpdateSettings();
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
	DownloadSetConfig(CONFIG_FILESYSTEM_WRITEPATH, SlPaths::GetDownloadDir().c_str());
	//FIXME: fileSystem->setEnginePortableDownload(cfg().ReadBool(_T("/Spring/PortableDownload")));
	// rapidDownload->setOption("masterurl", STD_STRING(cfg().ReadString(_T("/Spring/RapidMasterUrl"))));
}

void PrDownloader::RemoveTorrentByName(const std::string& /*name*/)
{
}

void PrDownloader::Download(DownloadEnum::Category cat, const std::string& filename, const std::string& url)
{
	DownloadItem* dl_item = new DownloadItem(cat, filename, url);
	m_dl_thread->DoWork(dl_item);
}


void PrDownloader::OnSpringStarted(wxCommandEvent& /*data*/)
{
	//FIXME: pause downloads
}

void PrDownloader::OnSpringTerminated(wxCommandEvent& /*data*/)
{
	//FIXME: resume downloads
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
