/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "prdownloader.h"

#include <lslutils/globalsmanager.h>

#include "lib/src/Downloader/Http/HttpDownloader.h" //FIXME
#include "lib/src/Downloader/IDownloader.h"	 //FIXME: remove this include
#include "lib/src/FileSystem/FileSystem.h"	  //FIXME
#include "lib/src/pr-downloader.h"
// Resolves names collision: CreateDialog from WxWidgets and CreateDialog macro from WINUSER.H
// Remove with HttpDownloader.h header inclusion
#ifdef CreateDialog
#undef CreateDialog
#endif

#include <lslunitsync/unitsync.h>
#include <lslutils/thread.h>
#include <sys/time.h>
#include <wx/log.h>
#include <list>
#include <memory>
#include <mutex>

#include "log.h"
#include "settings.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/slconfig.h"
#include "utils/slpaths.h"

SLCONFIG("/Spring/PortableDownload", false, "true to download portable versions of spring, if false cache/settings/etc are shared (bogous!)");
SLCONFIG("/Spring/RapidMasterUrl", "http://repos.springrts.com/repos.gz", "master url for rapid downloads");

static PrDownloader::DownloadProgress* m_progress = nullptr;
static std::mutex dlProgressMutex;

class DownloadItem : public LSL::WorkItem
{
private:
	DownloadEnum::Category m_category;
	std::string m_name;
	std::string m_filename;

public:
	DownloadItem(const DownloadEnum::Category cat, const std::string& name, const std::string& filename)
	    : m_category(cat)
	    , m_name(name)
	    , m_filename(filename)
	{
		slLogDebugFunc("");
	}

	void Run()
	{
		slLogDebugFunc("");
		wxLogInfo("Starting download of filename: %s, name: %s", m_filename.c_str(), m_name.c_str());

		{
			std::lock_guard<std::mutex> lock(dlProgressMutex);

			if (m_progress == nullptr)
				m_progress = new PrDownloader::DownloadProgress();
			m_progress->name = m_name;
		}

		const bool force = true;
		DownloadSetConfig(CONFIG_RAPID_FORCEUPDATE, &force);
		int results = 0;

		switch (m_category) {
			case DownloadEnum::CAT_SPRINGLOBBY:
			case DownloadEnum::CAT_HTTP:
				results = DownloadAddByUrl(m_category, m_filename.c_str(), m_name.c_str());
				break;
			default:
				results = DownloadSearch(m_category, m_name.c_str());
				break;
		}
		if (results <= 0) {
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadFailed);
			wxLogInfo("Nothing found to download");
			return;
		}

		for (int i = 0; i < results; i++) {
			DownloadAdd(i);
			break; //only add one result
		}

		downloadInfo info;
		const bool hasdlinfo = DownloadGetInfo(0, info);
		//In case if something gone wrong
		if (!hasdlinfo) {
			wxLogWarning("Download has no downloadinfo: %s", m_name.c_str());
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadFailed);
			return;
		}

		GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadStarted);

		const bool downloadFailed = DownloadStart();

		if (downloadFailed) {
			wxLogWarning("Download failed: %s", m_name.c_str());
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadFailed);
		} else {
			wxLogInfo("Download finished: %s", m_name.c_str());
			DownloadFinished(m_category, info);
			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadComplete);
		}
	}

	DownloadEnum::Category getCategory() const
	{
		return m_category;
	}
	const std::string& getFilename() const
	{
		return m_filename;
	}
	const std::string& getName() const
	{
		return m_name;
	}

