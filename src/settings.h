#ifndef SPRINGLOBBY_HEADERGUARD_SETTINGS_H
#define SPRINGLOBBY_HEADERGUARD_SETTINGS_H

#include <wx/string.h>
#include <vector>

const int CACHE_VERSION     = 10;
const int SETTINGS_VERSION  = 15;

const wxString DEFSETT_DEFAULT_SERVER_NAME= _T("Official server");
const wxString DEFSETT_DEFAULT_SERVER_HOST = _T("taspringmaster.clan-sy.com");
const int DEFSETT_DEFAULT_SERVER_PORT = 8200;
const bool DEFSETT_SAVE_PASSWORD = false;
const unsigned int DEFSETT_MW_WIDTH = 880;
const unsigned int DEFSETT_MW_HEIGHT = 600;
const unsigned int DEFSETT_MW_TOP = 50;
const unsigned int DEFSETT_MW_LEFT = 50;
const unsigned int DEFSETT_SPRING_PORT = 8452;

const unsigned int SET_MODE_EXPERT = 5000;
const unsigned int SET_MODE_SIMPLE = 5001;
const unsigned int DEFSETT_SW_WIDTH = 770;
const unsigned int DEFSETT_SW_HEIGHT = 580;
const unsigned int DEFSETT_SW_TOP = 50;
const unsigned int DEFSETT_SW_LEFT = 50;

const unsigned int SPRING_MAX_USERS = 64;
const unsigned int SPRING_MAX_TEAMS = 16;
const unsigned int SPRING_MAX_ALLIES = 16;

/** Default value for config path /General/WebBrowserUseDefault.
 */
const bool DEFSETT_WEB_BROWSER_USE_DEFAULT = true;

#include <wx/fileconf.h>
#include "useractions.h"
#include "Helper/sortutil.h"

class wxWindow;
class wxConfigBase;
class wxFileConfig;
class wxFont;
struct BattleListFilterValues;
struct PlaybackListFilterValues;
class wxFileInputStream;
class wxFileName;
class wxColour;
class wxColour;
class wxColourData;
class wxSize;
class wxPoint;
class wxPathList;

typedef std::map<unsigned int,unsigned int> ColumnMap;

struct ChannelJoinInfo
{
	wxString name;
	wxString password;
};

class SL_WinConf : public wxFileConfig
{
    public:
			SL_WinConf ( const wxString& appName, const wxString& vendorName, const wxString& strLocal, const wxString& strGlobal, long style, const wxMBConv& /*conv*/):
			wxFileConfig( appName, vendorName, strLocal, strGlobal, style)
			{
			}

			SL_WinConf( wxFileInputStream& in );
    protected:
			bool DoWriteLong(const wxString& key, long lValue);
};


//! @brief Class used to store and restore application settings.
class Settings
{
  public:

  /** Default constructor.
   */
    Settings();
    ~Settings();

    /** used for passing config file at command line
    */
    static bool m_user_defined_config;
    static wxString m_user_defined_config_path;

		/// used to import default configs from a file in windows
		#ifdef __WXMSW__
    void SetDefaultConfigs( SL_WinConf& conf );
    #else
    void SetDefaultConfigs( wxConfig& conf );
    #endif

    /// list all entries subkeys of a parent group
    wxArrayString GetGroupList( const wxString& base_key );
    /// list all groups subkeys of a parent group
    wxArrayString GetEntryList( const wxString& base_key );
    /// counts all groups subkeys of a parent group
		unsigned int GetGroupCount( const wxString& base_key );

    bool IsPortableMode();
    void SetPortableMode( bool mode );

    /** Initialize all settings to default.
     */
    void SetDefaultServerSettings();
    void SaveSettings();

    bool IsFirstRun();

    //! Sets/Gets settings revision number
    void SetSettingsVersion();
    unsigned int GetSettingsVersion();

    //! should we sayex/pm bot?
    void SetReportStats(const bool value);
    bool GetReportStats();

    void SetAutoUpdate( const bool value );
    bool GetAutoUpdate();

    wxString GetLobbyWriteDir();

    wxString GetTempStorage();

    /* ================================================================ */
    /** @name Network
     * @{
     */
    bool GetNoUDP();
    void SetNoUDP(bool value);

    int GetClientPort();/// use zero to pick port automatically, nonzero to override. This allows to play if you have broken router, by setting SourcePort to some forwarded port.
    void SetClientPort(int value);

