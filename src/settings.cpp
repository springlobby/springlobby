/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: Settings
//

#include "settings.h"

#include <lslutils/globalsmanager.h>
#include <wx/cmndata.h>
#include <wx/colour.h>
#include <wx/colourdata.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <cstdlib>
#include <set>

#include "playbackfiltervalues.h"
#include "springsettings/presets.h"
#include "utils/conversion.h"
#include "utils/platform.h"
#include "utils/slconfig.h"
#include "utils/slpaths.h"
#include "utils/wxTranslationHelper.h"

const wxChar sep = wxFileName::GetPathSeparator();
const wxString sepstring = wxString(sep);

const long CACHE_VERSION = 15;
const long SETTINGS_VERSION = 32;
SLCONFIG("/General/SettingsVersion", SETTINGS_VERSION, "version of settings file");
SLCONFIG("/General/CacheVersion", CACHE_VERSION, "version of used cache");
SLCONFIG("/General/firstrun", true, "true if app is run first time");


// http://stackoverflow.com/questions/3025997/defining-static-const-integer-members-in-class-definition
// definitions of static vars
const int Settings::columnWidthMinimum;
const int Settings::columnWidthUnset;
const size_t ScreenPosition::bottom_right;
const size_t ScreenPosition::bottom_left;
const size_t ScreenPosition::top_right;
const size_t ScreenPosition::top_left;

Settings& sett()
{
	static LSL::Util::LineInfo<Settings> m(AT);
	static LSL::Util::GlobalObjectHolder<Settings, LSL::Util::LineInfo<Settings> > m_sett(m);
	return m_sett;
}

Settings::Settings()
{
}

Settings::~Settings()
{
}


void Settings::Setup(wxTranslationHelper* translationhelper)
{

	long settversion = cfg().ReadLong(_T("/General/SettingsVersion"));
	long cacheversion = cfg().ReadLong(_T( "/General/CacheVersion" ));

	const wxString userConfigDir = TowxString(SlPaths::GetConfigfileDir());
	if ((cacheversion < CACHE_VERSION) && !IsFirstRun()) {
		SlPaths::RmDir(SlPaths::GetCachePath());
		// after resetting cache, set current cache version
		cfg().Write(_T( "/General/CacheVersion" ), CACHE_VERSION);
	}

	SlPaths::mkDir(STD_STRING(userConfigDir));

	if (!cfg().ReadBool(_T("/General/firstrun"))) {
		ConvertSettings(translationhelper, settversion);
	}

	if (ShouldAddDefaultServerSettings() || (settversion < 14 && GetServers().Count() < 2))
		SetDefaultServerSettings();

	if (ShouldAddDefaultChannelSettings()) {
		AddChannelJoin(_T("newbies"), wxEmptyString);
		if (translationhelper) {
			if (translationhelper->GetLocale()) {
				wxString localecode = translationhelper->GetLocale()->GetCanonicalName();
				if (localecode.Find(_T("_")) != -1)
					localecode = localecode.BeforeFirst(_T('_'));
				AddChannelJoin(localecode, wxEmptyString); // add locale's language code to autojoin
			}
		}
	}
}

void Settings::ConvertSettings(wxTranslationHelper* translationhelper, long settversion)
{
	switch (settversion) { //no breaks! thats by intention!
		case 18: {
			//the dummy column hack was removed on win
			cfg().DeleteGroup(_T("/GUI/ColumnWidths/"));
		}
		case 19:
		case 20:
		case 21: {
			if (translationhelper) {
				// add locale's language code to autojoin
				if (translationhelper->GetLocale()) {
					wxString localecode = translationhelper->GetLocale()->GetCanonicalName();
					if (localecode.Find(_T("_")) != -1)
						localecode = localecode.BeforeFirst(_T('_'));
					if (localecode == _T("en")) // we'll skip en for now, maybe in the future we'll reactivate it
						AddChannelJoin(localecode, wxEmptyString);
				}
			}
		}
		case 22: {
			ConvertLists();
		}
		case 23:
		case 24: {
			SetDisableSpringVersionCheck(false);
		}
		case 25: { // language id before was stored by index, now its stored by the id
			cfg().Write(_T("/General/LanguageID"), 0);
		}
		case 26: {
			SetDisableSpringVersionCheck(false);
		}
		case 27: {
			RemoveChannelJoin(_("main"));
		}
		case 28:
		case 29:
		case 30: {
#ifdef WIN32 // https://github.com/springlobby/springlobby/issues/385
			cfg().Write(_T("/GUI/UseNotificationPopups"), true);
#endif
		}
		case 31: {
			DeleteServer(_T("Backup server 1"));
			DeleteServer(_T("Backup server 2"));
			SetDefaultServerSettings();
		}

		default: {
		}
	}

	// after updating, set current version
	cfg().Write(_T("/General/SettingsVersion"), SETTINGS_VERSION);
}

//! @brief Saves the settings to file
void Settings::SaveSettings()
{
	cfg().Write(_T( "/General/firstrun" ), false);
	cfg().SaveFile();
}

bool Settings::IsFirstRun()
{
	return cfg().ReadBool(_T( "/General/firstrun" ));
}

bool Settings::GetNoUDP()
{
	return cfg().Read(_T( "/General/NoUDP" ), 0l);
}

void Settings::SetNoUDP(bool value)
{
	cfg().Write(_T( "/General/NoUDP" ), value);
}

int Settings::GetClientPort()
{
	return cfg().Read(_T( "/General/ClientPort" ), 0l);
}

void Settings::SetClientPort(int value)
{
	cfg().Write(_T( "/General/ClientPort" ), value);
}

bool Settings::GetShowIPAddresses()
{
	return cfg().Read(_T( "/General/ShowIP" ), 0l);
}

void Settings::SetShowIPAddresses(bool value)
{
	cfg().Write(_T( "/General/ShowIP" ), value);
}