private:
	void DownloadFinished(DownloadEnum::Category cat, const downloadInfo& info)
	{
		slLogDebugFunc("");

		switch (cat) {
			case DownloadEnum::CAT_ENGINE:
			case DownloadEnum::CAT_ENGINE_LINUX:
			case DownloadEnum::CAT_ENGINE_WINDOWS:
			case DownloadEnum::CAT_ENGINE_LINUX64:
			case DownloadEnum::CAT_ENGINE_MACOSX: {
				SlPaths::RefreshSpringVersionList(); //FIXME: maybe not thread-save!
				std::string version;
				const std::string prefix = "spring ";
				if (m_name.compare(0, prefix.size(), prefix) == 0)
					version = m_name.substr(prefix.size()); //FIXME: hack
				else
					version = m_name;

				SlPaths::SetUsedSpringIndex(version);
				//Inform all application components about new engine been available
				if (!LSL::usync().ReloadUnitSyncLib()) {
					wxLogWarning("Couldn't load downloaded unitsync %s!", version.c_str());
					break;
				}
				GlobalEventManager::Instance()->Send(GlobalEventManager::OnUnitsyncReloaded);
				break;
			}
			case DownloadEnum::CAT_SPRINGLOBBY: {
				const std::string& updatedir = SlPaths::GetUpdateDir();
				const std::string& zipfile = info.filename;
				if (!fileSystem->extract(zipfile, updatedir)) {
					wxLogError("Couldn't extract %s to %s", zipfile.c_str(), updatedir.c_str());
					break;
				}
				GlobalEventManager::Instance()->Send(GlobalEventManager::OnLobbyDownloaded);
				break;
			}
			case DownloadEnum::CAT_MAP:
			case DownloadEnum::CAT_GAME:
				if (!LSL::usync().ReloadUnitSyncLib()) {
					wxLogWarning("Couldn't reload unitsync!");
					break;
				}
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
	std::lock_guard<std::mutex> lock(dlProgressMutex);

	if (m_progress == nullptr) {
		assert(false);
		return;
	}

	progress.name = m_progress->name;
	progress.downloaded = m_progress->downloaded;
	progress.filesize = m_progress->filesize;

	wxLogDebug("%s %d %d", progress.name.c_str(), progress.downloaded, progress.filesize);
}

#ifdef WIN32
#define BILLION (1E9)

static BOOL g_first_time = 1;
static LARGE_INTEGER g_counts_per_sec;

int clock_gettime(int dummy, struct timespec* ct)
{
	LARGE_INTEGER count;
	if (g_first_time) {
		g_first_time = 0;
		if (0 == QueryPerformanceFrequency(&g_counts_per_sec)) {
			g_counts_per_sec.QuadPart = 0;
		}
	}

	if ((NULL == ct) || (g_counts_per_sec.QuadPart <= 0) || (0 == QueryPerformanceCounter(&count))) {
		return -1;
	}
	ct->tv_sec = count.QuadPart / g_counts_per_sec.QuadPart;
	ct->tv_nsec = ((count.QuadPart % g_counts_per_sec.QuadPart) * BILLION) / g_counts_per_sec.QuadPart;
	return 0;
}
#endif

void updatelistener(int downloaded, int filesize)
{
	std::lock_guard<std::mutex> lock(dlProgressMutex);

	if (m_progress == nullptr)
		m_progress = new PrDownloader::DownloadProgress();

	static struct timespec lastupdate = {0, 0};
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (((double)now.tv_sec + 1.0e-9 * now.tv_nsec) - ((double)lastupdate.tv_sec + 1.0e-9 * lastupdate.tv_nsec) < 0.2f) {
		//rate limit events
		return;
	}
	lastupdate.tv_nsec = now.tv_nsec;
	lastupdate.tv_sec = now.tv_sec;

	m_progress->filesize = filesize;
	m_progress->downloaded = downloaded;

	GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadProgress);
}

PrDownloader::PrDownloader()
    : wxEvtHandler()
    , m_dl_thread(new LSL::WorkerThread())
{
	slLogDebugFunc("");

	UpdateSettings();
	IDownloader::Initialize();
	IDownloader::setProcessUpdateListener(updatelistener);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnSpringStarted, PrDownloader::OnSpringStarted);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnSpringTerminated, PrDownloader::OnSpringTerminated);
}

PrDownloader::~PrDownloader()
{
	slLogDebugFunc("");

	GlobalEventManager::Instance()->UnSubscribeAll(this);

	if (!!m_dl_thread) {
		m_dl_thread->Wait();
		delete m_dl_thread;
		m_dl_thread = nullptr;
	}
	IDownloader::Shutdown();

	if (!!m_progress) {
		delete m_progress;
		m_progress = nullptr;
	}
}

void PrDownloader::ClearFinished()
{
	slLogDebugFunc("");
}

void PrDownloader::UpdateSettings()
{
	slLogDebugFunc("");

	DownloadSetConfig(CONFIG_FILESYSTEM_WRITEPATH, SlPaths::GetDownloadDir().c_str());
	//FIXME: fileSystem->setEnginePortableDownload(cfg().ReadBool(_T("/Spring/PortableDownload")));
	// rapidDownload->setOption("masterurl", STD_STRING(cfg().ReadString(_T("/Spring/RapidMasterUrl"))));
}

void PrDownloader::RemoveTorrentByName(const std::string& /*name*/)
{
	slLogDebugFunc("");
}

void PrDownloader::Download(DownloadEnum::Category cat, const std::string& filename, const std::string& url)
{
	slLogDebugFunc("");

	wxLogDebug("Starting download of %s, %s %d", filename.c_str(), url.c_str(), cat);
	DownloadItem* dl_item = new DownloadItem(cat, filename, url);
	m_dl_thread->DoWork(dl_item);
}


void PrDownloader::OnSpringStarted(wxCommandEvent& /*data*/)
{
	slLogDebugFunc("");
	//FIXME: pause downloads
}

void PrDownloader::OnSpringTerminated(wxCommandEvent& /*data*/)
{
	slLogDebugFunc("");
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
	slLogDebugFunc("");

	return m_progress != nullptr && !m_progress->IsFinished();
}

void PrDownloader::UpdateApplication(const std::string& updateurl)
{
	slLogDebugFunc("");

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

bool PrDownloader::DownloadUrl(const std::string& httpurl, std::string& res)
{
	{
		std::lock_guard<std::mutex> lock(dlProgressMutex);
		if (m_progress == nullptr)
			m_progress = new PrDownloader::DownloadProgress();
		m_progress->name = httpurl;
	}
	return CHttpDownloader::DownloadUrl(httpurl, res);
}