    bool GetShowIPAddresses();
    void SetShowIPAddresses(bool value);
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
    void SetWebBrowserPath( const wxString& path );

    /**@}*/

    /* ================================================================ */
    /** @name Cache
     *
     * Information about the directory tree used to cache infomation about maps
     * and mods.
     *
     * @{
     */
    wxString GetCachePath();

    void SetCacheVersion();
    int GetCacheVersion();

    void SetMapCachingThreadProgress( unsigned int index );
    unsigned int GetMapCachingThreadProgress();

    void SetModCachingThreadProgress( unsigned int index );
    unsigned int GetModCachingThreadProgress();

    /**@}*/

    /* ================================================================ */
    /** @name Servers
     * @{
     */
		void ConvertOldServerSettings();
    wxString GetDefaultServer();
    void SetDefaultServer( const wxString& server_name );
    void SetAutoConnect( bool do_autoconnect );
    bool GetAutoConnect( );

    wxString GetServerHost( const wxString& server_name );
    int GetServerPort( const wxString& server_name );

    wxArrayString GetServers();
    bool ServerExists( const wxString& server_name );
    void SetServer( const wxString& server_name, const wxString& url, int port );
    void DeleteServer( const wxString& server_name );

    bool ShouldAddDefaultServerSettings();
    /**@}*/

    /* ================================================================ */
    /** @name Accounts
     * @{
     */
    wxString GetServerAccountNick( const wxString& server_name );
    void   SetServerAccountNick( const wxString& server_name, const wxString& value );

    wxString GetServerAccountPass( const wxString& server_name );
    void   SetServerAccountPass( const wxString& server_name, const wxString& value );

    bool   GetServerAccountSavePass( const wxString& server_name );
    void   SetServerAccountSavePass( const wxString& server_name, const bool value );
    /**@}*/

    /* ================================================================ */
    /** @name Automatic channel join list
     *
     * These functions are used to manipulate the list of channels that
     * SpringLobby should join upon connecting to a server.
     *
     * @{
     */

    /** Fetch the number of channels in the autojoin list.
     */
    int GetNumChannelsJoin();


    /** Add a channel to the autojoin list.
     */
    void AddChannelJoin( const wxString& channel , const wxString& key );

    /** Remove a channel from the autojoin list.
     *
     * @param channel The name of the channel to remove
     */
    void RemoveChannelJoin( const wxString& channel );


    /** Returns the list of channels to autojoin
     *
     * @returns std::vector of ChannelJoinInfo struct, don't break ordering index!
     */
    std::vector<ChannelJoinInfo> GetChannelsJoin();


    /** Deletes all autojoined channels
     *
     */
    void RemoveAllChannelsJoin();

    /** Returns the join order of a channel
     *
     * @returns the order of the channel during autojoin or -1 if not found
     */
		int GetChannelJoinIndex( const wxString& name );

    void ConvertOldChannelSettings();

    bool ShouldAddDefaultChannelSettings();
		/**@}*/

    /* ================================================================ */
    /** @name UI
     * @{
     */

     void SetStartTab( const int idx );
     unsigned int GetStartTab( );

     void SaveCustomColors( const wxColourData& cdata, const wxString& paletteName = _T("Default") );
     wxColourData GetCustomColors( const wxString& paletteName = _T("Default") );

    int    GetWindowWidth( const wxString& window );
    void   SetWindowWidth( const wxString& window, const int value );

    int    GetWindowHeight( const wxString& window );
    void   SetWindowHeight( const wxString& window, const int value );

    int    GetWindowTop( const wxString& window );
    void   SetWindowTop( const wxString& window, const int value );

    int    GetWindowLeft( const wxString& window );
    void   SetWindowLeft( const wxString& window, const int value );

    wxSize  GetWindowSize( const wxString& window, const wxSize& def );
    void    SetWindowSize( const wxString& window, const wxSize& size  );

    wxPoint  GetWindowPos( const wxString& window, const wxPoint& def );
    void    SetWindowPos( const wxString& window, const wxPoint& pos );

    bool UseOldSpringLaunchMethod();
    void SetOldSpringLaunchMethod( bool value );

    void SetShowTooltips( bool show);
    bool GetShowTooltips();

    ColumnMap GetColumnMap( const wxString& name );
    void GetColumnMap( const wxString& name, const ColumnMap& map );

    SortOrder GetSortOrder( const wxString& list_name );
    void SetSortOrder( const wxString& list_name, const SortOrder& order  );