int Settings::GetHTTPMaxParallelDownloads()
{
	return cfg().Read(_T("/General/ParallelHTTPCount"), 3);
}
void Settings::SetHTTPMaxParallelDownloads(int value)
{
	cfg().Write(_T("/General/ParallelHTTPCount"), value);
}


bool Settings::GetWebBrowserUseDefault()
{
	// See note on ambiguities, in wx/confbase.h near line 180.
	bool useDefault;
	cfg().Read(_T( "/General/WebBrowserUseDefault" ), &useDefault, DEFSETT_WEB_BROWSER_USE_DEFAULT);
	return useDefault;
}

void Settings::SetWebBrowserUseDefault(bool useDefault)
{
	cfg().Write(_T( "/General/WebBrowserUseDefault" ), useDefault);
}

wxString Settings::GetWebBrowserPath()
{
	return cfg().Read(_T( "/General/WebBrowserPath" ), wxEmptyString);
}


void Settings::SetWebBrowserPath(const wxString& path)
{
	cfg().Write(_T( "/General/WebBrowserPath" ), path);
}


bool Settings::ShouldAddDefaultServerSettings()
{
	return !cfg().Exists(_T( "/Server" ));
}

//! @brief Restores default settings
void Settings::SetDefaultServerSettings()
{
	SetServer(DEFSETT_DEFAULT_SERVER_NAME, DEFSETT_DEFAULT_SERVER_HOST, DEFSETT_DEFAULT_SERVER_PORT, "0124dc0f4295b401a2d81ade3dc81b7a467eb9a70b0a4912b5e15fede735fe73");
	SetServer("Test server", "lobby.springrts.com", 7000, "bafee3142009baa105ade65b0f712ca5fcf30de8e91664a8825e0185a609277c");
	SetDefaultServer(DEFSETT_DEFAULT_SERVER_NAME);
}

