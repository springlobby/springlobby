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

#include <list>
#include <wx/log.h>
#include <lslunitsync/unitsync.h>
#include <lslutils/thread.h>
#include "settings.h"
#include "utils/slconfig.h"

SLCONFIG("/Spring/PortableDownload", false, "true to download portable versions of spring, if false cache/settings/etc are shared (bogous!)");
SLCONFIG("/Spring/RapidMasterUrl", "http://repos.springrts.com/repos.gz", "master url for rapid downloads");

static PrDownloader::DownloadProgress *m_progress = nullptr;

class DownloadItem : public LSL::WorkItem
{
private:
	DownloadEnum::Category m_category;
	std::string m_name;
	std::string m_filename;
	bool m_reload;
	IDownloadItemListener* m_listener;

public:
	DownloadItem(IDownloadItemListener* listener, const DownloadEnum::Category cat, const std::string& name, const std::string& filename)
		: m_category(cat)
		, m_name(name)
		, m_filename(filename)
		, m_reload(false)
		, m_listener(listener)
	{
	}

	void Run()
	{
		if (m_progress == nullptr)
			m_progress = new PrDownloader::DownloadProgress();
		m_progress->name = m_name;

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
		if (results <= 0) {
			m_listener->DownloadFailed(this);
			wxLogInfo("Nothing found to download");
			return;
		}
		for (int i=0; i < results; i++) {
			DownloadAdd(i);
			break; //only add one result
		}

		downloadInfo info;
		const bool hasdlinfo = DownloadGetInfo(0, info);
		//In case if something gone wrong
		if (!hasdlinfo) {
			m_listener->DownloadFailed(this);
			return;
		}
		m_progress->name = m_name; //update to fetched name

		m_listener->DownloadStarted(this);

		const bool downloadFailed = DownloadStart();

		if (downloadFailed) {
			m_listener->DownloadFailed(this);
		} else {
			DownloadFinished(m_category, info);
			m_listener->DownloadFinished(this);
		}
	}

	DownloadEnum::Category getCategory() const {
		return m_category;
	}
	const std::string& getFilename() const {
		return m_filename;
	}
	const std::string& getName() const {
		return m_name;
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

void PrDownloader::GetProgress(DownloadProgress& progress)
{
	assert(wxThread::IsMain());
	if (m_progress == nullptr)
		return;
	//TODO: add mutex
	progress.name = m_progress->name;
	progress.downloaded = m_progress->downloaded;
	progress.filesize = m_progress->filesize;
	wxLogDebug("%s %d %d", progress.name.c_str(), progress.downloaded, progress.filesize);
}

void updatelistener(int downloaded, int filesize)
{
	assert(!wxThread::IsMain());
	//TODO: add mutex
	if (m_progress == nullptr)
		m_progress = new PrDownloader::DownloadProgress();
	m_progress->filesize = filesize;
	m_progress->downloaded = downloaded;
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
	delete m_progress;
	m_progress = nullptr;
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
	DownloadItem* dl_item = new DownloadItem(this, cat, filename, url);
	m_dl_thread->DoWork(dl_item);

	if (m_progress == nullptr) {
		m_progress = new PrDownloader::DownloadProgress();
	}
	m_progress->name = filename;
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
	return m_progress != nullptr && !m_progress->finished;
}

void PrDownloader::DownloadMap(const std::string& filename) {
	Download(DownloadEnum::CAT_MAP, filename);
}

void PrDownloader::DownloadGame(const std::string& filename) {
	Download(DownloadEnum::CAT_GAME, filename);
}

void PrDownloader::DownloadEngine(const std::string& filename) {
	Download(DownloadEnum::CAT_ENGINE, filename);
}

void PrDownloader::UpdateApplication(const std::string& updateurl)
{
	const std::string updatedir = SlPaths::GetUpdateDir();
	const size_t mindirlen = 9; // safety, minimal is/should be: C:\update
	if ((updatedir.size() <= mindirlen)) {
		wxLogError(_T("Invalid update dir: ") + TowxString(updatedir));
		return;
	}
	if (wxDirExists(updatedir)) {
		if (!SlPaths::RmDir(updatedir)) {
			wxLogError(_T("Couldn't cleanup ") + TowxString(updatedir));
			return;
		}
	}
	if (!SlPaths::mkDir(updatedir)) {
		wxLogError(_T("couldn't create update directory") + TowxString(updatedir));
		return;
	}

	if (!wxFileName::IsDirWritable(updatedir)) {
		wxLogError(_T("dir not writable: ") + TowxString(updatedir));
		return;
	}

	const std::string dlfilepath = SlPaths::GetLobbyWriteDir() + "springlobby-latest.zip";
	if (wxFileExists(dlfilepath) && !(wxRemoveFile(dlfilepath))) {
		wxLogError(_T("couldn't delete: ") + dlfilepath);
		return;
	}
	Download(DownloadEnum::CAT_SPRINGLOBBY, updateurl, dlfilepath);
}

void PrDownloader::DownloadStarted(const DownloadItem* item) {
	wxString d(_("Downloading: " + item->getName()));
	UiEvents::ScopedStatusMessage msg(d, 0);

	GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadStarted);
}

void PrDownloader::DownloadFailed(const DownloadItem* item) {
	UiEvents::ScopedStatusMessage msgcomplete(wxEmptyString, 0);

	GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadFailed);
}

void PrDownloader::DownloadFinished(const DownloadItem* item) {
	wxString d(_("Download complete: " + item->getName()));
	UiEvents::ScopedStatusMessage msgcomplete(d, 0);

	GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadComplete);
}