    void SetColumnWidth( const wxString& list_name, const int column_ind, const int column_width );
    int GetColumnWidth( const wxString& list_name, const int column );
    //! used to signal unset column width in Get...
    static const int columnWidthUnset = -3;
    static const int columnWidthMinimum = 5;

    void SetLanguageID ( const long id );
    long GetLanguageID ( );

    int GetSashPosition( const wxString& window_name );
    void SetSashPosition( const wxString& window_name, const int pos );

    bool GetSplitBRoomHorizontally();
    void SetSplitBRoomHorizontally( const bool vertical );

    void TranslateSavedColumWidths();

    wxString GetEditorPath( );
    void SetEditorPath( const wxString& path );
    /*@}*/

    /* ================================================================ */
    /** @name People/Group management
     * @{
     */
    void SetPeopleList( const wxArrayString& friends, const wxString& group = _T("default") );
    wxArrayString GetPeopleList( const wxString& group = _T("default") ) const;

    wxArrayString GetGroups( );
    void AddGroup( const wxString& group ) ;
    void DeleteGroup( const wxString& group ) ;

    void SetGroupHLColor( const wxColour& color, const wxString& group = _T("default") );
    wxColour GetGroupHLColor( const wxString& group = _T("default") ) const;

    void SetGroupActions( const wxString& group, UserActions::ActionType action );
    UserActions::ActionType GetGroupActions( const wxString& group ) const;

		bool ShouldAddDefaultGroupSettings();

    /*@}*/

    /* ================================================================ */
    /** @name Spring locations
     * @{
     */

		wxPathList GetAdditionalSearchPaths( wxPathList& pl );

    void ConvertOldSpringDirsOptions();

		void RefreshSpringVersionList();
    std::map<wxString, wxString> GetSpringVersionList(); /// index -> version
    wxString GetCurrentUsedSpringIndex();
    void SetUsedSpringIndex( const wxString& index );
    void DeleteSpringVersionbyIndex( const wxString& index );

    /// when this mode is enabled in windows SL will search for spring files only in the current executable folder
    void SetSearchSpringOnlyInSLPath( bool value );
    bool GetSearchSpringOnlyInSLPath();

    /// convenience wrappers to get current used version paths
    wxString GetCurrentUsedDataDir();
    wxString GetCurrentUsedUnitSync();
    wxString GetCurrentUsedSpringBinary();
    //!@brief returns config file path unitsync uses, returns empty if unitsync isn't loaded
    wxString GetCurrentUsedSpringConfigFilePath();

    wxString GetUnitSync( const wxString& index );
    wxString GetSpringBinary( const wxString& index );

    void SetUnitSync( const wxString& index, const wxString& path );
    void SetSpringBinary( const wxString& index, const wxString& path );

    wxString AutoFindSpringBin();
    wxString AutoFindUnitSync();

    //!@brief returns config file path spring should use, returns empty for default
    wxString GetForcedSpringConfigFilePath();

    /*@}*/

    /* ================================================================ */
    /** @name Chat
     * @{
     */
    bool GetChatLogEnable();
    void SetChatLogEnable( const bool value );
    wxString GetChatLogLoc();
    void   SetChatLogLoc( const wxString& loc );

    //!@brief sets how many lines can stay in a chat panel before the old will start getting erased, 0 to disable
    void SetChatHistoryLenght( int historylines );
    int GetChatHistoryLenght();

    void SetChatPMSoundNotificationEnabled( bool enabled );
    bool GetChatPMSoundNotificationEnabled();

    void ConvertOldColorSettings();

    wxColour GetChatColorNormal();
    void SetChatColorNormal( wxColour value );
    wxColour GetChatColorBackground();
    void SetChatColorBackground( wxColour value );
    wxColour GetChatColorHighlight();
    void SetChatColorHighlight( wxColour value );
    wxColour GetChatColorMine();
    void SetChatColorMine( wxColour value );
    wxColour GetChatColorNotification();
    void SetChatColorNotification( wxColour value );
    wxColour GetChatColorAction();
    void SetChatColorAction( wxColour value );
    wxColour GetChatColorServer();
    void SetChatColorServer( wxColour value );
    wxColour GetChatColorClient();
    void SetChatColorClient( wxColour value );
    wxColour GetChatColorJoinPart();
    void SetChatColorJoinPart( wxColour value );
    wxColour GetChatColorError();
    void SetChatColorError( wxColour value );
    wxColour GetChatColorTime();
    void SetChatColorTime( wxColour value );
    wxFont GetChatFont();
    void SetChatFont( wxFont value );