//! @brief Checks if the server name/alias exists in the settings
bool Settings::ServerExists(const wxString& server_name)
{
	return cfg().Exists(_T( "/Server/Servers/" ) + server_name);
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
wxString Settings::GetDefaultServer()
{
	wxString serv = DEFSETT_DEFAULT_SERVER_NAME;
	return cfg().Read(_T("/Server/Default"), serv);
}


//! @brief Set the name/alias of the default server.
//!
//! @param server_name the server name/alias
//! @see GetDefaultServer()
void Settings::SetDefaultServer(const wxString& server_name)
{
	cfg().Write(_T( "/Server/Default" ), server_name);
}


//! @brief Get hostname of a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerHost(const wxString& server_name)
{
	wxString host = DEFSETT_DEFAULT_SERVER_HOST;
	return cfg().Read(_T("/Server/Servers/") + server_name + _T("/Host"), host);
}


//! @brief Set hostname of a server.
//!
//! @param server_name the server name/alias
//! @param the host url address
//! @param the port where the service is run
void Settings::SetServer(const wxString& server_name, const wxString& url, int port, const wxString& fingerprint)
{
	cfg().Write(_T( "/Server/Servers/" ) + server_name + _T( "/Host" ), url);
	cfg().Write(_T( "/Server/Servers/" ) + server_name + _T( "/Port" ), port);
	if (!fingerprint.empty()) {
		cfg().Write(_T( "/Server/Servers/" ) + server_name + _T( "/Certificate" ), fingerprint);
	}
}

//! @brief Deletes a server from the list.
//!
//! @param server_name the server name/alias
void Settings::DeleteServer(const wxString& server_name)
{
	cfg().DeleteGroup(_T( "/Server/Servers/" ) + server_name);
}


//! @brief Get port number of a server.
//!
//! @param server_name the server name/alias
int Settings::GetServerPort(const wxString& server_name)
{
	return cfg().Read(_T( "/Server/Servers/" ) + server_name + _T( "/Port" ), DEFSETT_DEFAULT_SERVER_PORT);
}

//! @brief Get list of server aliases
wxArrayString Settings::GetServers()
{
	return cfg().GetGroupList(_T( "/Server/Servers/" ));
}


//! @brief Get nickname of the default account for a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerAccountNick(const wxString& server_name)
{
	return cfg().Read(_T( "/Server/Servers/" ) + server_name + _T( "/Nick" ), wxEmptyString);
}


//! @brief Set nickname of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void Settings::SetServerAccountNick(const wxString& server_name, const wxString& value)
{
	cfg().Write(_T( "/Server/Servers/" ) + server_name + _T( "/Nick" ), value);
}


//! @brief Get password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @todo Implement
wxString Settings::GetServerAccountPass(const wxString& server_name)
{
	return cfg().Read(_T( "/Server/Servers/" ) + server_name + _T( "/Pass" ), wxEmptyString);
}


//! @brief Set password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void Settings::SetServerAccountPass(const wxString& server_name, const wxString& value)
{
	cfg().Write(_T( "/Server/Servers/" ) + server_name + _T( "/Pass" ), value);
}


//! @brief Get if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @todo Implement
bool Settings::GetServerAccountSavePass(const wxString& server_name)
{
	return cfg().Read(_T( "/Server/Servers/" ) + server_name + _T( "/savepass" ), (long int)DEFSETT_SAVE_PASSWORD);
}


//! @brief Set if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void Settings::SetServerAccountSavePass(const wxString& server_name, const bool value)
{
	cfg().Write(_T( "/Server/Servers/" ) + server_name + _T( "/savepass" ), (long int)value);
}


void Settings::AddChannelJoin(const wxString& channel, const wxString& key)
{
	int index = cfg().GetGroupCount(_T( "/Channels/AutoJoin" ));

	//Does channel saved already?
	if (GetChannelJoinIndex(channel) != -1)
		return;

	cfg().Write(wxString::Format(_T( "/Channels/AutoJoin/Channel%d/Name" ), index), channel);
	cfg().Write(wxString::Format(_T( "/Channels/AutoJoin/Channel%d/Password" ), index), key);
}


void Settings::RemoveChannelJoin(const wxString& channel)
{
	auto channelsList = GetChannelsJoin();

	RemoveAllChannelsJoin();

	for (auto& channelItem : channelsList) {
		if ((channelItem.name == channel) || (channelItem.name.empty())) {
			continue;
		}
		AddChannelJoin(channelItem.name, channelItem.password);
	}
}

void Settings::RemoveAllChannelsJoin()
{
	cfg().DeleteGroup(_T( "/Channels/AutoJoin" ));
}


int Settings::GetChannelJoinIndex(const wxString& name)
{
	int numchannels = cfg().GetGroupCount(_T( "/Channels/AutoJoin" ));
	int ret = -1;
	for (int i = 0; i < numchannels; i++) {
		if (cfg().Read(wxString::Format(_T( "/Channels/AutoJoin/Channel%d/Name" ), i), wxEmptyString) == name)
			ret = i;
	}
	return ret;
}

std::vector<ChannelJoinInfo> Settings::GetChannelsJoin()
{
	std::vector<ChannelJoinInfo> ret;
	wxArrayString channels = cfg().GetGroupList(_T("/Channels/AutoJoin/"));
	slConfig::PathGuard pathguard(&cfg(), _T("/Channels/AutoJoin/"));
	for (size_t i = 0; i < channels.Count(); ++i) {
		if (!channels[i].StartsWith(_T("Channel")))
			continue;
		ChannelJoinInfo info;
		info.name = cfg().Read(channels[i] + _T("/Name" ));
		info.password = cfg().Read(channels[i] + _T("/Password" ));
		ret.push_back(info);
	}
	return ret;
}

bool Settings::ShouldAddDefaultChannelSettings()
{
	return !cfg().Exists(_T( "/Channels" ));
}

// ===================================================


wxString Settings::GetLastHostDescription()
{
	return cfg().Read(_T( "/Hosting/LastDescription" ), wxEmptyString);
}


wxString Settings::GetLastHostMod()
{
	return cfg().Read(_T( "/Hosting/LastMod" ), wxEmptyString);
}


wxString Settings::GetLastHostPassword()
{
	return cfg().Read(_T( "/Hosting/LastPassword" ), wxEmptyString);
}


int Settings::GetLastHostPort()
{
	return cfg().Read(_T( "/Hosting/LastPort" ), DEFSETT_SPRING_PORT);
}


int Settings::GetLastHostPlayerNum()
{
	return cfg().Read(_T( "/Hosting/LastPlayerNum" ), 4);
}


int Settings::GetLastHostNATSetting()
{
	return cfg().Read(_T( "/Hosting/LastNATSetting" ), (long)0);
}


wxString Settings::GetLastHostMap()
{
	return cfg().Read(_T( "/Hosting/LastMap" ), wxEmptyString);
}

int Settings::GetLastRankLimit()
{
	return cfg().Read(_T( "/Hosting/LastRank" ), 0l);
}

bool Settings::GetTestHostPort()
{
	return cfg().Read(_T( "/Hosting/TestHostPort" ), 0l);
}

bool Settings::GetLastAutolockStatus()
{
	return cfg().Read(_T( "/Hosting/LastAutoLock" ), true);
}

bool Settings::GetLastHostRelayedMode()
{
	return cfg().Read(_T( "/Hosting/LastRelayedMode" ), 0l);
}

wxColour Settings::GetBattleLastColour()
{
	return wxColour(cfg().Read(_T( "/Hosting/MyLastColour" ), _T( "#FFFF00" )));
}


void Settings::SetBattleLastColour(const wxColour& col)
{
	cfg().Write(_T( "/Hosting/MyLastColour" ), col.GetAsString(wxC2S_HTML_SYNTAX));
}

void Settings::SetLastHostDescription(const wxString& value)
{
	cfg().Write(_T( "/Hosting/LastDescription" ), value);
}


void Settings::SetLastHostMod(const wxString& value)
{
	cfg().Write(_T( "/Hosting/LastMod" ), value);
}


void Settings::SetLastHostPassword(const wxString& value)
{
	cfg().Write(_T( "/Hosting/LastPassword" ), value);
}


void Settings::SetLastHostPort(int value)
{
	cfg().Write(_T( "/Hosting/LastPort" ), value);
}


void Settings::SetLastHostPlayerNum(int value)
{
	cfg().Write(_T( "/Hosting/LastPlayerNum" ), value);
}


void Settings::SetLastHostNATSetting(int value)
{
	cfg().Write(_T( "/Hosting/LastNATSetting" ), value);
}


void Settings::SetLastHostMap(const wxString& value)
{
	cfg().Write(_T( "/Hosting/LastMap" ), value);
}

void Settings::SetLastRankLimit(int rank)
{
	cfg().Write(_T( "/Hosting/LastRank" ), rank);
}

void Settings::SetLastAI(const wxString& ai)
{
	cfg().Write(_T( "/SinglePlayer/LastAI" ), ai);
}

void Settings::SetLastBattleId(int battleId)
{
	cfg().Write(_T( "/Hosting/MyLastBattleId" ), battleId);
}

void Settings::SetLastScriptPassword(const wxString& scriptPassword)
{
	cfg().Write(_T( "/Hosting/MyLastScriptPassword" ), scriptPassword);
}

void Settings::SetTestHostPort(bool value)
{
	cfg().Write(_T( "/Hosting/TestHostPort" ), value);
}

void Settings::SetLastAutolockStatus(bool value)
{
	cfg().Write(_T( "/Hosting/LastAutoLock" ), value);
}

void Settings::SetLastHostRelayedMode(bool value)
{
	cfg().Write(_T( "/Hosting/LastRelayedMode" ), value);
}

void Settings::SetHostingPreset(const wxString& name, int optiontype, std::map<wxString, wxString> options)
{
	cfg().DeleteGroup(_T( "/Hosting/Preset/" ) + name + _T( "/" ) + TowxString(optiontype));
	for (std::map<wxString, wxString>::const_iterator it = options.begin(); it != options.end(); ++it) {
		cfg().Write(_T( "/Hosting/Preset/" ) + name + _T( "/" ) + TowxString(optiontype) + _T( "/" ) + it->first, it->second);
	}
}

std::map<wxString, wxString> Settings::GetHostingPreset(const wxString& name, int optiontype)
{
	wxString path_base = _T( "/Hosting/Preset/" ) + name + _T( "/" ) + TowxString(optiontype);
	std::map<wxString, wxString> ret;
	wxArrayString list = cfg().GetEntryList(path_base);

	slConfig::PathGuard pathGuard(&cfg(), path_base);

	int count = list.GetCount();
	for (int i = 0; i < count; i++) {
		wxString keyname = list[i];
		wxString val = cfg().Read(keyname);
		ret[keyname] = val;
	}
	return ret;
}


wxArrayString Settings::GetPresetList()
{
	return cfg().GetGroupList(_T( "/Hosting/Preset" ));
}


void Settings::DeletePreset(const wxString& name)
{
	cfg().DeleteGroup(_T( "/Hosting/Preset/" ) + name);

	//delete mod default preset associated
	wxArrayString list = cfg().GetEntryList(_T( "/Hosting/ModDefaultPreset" ));
	int count = list.GetCount();
	for (int i = 0; i < count; i++) {
		wxString keyname = list[i];
		if (cfg().Read(keyname) == name)
			cfg().DeleteEntry(keyname);
	}
}


wxString Settings::GetModDefaultPresetName(const wxString& gamename)
{
	return cfg().Read(_T( "/Hosting/ModDefaultPreset/" ) + gamename);
}


void Settings::SetModDefaultPresetName(const wxString& gamename, const wxString& presetname)
{
	cfg().Write(_T( "/Hosting/ModDefaultPreset/" ) + gamename, presetname);
}


void Settings::SetBalanceMethod(int value)
{
	cfg().Write(_T( "/Hosting/BalanceMethod" ), value);
}
int Settings::GetBalanceMethod()
{
	return cfg().Read(_T( "/Hosting/BalanceMethod" ), 1l);
}

void Settings::SetBalanceClans(bool value)
{
	cfg().Write(_T( "/Hosting/BalanceClans" ), value);
}
bool Settings::GetBalanceClans()
{
	return cfg().Read(_T( "/Hosting/BalanceClans" ), true);
}

void Settings::SetBalanceStrongClans(bool value)
{
	cfg().Write(_T( "/Hosting/BalanceStrongClans" ), value);
}

bool Settings::GetBalanceStrongClans()
{
	return cfg().Read(_T( "/Hosting/BalanceStrongClans" ), 0l);
}

void Settings::SetBalanceGrouping(int value)
{
	cfg().Write(_T( "/Hosting/BalanceGroupingSize" ), value);
}

int Settings::GetBalanceGrouping()
{
	return cfg().Read(_T( "/Hosting/BalanceGroupingSize" ), 0l);
}


void Settings::SetFixIDMethod(int value)
{
	cfg().Write(_T( "/Hosting/FixIDMethod" ), value);
}
int Settings::GetFixIDMethod()
{
	return cfg().Read(_T( "/Hosting/FixIDMethod" ), 1l);
}

void Settings::SetFixIDClans(bool value)
{
	cfg().Write(_T( "/Hosting/FixIDClans" ), value);
}
bool Settings::GetFixIDClans()
{
	return cfg().Read(_T( "/Hosting/FixIDClans" ), true);
}

void Settings::SetFixIDStrongClans(bool value)
{
	cfg().Write(_T( "/Hosting/FixIDStrongClans" ), value);
}

bool Settings::GetFixIDStrongClans()
{
	return cfg().Read(_T( "/Hosting/FixIDStrongClans" ), 0l);
}

void Settings::SetFixIDGrouping(int value)
{
	cfg().Write(_T( "/Hosting/FixIDGroupingSize" ), value);
}

int Settings::GetFixIDGrouping()
{
	return cfg().Read(_T( "/Hosting/FixIDGroupingSize" ), 0l);
}


wxString Settings::GetLastAI()
{
	return cfg().Read(_T( "/SinglePlayer/LastAI" ), wxEmptyString);
}

int Settings::GetLastBattleId()
{
	return cfg().Read(_T( "/Hosting/MyLastBattleId" ), -1);
}

wxString Settings::GetLastScriptPassword()
{
	return cfg().Read(_T( "/Hosting/MyLastScriptPassword" ), wxEmptyString);
}

void Settings::SetChatHistoryLenght(int historylines)
{
	cfg().Write(_T( "/Chat/HistoryLinesLenght" ), historylines);
}


int Settings::GetChatHistoryLenght()
{
	return cfg().Read(_T( "/Chat/HistoryLinesLenght" ), 1000l);
}


void Settings::SetChatPMSoundNotificationEnabled(bool enabled)
{
	cfg().Write(_T( "/Chat/PMSound" ), enabled);
}


bool Settings::GetChatPMSoundNotificationEnabled()
{
	return cfg().Read(_T( "/Chat/PMSound" ), 1l);
}

/*
wxColour ConvertOldRGBFormat( wxString color )
{
	long R = 0, G = 0, B = 0;
	color.BeforeFirst( _T( ' ' ) ).ToLong( &R );
	color = color.AfterFirst( _T( ' ' ) );
	color.BeforeFirst( _T( ' ' ) ).ToLong( &G );
	color = color.AfterFirst( _T( ' ' ) );
	color.BeforeFirst( _T( ' ' ) ).ToLong( &B );
	return wxColour( R % 256, G % 256, B % 256 );
}
*/

wxColour Settings::GetChatColorNormal()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Normal" ), _T( "#000000" )));
}

