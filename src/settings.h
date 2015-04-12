/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SETTINGS_H
#define SPRINGLOBBY_HEADERGUARD_SETTINGS_H

#include <wx/string.h>
#include <wx/intl.h>

#include <vector>
#include <set>
#include "utils/mixins.h"
#include "useractions.h"
#include "utils/sortutil.h"

const long CACHE_VERSION = 14;
const long SETTINGS_VERSION = 31;


const wxString DEFSETT_DEFAULT_SERVER_NAME = _T("Official server");
const wxString DEFSETT_DEFAULT_SERVER_HOST = _T("lobby.springrts.com");
const wxString BattlePostfix = _T("_battle");
const int DEFSETT_DEFAULT_SERVER_PORT = 8200;
const bool DEFSETT_SAVE_PASSWORD = false;
const unsigned int DEFSETT_SPRING_PORT = 8452;

const long SET_MODE_EXPERT = 5000;
const long SET_MODE_SIMPLE = 5001;
const unsigned int DEFSETT_SW_WIDTH = 1055;
const unsigned int DEFSETT_SW_HEIGHT = 670;
const unsigned int DEFSETT_SW_TOP = 50;
const unsigned int DEFSETT_SW_LEFT = 50;

const unsigned int SPRING_MAX_USERS = 64;
const unsigned int SPRING_MAX_TEAMS = 16;
const unsigned int SPRING_MAX_ALLIES = 16;

/** Default value for config path /General/WebBrowserUseDefault.
 */
const bool DEFSETT_WEB_BROWSER_USE_DEFAULT = true;

class wxWindow;
class wxConfigBase;
class wxFileConfig;
class wxFont;
struct PlaybackListFilterValues;
class wxFileInputStream;
class wxFileName;
class wxColour;
class wxColour;
class wxColourData;
class wxSize;
class wxPoint;
class wxPathList;
class wxTranslationHelper;

typedef std::map<unsigned int, unsigned int> ColumnMap;

struct ChannelJoinInfo
{
	wxString name;
	wxString password;
};

namespace LSL
{
class SpringBundle;
};

//!convenience class for saving and displaying a choice of discrete screen pos
class ScreenPosition : public wxArrayString
{
public:
	ScreenPosition()
	{
		Add(_("Bottom right"));
		Add(_("Bottom left"));
		Add(_("Top right"));
		Add(_("Top left"));
	}
	static const size_t bottom_right = 0;
	static const size_t bottom_left = 1;
	static const size_t top_right = 2;
	static const size_t top_left = 3;
};

//! @brief Class used to store and restore application settings.
class Settings : public SL::NonCopyable
{
public:
	/** Default constructor.
   */
	Settings();
	~Settings();

	void Setup(wxTranslationHelper* translationhelper);
	void ConvertSettings(wxTranslationHelper* translationhelper, long settversion);

	/** Initialize all settings to default.
     */
	void SetDefaultServerSettings();
	void SaveSettings();

	bool IsFirstRun();


	/* ================================================================ */
	/** @name Network
     * @{
     */
	bool GetNoUDP();
	void SetNoUDP(bool value);

	int GetClientPort(); /// use zero to pick port automatically, nonzero to override. This allows to play if you have broken router, by setting SourcePort to some forwarded port.
	void SetClientPort(int value);

	bool GetShowIPAddresses();
	void SetShowIPAddresses(bool value);

	int GetHTTPMaxParallelDownloads();
	void SetHTTPMaxParallelDownloads(int value);
	/**@}*/

	/* ================================================================ */
	/** @name Web Browser
     *
     * Web browser preferences.
     *
     *@{
     */

	/** Fetch the "Use Default" setting for the web browser.
     *
     * @returns the current setting.
     */
	bool GetWebBrowserUseDefault();


	/** Set the "Use Default" setting for the web browser.  If @c true, we use
     * wxLaunchDefaultBrowser when we want to open a web browser.
     *
     * @param useDefault Whether or not to use the system-default browser.
     *
     * @sa Ui::OpenWebBrowser
     */
	void SetWebBrowserUseDefault(bool useDefault);

	/** Get the path to the user-configured browser.
     *
     * @returns The user-defined browser path, if set, otherwise an empty string
     * (wxEmptyString).
     */
	wxString GetWebBrowserPath();