    void SetDisplayJoinLeave( bool display, const wxString& channel  );
    bool GetDisplayJoinLeave( const wxString& channel );

    void SetHighlightedWords( const wxArrayString& words );
    wxArrayString GetHighlightedWords( );

    //!\brief controls if user attention is requested when highlighting a line
    void SetRequestAttOnHighlight( const bool req );
    bool GetRequestAttOnHighlight( );
    /**@}*/

    /* Do these go in Chat? */
    bool GetSmartScrollEnabled();
    void SetSmartScrollEnabled(bool value);
    bool GetAlwaysAutoScrollOnFocusLost();
    void SetAlwaysAutoScrollOnFocusLost(bool value);

		void ConvertOldHiglightSettings();

		void SetUseIrcColors( bool value );
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
    int GetLastRankLimit();
    bool GetTestHostPort();
    bool GetLastAutolockStatus();
    bool GetLastHostRelayedMode();

    void SetLastHostDescription( const wxString& value );
    void SetLastHostMod( const wxString& value );
    void SetLastHostPassword( const wxString& value );
    void SetLastHostPort( int value );
    void SetLastHostPlayerNum( int value );
    void SetLastHostNATSetting( int value );
    void SetLastHostMap( const wxString& value );
    void SetLastRankLimit( int rank );
    void SetTestHostPort( bool value );
    void SetLastAutolockStatus( bool value );
    void SetLastHostRelayedMode( bool value );

    void SetHostingPreset( const wxString& name, int optiontype, std::map<wxString,wxString> options );
    std::map<wxString,wxString> GetHostingPreset( const wxString& name, int optiontype );
    wxArrayString GetPresetList();
    void DeletePreset( const wxString& name );

    wxString GetModDefaultPresetName( const wxString& modname );
    void SetModDefaultPresetName( const wxString& modname, const wxString& presetname );

    /**@}*/


    /* ================================================================ */
    wxColour GetBattleLastColour();
    void SetBattleLastColour( const wxColour& col );


    void SetLastAI( const wxString& ai );
    wxString GetLastAI();

    void SetBalanceMethod(int value);
    int GetBalanceMethod();

    void SetBalanceClans(bool value);
    bool GetBalanceClans();

    void SetBalanceStrongClans(bool value);
    bool GetBalanceStrongClans();

    void SetBalanceGrouping( int value );
    int GetBalanceGrouping();

    void SetFixIDMethod(int value);
    int GetFixIDMethod();

    void SetFixIDClans(bool value);
    bool GetFixIDClans();

    void SetFixIDStrongClans(bool value);
    bool GetFixIDStrongClans();

    void SetFixIDGrouping( int value );
    int GetFixIDGrouping();

    /** @name Battle filters
     * @{
     */
    BattleListFilterValues GetBattleFilterValues(const wxString& profile_name = (_T("default")));
    void SetBattleFilterValues(const BattleListFilterValues& blfValues, const wxString& profile_name = _T("default"));
    wxString GetLastBattleFilterProfileName();
    void SetBattleFilterActivState( const bool state );
    bool GetBattleFilterActivState( ) const;

    bool GetBattleLastAutoStartState();
    void SetBattleLastAutoStartState( bool value );

    bool GetBattleLastAutoControlState();
    void SetBattleLastAutoControlState( bool value );

		int GetBattleLastAutoSpectTime();
    void SetBattleLastAutoSpectTime( int value );

    bool GetBattleLastAutoAnnounceDescription();
    void SetBattleLastAutoAnnounceDescription( bool value );

    struct SettStartBox
    {
    	int ally;
    	int topx;
    	int topy;
    	int bottomx;
    	int bottomy;
    };

    void SetMapLastStartPosType( const wxString& mapname, const wxString& startpostype );
		void SetMapLastRectPreset( const wxString& mapname, std::vector<Settings::SettStartBox> rects );

		wxString GetMapLastStartPosType( const wxString& mapname );
		std::vector<Settings::SettStartBox> GetMapLastRectPreset( const wxString& mapname );
    /**@}*/