void Settings::SetChatColorNormal(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Normal" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}


wxColour Settings::GetChatColorBackground()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Background" ), _T( "#FFFFFF" )));
}

void Settings::SetChatColorBackground(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Background" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorHighlight()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Highlight" ), _T( "#FF0000" )));
}

void Settings::SetChatColorHighlight(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Highlight" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorMine()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Mine" ), _T( "#8A8A8A" )));
}

void Settings::SetChatColorMine(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Mine" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorNotification()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Notification" ), _T( "#FF2828" )));
}

void Settings::SetChatColorNotification(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Notification" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorAction()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Action" ), _T( "#E600FF" )));
}

void Settings::SetChatColorAction(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Action" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorServer()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Server" ), _T( "#005080" )));
}

void Settings::SetChatColorServer(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Server" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorClient()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Client" ), _T( "#14C819" )));
}

void Settings::SetChatColorClient(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Client" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorJoinPart()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/JoinPart" ), _T( "#42CC42" )));
}

void Settings::SetChatColorJoinPart(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/JoinPart" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorError()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Error" ), _T( "#800000" )));
}

void Settings::SetChatColorError(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Error" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxColour Settings::GetChatColorTime()
{
	return wxColour(cfg().Read(_T( "/Chat/Colour/Time" ), _T( "#64648C" )));
}

void Settings::SetChatColorTime(wxColour value)
{
	cfg().Write(_T( "/Chat/Colour/Time" ), value.GetAsString(wxC2S_CSS_SYNTAX));
}

wxFont Settings::GetChatFont()
{
	wxString info = cfg().Read(_T( "/Chat/Font" ), wxEmptyString);
	if (info != wxEmptyString) {
		wxFont f(info);
		if (f.IsOk()) {
			return f;
		}
	}
	return wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
}

void Settings::SetChatFont(wxFont value)
{
	cfg().Write(_T( "/Chat/Font" ), value.GetNativeFontInfoDesc());
}


bool Settings::GetSmartScrollEnabled()
{
	return cfg().Read(_T( "/Chat/SmartScrollEnabled" ), true);
}

void Settings::SetSmartScrollEnabled(bool value)
{
	cfg().Write(_T( "/Chat/SmartScrollEnabled" ), value);
}

bool Settings::GetAlwaysAutoScrollOnFocusLost()
{
	return cfg().Read(_T( "/Chat/AlwaysAutoScrollOnFocusLost" ), true);
}

void Settings::SetAlwaysAutoScrollOnFocusLost(bool value)
{
	cfg().Write(_T( "/Chat/AlwaysAutoScrollOnFocusLost" ), value);
}

void Settings::SetUseIrcColors(bool value)
{
	cfg().Write(_T( "/Chat/UseIrcColors" ), value);
}

bool Settings::GetUseIrcColors()
{
	return cfg().Read(_T( "/Chat/UseIrcColors" ), true);
}

void Settings::setFromList(const wxArrayString& list, const wxString& path)
{
	wxString string;
	for (unsigned int i = 0; i < list.GetCount(); i++)
		string << list[i] << _T( ";" );
	cfg().Write(path, string);
}

wxArrayString Settings::getFromList(const wxString& path)
{
	return wxStringTokenize(cfg().Read(path, wxString()), _T(";"));
}

void Settings::SetHighlightedWords(const wxArrayString& words)
{
	setFromList(words, _T("/Chat/HighlightedWords"));
}

wxArrayString Settings::GetHighlightedWords()
{
	return getFromList(_T("/Chat/HighlightedWords"));
}

void Settings::ConvertLists()
{
	const wxArrayString current_hl = cfg().GetEntryList(_T( "/Chat/HighlightedWords" ));
	cfg().DeleteGroup(_T( "/Chat/HighlightedWords" ));
	SaveSettings();
	SetHighlightedWords(current_hl);
	SaveSettings();
}

void Settings::SetRequestAttOnHighlight(const bool req)
{
	cfg().Write(_T( "/Chat/ReqAttOnHighlight" ), req);
}

bool Settings::GetRequestAttOnHighlight()
{
	return cfg().Read(_T( "/Chat/ReqAttOnHighlight" ), 0l);
}


bool Settings::GetBattleLastAutoStartState()
{
	return cfg().Read(_T( "/Hosting/AutoStart" ), 0l);
}

void Settings::SetBattleLastAutoStartState(bool value)
{
	cfg().Write(_T( "/Hosting/AutoStart" ), value);
}

bool Settings::GetBattleLastAutoControlState()
{
	return cfg().Read(_T( "/Hosting/AutoControl" ), 0l);
}

void Settings::SetBattleLastAutoControlState(bool value)
{
	cfg().Write(_T( "/Hosting/AutoControl" ), value);
}

int Settings::GetBattleLastAutoSpectTime()
{
	return cfg().Read(_T( "/Hosting/AutoSpectTime" ), 0l);
}

void Settings::SetBattleLastAutoSpectTime(int value)
{
	cfg().Write(_T( "/Hosting/AutoSpectTime" ), value);
}

void Settings::SetBattleLastSideSel(const wxString& gamename, int sidenum)
{
	cfg().Write(_T("/Battle/Sides/" + gamename), sidenum);
}

int Settings::GetBattleLastSideSel(const wxString& gamename)
{
	if (gamename.IsEmpty())
		return 0;
	return cfg().Read(_T("/Battle/Sides/" + gamename), 0l);
}

void Settings::SetMapLastStartPosType(const wxString& mapname, const wxString& startpostype)
{
	cfg().Write(_T( "/Hosting/MapLastValues/" ) + mapname + _T( "/startpostype" ), startpostype);
}

void Settings::SetMapLastRectPreset(const wxString& mapname, std::vector<Settings::SettStartBox> rects)
{
	wxString basepath = _T( "/Hosting/MapLastValues/" ) + mapname + _T( "/Rects" );
	cfg().DeleteGroup(basepath);
	for (std::vector<Settings::SettStartBox>::const_iterator itor = rects.begin(); itor != rects.end(); ++itor) {
		SettStartBox box = *itor;
		wxString additionalpath = basepath + _T( "/Rect" ) + TowxString(box.ally) + _T( "/" );
		cfg().Write(additionalpath + _T( "TopLeftX" ), box.topx);
		cfg().Write(additionalpath + _T( "TopLeftY" ), box.topy);
		cfg().Write(additionalpath + _T( "BottomRightX" ), box.bottomx);
		cfg().Write(additionalpath + _T( "BottomRightY" ), box.bottomy);
		cfg().Write(additionalpath + _T( "AllyTeam" ), box.ally);
	}
}

wxString Settings::GetMapLastStartPosType(const wxString& mapname)
{
	return cfg().Read(_T( "/Hosting/MapLastValues/" ) + mapname + _T( "/startpostype" ), wxEmptyString);
}

std::vector<Settings::SettStartBox> Settings::GetMapLastRectPreset(const wxString& mapname)
{
	wxString basepath = _T( "/Hosting/MapLastValues/" ) + mapname + _T( "/Rects" );
	wxArrayString boxes = cfg().GetGroupList(basepath);
	std::vector<Settings::SettStartBox> ret;
	for (unsigned int i = 0; i < boxes.GetCount(); i++) {
		wxString additionalpath = basepath + _T( "/" ) + boxes[i] + _T( "/" );
		SettStartBox box;
		box.topx = cfg().Read(additionalpath + _T( "TopLeftX" ), -1);
		box.topy = cfg().Read(additionalpath + _T( "TopLeftY" ), -1);
		box.bottomx = cfg().Read(additionalpath + _T( "BottomRightX" ), -1);
		box.bottomy = cfg().Read(additionalpath + _T( "BottomRightY" ), -1);
		box.ally = cfg().Read(additionalpath + _T( "AllyTeam" ), -1);
		ret.push_back(box);
	}
	return ret;
}

bool Settings::GetDisableSpringVersionCheck()
{
	bool ret;
	cfg().Read(_T( "/Spring/DisableVersionCheck" ), &ret, false);
	return ret;
}

void Settings::SetDisableSpringVersionCheck(bool disable)
{
	cfg().Write(_T( "/Spring/DisableVersionCheck" ), (bool)disable);
}


void Settings::SetShowTooltips(bool show)
{
	cfg().Write(_T( "/GUI/ShowTooltips" ), show);
}

bool Settings::GetShowTooltips()
{
	return cfg().Read(_T( "/GUI/ShowTooltips" ), 1l);
}

void Settings::RemoveLayouts()
{
	cfg().DeleteEntry(_T("/GUI/DefaultLayout"));
	cfg().DeleteGroup(_T("/Layout"));
	cfg().DeleteGroup(_T("/GUI/AUI"));
}

void Settings::SetColumnWidth(const wxString& list_name, const int column_ind, const int column_width)
{
	cfg().Write(_T( "GUI/ColumnWidths/" ) + list_name + _T( "/" ) + TowxString(column_ind), column_width);
}

void Settings::SetColumnHidden(const wxString& list_name, const int column_ind, const bool columnIsHidden)
{
	cfg().Write(_T( "GUI/ColumnVisibilities/" ) + list_name + _T( "/" ) + TowxString(column_ind), columnIsHidden);
}

int Settings::GetColumnWidth(const wxString& list_name, const int column)
{
	const int orgwidth = cfg().Read(_T( "/GUI/ColumnWidths/" ) + list_name + _T( "/" ) + TowxString(column), columnWidthUnset);
	int width = orgwidth;
	if (orgwidth > -1) //-3 is unset, -2 and -1 used for auto size by wx
		width = std::max(width, Settings::columnWidthMinimum);
	return width;
}

bool Settings::IsColumnHidden(const wxString& list_name, const int column, bool defVal)
{
	bool isHidden;
	cfg().Read(wxString::Format(_T("/GUI/ColumnVisibilities/%s/%d"), list_name, column),
		   &isHidden, defVal);
	return isHidden;
}

void Settings::SaveCustomColors(const wxColourData& _cdata, const wxString& paletteName)
{
	//note 16 colors is wx limit
	wxColourData cdata = _cdata;
	for (int i = 0; i < 16; ++i) {
		wxColour col = cdata.GetCustomColour(i);
		if (!col.IsOk())
			col = wxColour(255, 255, 255);
		cfg().Write(_T( "/CustomColors/" ) + paletteName + _T( "/" ) + TowxString(i), col.GetAsString(wxC2S_HTML_SYNTAX));
	}
}

wxColourData Settings::GetCustomColors(const wxString& paletteName)
{
	wxColourData cdata;
	//note 16 colors is wx limit
	for (int i = 0; i < 16; ++i) {
		wxColour col(cfg().Read(_T( "/CustomColors/" ) + paletteName + _T( "/" ) + TowxString(i), wxColour(255, 255, 255).GetAsString(wxC2S_HTML_SYNTAX)));
		cdata.SetCustomColour(i, col);
	}

	return cdata;
}

PlaybackListFilterValues Settings::GetReplayFilterValues(const wxString& profile_name)
{
	PlaybackListFilterValues filtervalues;
	filtervalues.duration = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/duration" ), wxEmptyString);
	filtervalues.map = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/map" ), wxEmptyString);
	filtervalues.map_show = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/map_show" ), 0L);
	filtervalues.filesize = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/filesize" ), wxEmptyString);
	filtervalues.filesize_mode = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/filesize_mode" ), _T( ">" ));
	filtervalues.duration_mode = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/duration_mode" ), _T( ">" ));
	filtervalues.mod = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/mod" ), wxEmptyString);
	filtervalues.mod_show = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/mod_show" ), 0L);
	filtervalues.player_mode = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/player_mode" ), _T( "=" ));
	filtervalues.player_num = cfg().Read(_T( "/ReplayFilter/" ) + profile_name + _T( "/player_num" ), _T( "All" ));

	return filtervalues;
}

void Settings::SetReplayFilterValues(const PlaybackListFilterValues& filtervalues, const wxString& profile_name)
{
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/duration" ), filtervalues.duration);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/map" ), filtervalues.map);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/map_show" ), filtervalues.map_show);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/filesize" ), filtervalues.filesize);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/filesize_mode" ), filtervalues.filesize_mode);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/duration_mode" ), filtervalues.duration_mode);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/mod" ), filtervalues.mod);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/mod_show" ), filtervalues.mod_show);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/player_mode" ), filtervalues.player_mode);
	cfg().Write(_T( "/ReplayFilter/" ) + profile_name + _T( "/player_num" ), filtervalues.player_num);
	cfg().Write(_T( "/ReplayFilter/lastprofile" ), profile_name);
}