	/** Set the path to the user-configured browser.
     *
     * @param path A path to a web browser
     */
	void SetWebBrowserPath(const wxString& path);

	/**@}*/


	/* ================================================================ */
	/** @name Servers
     * @{
     */
	wxString GetDefaultServer();
	void SetDefaultServer(const wxString& server_name);

	wxString GetServerHost(const wxString& server_name);
	int GetServerPort(const wxString& server_name);

	wxArrayString GetServers();
	bool ServerExists(const wxString& server_name);
	void SetServer(const wxString& server_name, const wxString& url, int port);
	void DeleteServer(const wxString& server_name);

	bool ShouldAddDefaultServerSettings();
	/**@}*/

	/* ================================================================ */
	/** @name Accounts
     * @{
     */
	wxString GetServerAccountNick(const wxString& server_name);
	void SetServerAccountNick(const wxString& server_name, const wxString& value);

	wxString GetServerAccountPass(const wxString& server_name);
	void SetServerAccountPass(const wxString& server_name, const wxString& value);

	bool GetServerAccountSavePass(const wxString& server_name);
	void SetServerAccountSavePass(const wxString& server_name, const bool value);
	/**@}*/

	/* ================================================================ */
	/** @name Automatic channel join list
     *
     * These functions are used to manipulate the list of channels that
     * SpringLobby should join upon connecting to a server.
     *
     * @{
     */


	/** Add a channel to the autojoin list.
     */
	void AddChannelJoin(const wxString& channel, const wxString& key);

	/** Remove a channel from the autojoin list.
     *
     * @param channel The name of the channel to remove
     */
	void RemoveChannelJoin(const wxString& channel);


	/** Returns the list of channels to autojoin
     *
     * @returns std::vector of ChannelJoinInfo struct, don't break ordering index!
     */
	std::vector<ChannelJoinInfo> GetChannelsJoin();

	/** Deletes all autojoined channels
     *
     */
	void RemoveAllChannelsJoin();

	bool ShouldAddDefaultChannelSettings();
	/**@}*/

	/* ================================================================ */
	/** @name UI
     * @{
     */

	void SaveCustomColors(const wxColourData& cdata, const wxString& paletteName = _T("Default"));
	wxColourData GetCustomColors(const wxString& paletteName = _T("Default"));


	void SetShowTooltips(bool show);
	bool GetShowTooltips();

	ColumnMap GetColumnMap(const wxString& name);
	void GetColumnMap(const wxString& name, const ColumnMap& map);

	SortOrder GetSortOrder(const wxString& list_name);
	void SetSortOrder(const wxString& list_name, const SortOrder& order);

	void SetColumnWidth(const wxString& list_name, const int column_ind, const int column_width);
	int GetColumnWidth(const wxString& list_name, const int column);
	//! used to signal unset column width in Get...
	static const int columnWidthUnset = -3;
	static const int columnWidthMinimum = 5;

	int GetSashPosition(const wxString& window_name);
	void SetSashPosition(const wxString& window_name, const int pos);

	bool GetSplitBRoomHorizontally();
	void SetSplitBRoomHorizontally(const bool vertical);

	bool GetShowXallTabs();
	void SetShowXallTabs(bool show);


	void SetAutoloadedChatlogLinesCount(const int count);
	int GetAutoloadedChatlogLinesCount();

	void SetUseNotificationPopups(const bool use);
	bool GetUseNotificationPopups();
	void SetNotificationPopupPosition(const size_t index);
	size_t GetNotificationPopupPosition();
	void SetNotificationPopupDisplayTime(const unsigned int seconds);
	unsigned int GetNotificationPopupDisplayTime();
	/*@}*/


	/* ================================================================ */
	/** @name Chat
     * @{
     */

	//!@brief sets how many lines can stay in a chat panel before the old will start getting erased, 0 to disable
	void SetChatHistoryLenght(int historylines);
	int GetChatHistoryLenght();

	void SetChatPMSoundNotificationEnabled(bool enabled);
	bool GetChatPMSoundNotificationEnabled();