    /** @name Replay filters
     * @{
     */
    PlaybackListFilterValues GetReplayFilterValues(const wxString& profile_name = (_T("default")));
    void SetReplayFilterValues(const PlaybackListFilterValues& blfValues, const wxString& profile_name = _T("default"));
    wxString GetLastReplayFilterProfileName();
    void SetReplayFilterActivState( const bool state );
    bool GetReplayFilterActivState( ) const;
    /**@}*/

    bool GetDisableSpringVersionCheck();

    /* ================================================================ */
    /** @name Torrent System
     * @{
     */
    unsigned int GetTorrentPort();
    void SetTorrentPort( unsigned int port );
    int GetTorrentUploadRate();
    void SetTorrentUploadRate( int speed );
    int GetTorrentDownloadRate();
    void SetTorrentDownloadRate( int speed );

    int GetTorrentSystemSuspendMode();
    void SetTorrentSystemSuspendMode( int mode );
    int GetTorrentThrottledUploadRate();
    void SetTorrentThrottledUploadRate( int speed );
    int GetTorrentThrottledDownloadRate();
    void SetTorrentThrottledDownloadRate( int speed );

    int GetTorrentSystemAutoStartMode();
    void SetTorrentSystemAutoStartMode( int mode );

    void SetTorrentMaxConnections( int connections );
    int GetTorrentMaxConnections();

    void SetTorrentListToResume( const wxArrayString& list );
    wxArrayString GetTorrentListToResume();

    /** Get the path to the directory where *.torrent files are stored.
     */
    wxFileName GetTorrentDir();


    /** Get the path to the directory where partially-downloaded
     * torrented files are stored.
     *
     * @sa GetTorrentsFolder
     */
    wxFileName GetTorrentDataDir();

    /**@}*/

    /** @name Aui
     *
     * Functions used to store and retrieve the current SpringLobby
     * interface layout.
     *
     * @{
     */
    void SaveLayout( wxString& layout_name, wxString& layout_string );
    wxString GetLayout( wxString& layout_name );
    wxArrayString GetLayoutList();
    void SetDefaultLayout( const wxString& layout_name );
    wxString GetDefaultLayout();
    //! icons for mainwindow tabs??
    bool GetUseTabIcons();
    void SetUseTabIcons( bool use );
    /**@}*/

    enum CompletionMethod {
        MatchNearest = 1,
        MatchExact = 2
    };

    void SetCompletionMethod( CompletionMethod method );
    CompletionMethod GetCompletionMethod(  ) const;

    /** @name SpringSettings
     * @{
     */
    int getMode();
    void setMode( int );
    bool getDisableWarning();
    void setDisableWarning( bool );
    wxString getSimpleRes();
    void setSimpleRes( wxString );
    wxString getSimpleQuality();
    void setSimpleQuality( wxString );
    wxString getSimpleDetail();
    void setSimpleDetail( wxString );

  /**@}*/

  /* ================================================================ */
    /** @name Map selection dialog
     * @{
     */
    unsigned int GetVerticalSortkeyIndex(  );
    void SetVerticalSortkeyIndex( const unsigned int idx );

    unsigned int GetHorizontalSortkeyIndex(  );
    void SetHorizontalSortkeyIndex( const unsigned int idx );

    /** \return true for "<" false for ">" */
    bool GetHorizontalSortorder();
    void SetHorizontalSortorder( const bool order );

    /** \return true for "ᴧ", false for "ᴠ" */
    bool GetVerticalSortorder();
    void SetVerticalSortorder( const bool order );

    void SetMapSelectorFollowsMouse( bool value );
    bool GetMapSelectorFollowsMouse();

    /** \return m_filter_all_sett = 0; (default)
                m_filter_recent_sett = 1;
                m_filter_popular_sett = 2; */
    unsigned int GetMapSelectorFilterRadio();
    void SetMapSelectorFilterRadio( const unsigned int val );
    /**@}*/


  protected:
    bool IsSpringBin( const wxString& path );

    #ifdef __WXMSW__
    SL_WinConf* m_config; //!< wxConfig object to store and restore  all settings in.
    #elif defined(__WXMAC__)
    wxFileConfig* m_config; //!< wxConfig object to store and restore  all settings in.
    #else
    wxConfigBase* m_config; //!< wxConfig object to store and restore  all settings in.
    #endif

    wxString m_chosed_path;
    bool m_portable_mode;

    std::map<wxString, wxString> m_spring_versions;

    Settings( const Settings& );

};

Settings& sett();

#endif // SPRINGLOBBY_HEADERGUARD_SETTINGS_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