/*
bool Settings::GetReplayFilterActivState() const
{
	return cfg().Read( _T( "/ReplayFilter/Active" ) , 0l );
}

*/

void Settings::SetReplayFilterActivState(const bool state)
{
	cfg().Write(_T( "/ReplayFilter/Active" ), state);
}

wxString Settings::GetLastReplayFilterProfileName()
{
	return cfg().Read(_T( "/ReplayFilter/lastprofile" ), _T( "default" ));
}
wxString Settings::GetLastRelayedHost()
{
	return cfg().Read(_T("/General/RelayHost"), wxEmptyString);
}
void Settings::SetLastRelayedHost(wxString relhost)
{
	cfg().Write(_T("/General/RelayHost"), relhost);
}
void Settings::SetCompletionMethod(CompletionMethod method)
{
	cfg().Write(_T( "/General/CompletionMethod" ), (int)method);
}
Settings::CompletionMethod Settings::GetCompletionMethod() const
{
	return (CompletionMethod)cfg().Read(_T( "/General/CompletionMethod" ), (int)MatchExact);
}


unsigned int Settings::GetHorizontalSortkeyIndex()
{
	return cfg().Read(_T( "/GUI/MapSelector/HorizontalSortkeyIndex" ), 0l);
}

void Settings::SetHorizontalSortkeyIndex(const unsigned int idx)
{
	cfg().Write(_T( "/GUI/MapSelector/HorizontalSortkeyIndex" ), (int)idx);
}