	wxColour GetChatColorNormal();
	void SetChatColorNormal(wxColour value);
	wxColour GetChatColorBackground();
	void SetChatColorBackground(wxColour value);
	wxColour GetChatColorHighlight();
	void SetChatColorHighlight(wxColour value);
	wxColour GetChatColorMine();
	void SetChatColorMine(wxColour value);
	wxColour GetChatColorNotification();
	void SetChatColorNotification(wxColour value);
	wxColour GetChatColorAction();
	void SetChatColorAction(wxColour value);
	wxColour GetChatColorServer();
	void SetChatColorServer(wxColour value);
	wxColour GetChatColorClient();
	void SetChatColorClient(wxColour value);
	wxColour GetChatColorJoinPart();
	void SetChatColorJoinPart(wxColour value);
	wxColour GetChatColorError();
	void SetChatColorError(wxColour value);
	wxColour GetChatColorTime();
	void SetChatColorTime(wxColour value);
	wxFont GetChatFont();
	void SetChatFont(wxFont value);

	void SetHighlightedWords(const wxArrayString& words);
	wxArrayString GetHighlightedWords();

	//!\brief controls if user attention is requested when highlighting a line
	void SetRequestAttOnHighlight(const bool req);
	bool GetRequestAttOnHighlight();
	/**@}*/

	/* Do these go in Chat? */
	bool GetSmartScrollEnabled();
	void SetSmartScrollEnabled(bool value);
	bool GetAlwaysAutoScrollOnFocusLost();
	void SetAlwaysAutoScrollOnFocusLost(bool value);

	void SetUseIrcColors(bool value);
	bool GetUseIrcColors();

	/* ================================================================ */
	/** @name Hosting
     *
     * %Settings to use when hosting games.  Includes "sticky" settings from the
     * last time a game was hosted.
     *
     * @{
     */
	wxString GetLastHostDescription();
	wxString GetLastHostMod();
	wxString GetLastHostPassword();
	int GetLastHostPort();
	int GetLastHostPlayerNum();
	int GetLastHostNATSetting();
	wxString GetLastHostMap();
	wxString GetDefaultNick();
	int GetLastRankLimit();
	bool GetTestHostPort();
	bool GetLastAutolockStatus();
	bool GetLastHostRelayedMode();

	void SetLastHostDescription(const wxString& value);
	void SetLastHostMod(const wxString& value);
	void SetLastHostPassword(const wxString& value);
	void SetLastHostPort(int value);
	void SetLastHostPlayerNum(int value);
	void SetLastHostNATSetting(int value);
	void SetLastHostMap(const wxString& value);
	void SetLastRankLimit(int rank);
	void SetTestHostPort(bool value);
	void SetLastAutolockStatus(bool value);
	void SetLastHostRelayedMode(bool value);

	void SetHostingPreset(const wxString& name, int optiontype, std::map<wxString, wxString> options);
	std::map<wxString, wxString> GetHostingPreset(const wxString& name, int optiontype);
	wxArrayString GetPresetList();
	void DeletePreset(const wxString& name);

	wxString GetModDefaultPresetName(const wxString& modname);
	void SetModDefaultPresetName(const wxString& modname, const wxString& presetname);

	/**@}*/


	/* ================================================================ */
	wxColour GetBattleLastColour();
	void SetBattleLastColour(const wxColour& col);

	void SetLastBattleId(int battleId);
	int GetLastBattleId();

	void SetLastScriptPassword(const wxString& scriptPassword);
	wxString GetLastScriptPassword();

	void SetLastAI(const wxString& ai);
	wxString GetLastAI();

	void SetBalanceMethod(int value);
	int GetBalanceMethod();

	void SetBalanceClans(bool value);
	bool GetBalanceClans();

	void SetBalanceStrongClans(bool value);
	bool GetBalanceStrongClans();

	void SetBalanceGrouping(int value);
	int GetBalanceGrouping();

	void SetFixIDMethod(int value);
	int GetFixIDMethod();

	void SetFixIDClans(bool value);
	bool GetFixIDClans();

	void SetFixIDStrongClans(bool value);
	bool GetFixIDStrongClans();

	void SetFixIDGrouping(int value);
	int GetFixIDGrouping();

	/** @name Battle filters
     * @{
     */

	bool GetBattleLastAutoStartState();
	void SetBattleLastAutoStartState(bool value);

	bool GetBattleLastAutoControlState();
	void SetBattleLastAutoControlState(bool value);

	int GetBattleLastAutoSpectTime();
	void SetBattleLastAutoSpectTime(int value);

	bool GetBattleLastAutoAnnounceDescription();
	void SetBattleLastAutoAnnounceDescription(bool value);