unsigned int Settings::GetVerticalSortkeyIndex()
{
	return cfg().Read(_T( "/GUI/MapSelector/VerticalSortkeyIndex" ), 0l);
}

void Settings::SetVerticalSortkeyIndex(const unsigned int idx)
{
	cfg().Write(_T( "/GUI/MapSelector/VerticalSortkeyIndex" ), (int)idx);
}

bool Settings::GetHorizontalSortorder()
{
	return cfg().Read(_T( "/GUI/MapSelector/HorizontalSortorder" ), 0l);
}

void Settings::SetHorizontalSortorder(const bool order)
{
	cfg().Write(_T( "/GUI/MapSelector/HorizontalSortorder" ), order);
}

bool Settings::GetVerticalSortorder()
{
	return cfg().Read(_T( "/GUI/MapSelector/VerticalSortorder" ), 0l);
}

void Settings::SetVerticalSortorder(const bool order)
{
	cfg().Write(_T( "/GUI/MapSelector/VerticalSortorder" ), order);
}

void Settings::SetMapSelectorFollowsMouse(bool value)
{
	cfg().Write(_T( "/GUI/MapSelector/SelectionFollowsMouse" ), value);
}

bool Settings::GetMapSelectorFollowsMouse()
{
	return cfg().Read(_T( "/GUI/MapSelector/SelectionFollowsMouse" ), 0l);
}

unsigned int Settings::GetMapSelectorFilterRadio()
{
	int val = 0;
	cfg().Read(_T( "/GUI/MapSelector/FilterRadio" ), &val);
	return val;
}

void Settings::SetMapSelectorFilterRadio(const unsigned int val)
{
	cfg().Write(_T( "/GUI/MapSelector/FilterRadio" ), (int)val);
}

//////////////////////////////////////////////////////////////////////////////
///                            SpringSettings                              ///
//////////////////////////////////////////////////////////////////////////////


long Settings::getMode()
{
	return cfg().Read(_T( "/SpringSettings/mode" ), SET_MODE_SIMPLE);
}

void Settings::setMode(long mode)
{
	cfg().Write(_T( "/SpringSettings/mode" ), mode);
}

bool Settings::getDisableWarning()
{
	return cfg().Read(_T( "/SpringSettings/disableWarning" ), 0l);
}

void Settings::setDisableWarning(bool disable)
{
	cfg().Write(_T( "/SpringSettings/disableWarning" ), disable);
}


wxString Settings::getSimpleRes()
{
	wxString def = vl_Resolution_Str[1];
	cfg().Read(_T( "/SpringSettings/SimpleRes" ), &def);
	return def;
}
void Settings::setSimpleRes(wxString res)
{
	cfg().Write(_T( "/SpringSettings/SimpleRes" ), res);
}

wxString Settings::getSimpleQuality()
{
	wxString def = wxT("medium");
	cfg().Read(_T( "/SpringSettings/SimpleQuality" ), &def);
	return def;
}

void Settings::setSimpleQuality(wxString qual)
{
	cfg().Write(_T( "/SpringSettings/SimpleQuality" ), qual);
}

wxString Settings::getSimpleDetail()
{
	wxString def = wxT("medium");
	cfg().Read(_T( "/SpringSettings/SimpleDetail" ), &def);
	return def;
}

void Settings::setSimpleDetail(wxString det)
{
	cfg().Write(_T( "/SpringSettings/SimpleDetail" ), det);
}