	void SetBattleLastSideSel(const wxString& modname, int sidenum);
	int GetBattleLastSideSel(const wxString& modname);

	struct SettStartBox
	{
		int ally;
		int topx;
		int topy;
		int bottomx;
		int bottomy;
	};

	void SetMapLastStartPosType(const wxString& mapname, const wxString& startpostype);
	void SetMapLastRectPreset(const wxString& mapname, std::vector<Settings::SettStartBox> rects);

	wxString GetMapLastStartPosType(const wxString& mapname);
	std::vector<Settings::SettStartBox> GetMapLastRectPreset(const wxString& mapname);
	/**@}*/

	/** @name Replay filters
     * @{
     */
	PlaybackListFilterValues GetReplayFilterValues(const wxString& profile_name = (_T("default")));
	void SetReplayFilterValues(const PlaybackListFilterValues& blfValues, const wxString& profile_name = _T("default"));
	wxString GetLastReplayFilterProfileName();
	void SetReplayFilterActivState(const bool state);
	bool GetReplayFilterActivState() const;
	/**@}*/

	bool GetDisableSpringVersionCheck();
	void SetDisableSpringVersionCheck(bool disable);

	/** @name Aui
     *
     * Functions used to store and retrieve the current SpringLobby
     * interface layout.
     *
     * @{
     */
	void SaveLayout(wxString& layout_name, wxString& layout_string);
	wxString GetLayout(wxString& layout_name);

	void SavePerspective(const wxString& notebook_name, const wxString& perspective_name, const wxString& layout_string);
	wxString LoadPerspective(const wxString& notebook_name, const wxString& perspective_name);
	wxString GetLastPerspectiveName();
	void SetLastPerspectiveName(const wxString& name);
	void SetAutosavePerspective(bool autosave);
	bool GetAutosavePerspective();
	wxArrayString GetPerspectives();
	bool PerspectiveExists(const wxString& perspective_name);

	void RemoveLayouts();

	/**@}*/

	enum CompletionMethod {
		MatchNearest = 1,
		MatchExact = 2
	};

	void SetCompletionMethod(CompletionMethod method);
	CompletionMethod GetCompletionMethod() const;

	/** @name SpringSettings
     * @{
     */
	long getMode();
	void setMode(long);
	bool getDisableWarning();
	void setDisableWarning(bool);
	wxString getSimpleRes();
	void setSimpleRes(wxString);
	wxString getSimpleQuality();
	void setSimpleQuality(wxString);
	wxString getSimpleDetail();
	void setSimpleDetail(wxString);

	/**@}*/

	/* ================================================================ */
	/** @name Map selection dialog
     * @{
     */
	unsigned int GetVerticalSortkeyIndex();
	void SetVerticalSortkeyIndex(const unsigned int idx);

	unsigned int GetHorizontalSortkeyIndex();
	void SetHorizontalSortkeyIndex(const unsigned int idx);

	/** \return true for "<" false for ">" */
	bool GetHorizontalSortorder();
	void SetHorizontalSortorder(const bool order);

	/** \return true for "ᴧ", false for "ᴠ" */
	bool GetVerticalSortorder();
	void SetVerticalSortorder(const bool order);

	void SetMapSelectorFollowsMouse(bool value);
	bool GetMapSelectorFollowsMouse();

	/** \return m_filter_all_sett = 0; (default)
                m_filter_recent_sett = 1;
                m_filter_popular_sett = 2; */
	unsigned int GetMapSelectorFilterRadio();
	void SetMapSelectorFilterRadio(const unsigned int val);
	/**@}*/
	/* ============================================================== */
	/** @name Relay Hosts
    * @{
    */

	wxString GetLastRelayedHost(void);
	void SetLastRelayedHost(wxString relhost);

	/**@}*/

	//! move weirdly saved lists to sane form
	void ConvertLists();

private:
	/** Returns the join order of a channel
     *
     * @returns the order of the channel during autojoin or -1 if not found
     */
	/** Fetch the number of channels in the autojoin list.
     */
	int GetNumChannelsJoin();

	int GetChannelJoinIndex(const wxString& name);
	void setFromList(const wxArrayString& list, const wxString& path);
	wxArrayString getFromList(const wxString& path);
};

Settings& sett();

#endif // SPRINGLOBBY_HEADERGUARD_SETTINGS_H