SortOrder Settings::GetSortOrder(const wxString& list_name)
{
	SortOrder order;
	slConfig::PathGuard pathGuard(&cfg(), _T( "/UI/SortOrder/" ) + list_name + _T( "/" ));
	unsigned int entries = cfg().GetNumberOfGroups(false); //do not recurse
	for (unsigned int i = 0; i < entries; i++) {
		SortOrderItem it;
		it.direction = cfg().Read(TowxString(i) + _T( "/dir" ), 1);
		it.col = cfg().Read(TowxString(i) + _T( "/col" ), i);
		order[i] = it;
	}
	return order;
}

void Settings::SetSortOrder(const wxString& list_name, const SortOrder& order)
{
	SortOrder::const_iterator it = order.begin();
	for (; it != order.end(); ++it) {
		cfg().Write(_T( "/UI/SortOrder/" ) + list_name + _T( "/" ) + TowxString(it->first) + _T( "/dir" ), it->second.direction);
		cfg().Write(_T( "/UI/SortOrder/" ) + list_name + _T( "/" ) + TowxString(it->first) + _T( "/col" ), it->second.col);
	}
}

int Settings::GetSashPosition(const wxString& window_name)
{
	return cfg().Read(_T( "/GUI/SashPostion/" ) + window_name, 200l);
}

void Settings::SetSashPosition(const wxString& window_name, const int pos)
{
	cfg().Write(_T( "/GUI/SashPostion/" ) + window_name, pos);
}

bool Settings::GetSplitBRoomHorizontally()
{
	return cfg().Read(_T( "/GUI/SplitBRoomHorizontally" ), 1l);
}

void Settings::SetSplitBRoomHorizontally(const bool vertical)
{
	cfg().Write(_T( "/GUI/SplitBRoomHorizontally" ), vertical);
}

bool Settings::GetShowXallTabs()
{
	return cfg().Read(_T( "/GUI/CloseOnAll" ), 0l);
}

void Settings::SetShowXallTabs(bool show)
{
	cfg().Write(_T( "/GUI/CloseOnAll" ), show);
}

void Settings::SavePerspective(const wxString& notebook_name, const wxString& perspective_name, const wxString& layout_string)
{
	wxString entry = wxString::Format(_T( "/GUI/AUI/%s/%s" ), perspective_name.c_str(), notebook_name.c_str());
	cfg().Write(entry, layout_string);
}

wxString Settings::LoadPerspective(const wxString& notebook_name, const wxString& perspective_name)
{
	wxString entry = wxString::Format(_T( "/GUI/AUI/%s/%s" ), perspective_name.c_str(), notebook_name.c_str());
	return cfg().Read(entry, wxEmptyString);
}

wxString Settings::GetLastPerspectiveName()
{
	return cfg().Read(_T( "/GUI/AUI/lastperspective_name" ), _T("default"));
}

void Settings::SetLastPerspectiveName(const wxString& name)
{
	cfg().Write(_T( "/GUI/AUI/lastperspective_name" ), name);
}

void Settings::SetAutosavePerspective(bool autosave)
{
	cfg().Write(_T( "/GUI/AUI/autosave" ), autosave);
}

bool Settings::GetAutosavePerspective()
{
	return cfg().Read(_T( "/GUI/AUI/autosave" ), 1l);
}

wxArrayString Settings::GetPerspectives()
{
	wxArrayString list = cfg().GetGroupList(_T( "/GUI/AUI" ));
	wxArrayString ret;
	for (size_t i = 0; i < list.GetCount(); ++i) {
		if (!list[i].EndsWith(BattlePostfix))
			ret.Add(list[i]);
		else {
			wxString stripped = list[i].Left(list[i].Len() - BattlePostfix.Len());
			if (!PerspectiveExists(stripped))
				ret.Add(stripped);
		}
	}
	return ret;
}

bool Settings::PerspectiveExists(const wxString& perspective_name)
{
	wxArrayString list = cfg().GetGroupList(_T( "/GUI/AUI" ));
	for (size_t i = 0; i < list.GetCount(); ++i) {
		if (list[i] == perspective_name)
			return true;
	}
	return false;
}

void Settings::SetAutoloadedChatlogLinesCount(const int count)
{
	cfg().Write(_T( "/GUI/AutoloadedChatlogLinesCount" ), std::abs(count));
}

int Settings::GetAutoloadedChatlogLinesCount()
{
	return cfg().Read(_T( "/GUI/AutoloadedChatlogLinesCount" ), 10l);
}

void Settings::SetUseNotificationPopups(const bool use)
{
	cfg().Write(_T("/GUI/UseNotificationPopups"), use);
}

bool Settings::GetUseNotificationPopups()
{
	return cfg().Read(_T("/GUI/UseNotificationPopups"), true);
}

void Settings::SetNotificationPopupPosition(const size_t index)
{
	cfg().Write(_T("/GUI/NotificationPopupPosition"), (long)index);
}

size_t Settings::GetNotificationPopupPosition()
{
	return cfg().Read(_T("/GUI/NotificationPopupPosition"), (long)ScreenPosition::bottom_right);
}


void Settings::SetNotificationPopupDisplayTime(const unsigned int seconds)
{
	cfg().Write(_T("/GUI/NotificationPopupDisplayTime"), (long)seconds);
}

unsigned int Settings::GetNotificationPopupDisplayTime()
{
	return cfg().Read(_T("/GUI/NotificationPopupDisplayTime"), 5l);
}

std::string Settings::GetServerFingerprint(const std::string& server_name)
{
	return STD_STRING(cfg().Read(_T( "/Server/Servers/" ) + TowxString(server_name) + _T( "/Certificate" ), ""));
}

void Settings::SetServerFingerprint(const std::string& server_name, const std::string& fingerprint)
{
	cfg().Write(_T( "/Server/Servers/" ) + TowxString(server_name) + _T( "/Certificate" ), TowxString(fingerprint));
}
