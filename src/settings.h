#ifndef SPRINGLOBBY_HEADERGUARD_SETTINGS_H
#define SPRINGLOBBY_HEADERGUARD_SETTINGS_H

#include <wx/string.h>

const int CACHE_VERSION     = 7;
const int SETTINGS_VERSION  = 3;

const wxString DEFSETT_DEFAULT_SERVER = _T("TAS Server");
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

//doing this "properly" would mean dragging in stdpaths header, doesn't seem warranted (koshi)
#define DEFSETT_SPRING_DIR  wxGetCwd()

/** Default value for config path /General/WebBrowserUseDefault.
 */
const bool DEFSETT_WEB_BROWSER_USE_DEFAULT = true;

#include <wx/fileconf.h>
#include "utils.h"
#include "useractions.h"


class wxConfigBase;
class wxFont;
struct BattleListFilterValues;
struct ReplayListFilterValues;
class wxFileInputStream;
class wxFileName;
class wxColor;
class wxColour;
struct wxColourData;

class SL_WinConf : public wxFileConfig
{
    public:
    SL_WinConf (const wxString& appName, const wxString& vendorName,
                           const wxString& strLocal, const wxString& strGlobal,
                           long style,
                           const wxMBConv& conv)
            : wxFileConfig(appName, vendorName,
                           strLocal, strGlobal,
                           style)

    {

    }

    SL_WinConf(wxFileInputStream& in);

//    int Read(const wxString& key, int def)
//    {
//      return s2l(wxFileConfig::Read(key, TowxString<long>(def)));
//    }
//
//    bool Write(const wxString& key, const int lval)
//    {
//        return wxFileConfig::Write(key, TowxString<int>(lval) );
//    }

    protected:

//    bool DoReadLong(const wxString& key, long *pl) const
//    {
//        wxFileConfig::DoReadString(key,
//    }

    bool DoWriteLong(const wxString& key, long lValue)
    {
        return wxFileConfig::DoWriteString(key, TowxString<long>( lValue ) );
    }
};


//! @brief Class used to store and restore application settings.
class Settings
{
  public:

  /** Default constructor.
   */
    Settings();
    ~Settings();

    bool IsPortableMode();
    void SetPortableMode( bool mode );

    /** Initialize all settings to default.
     */
    void SetDefaultSettings();
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

    int GetClientPort();
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
    void SetWebBrowserPath( const wxString path );

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
    wxString GetDefaultServer();
    void SetDefaultServer( const wxString& server_name );
    void SetAutoConnect( bool do_autoconnect );
    bool GetAutoConnect( );

    bool ServerExists( const wxString& server_name );

    wxString GetServerHost( const wxString& server_name );
    void SetServerHost( const wxString& server_name, const wxString& value );

    int GetServerPort( const wxString& server_name );
    void SetServerPort( const wxString& server_name, const int value );

    int GetNumServers();
    void SetNumServers( int num );
    void AddServer( const wxString& server_name );
    int GetServerIndex( const wxString& server_name );

    wxString GetServerName( int index );
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


    /** Set the number of channels currently in the autojoin list.  This
     * function is not intended for direct use, and will probably go away soon.
     *
     * @internal
     *
     * @param num The new maximum number of channels we think are in the
     * autojoin list.
     */
    void SetNumChannelsJoin( int num );


    /** Add a channel to the autojoin list.
     */
    void AddChannelJoin( const wxString& channel , const wxString& key );

    /** Remove a channel from the autojoin list.
     *
     * @param channel The name of the channel to remove
     */
    void RemoveChannelJoin( const wxString& channel );


    /** Determine the index of a channel name in the autojoin list.
     *
     * @param channel A channel name
     *
     * @returns The channel's autojoin list index, or @c -1 if it was not found.
     */
    int GetChannelJoinIndex( const wxString& channel );


    /** Fetch the name corresponding to the given index in the autojoin list.
     *
     * @param index A channel index
     *
     * @returns The name corresponding to @c index, or an empty string if it was
     * not found.
     */
    wxString GetChannelJoinName( int index );
    /**@}*/


    /* ================================================================ */
    /** @name UI
     * @{
     */

     void SaveCustomColors( const wxColourData& cdata, const wxString& paletteName = _T("Default") );
     wxColourData GetCustomColors( const wxString& paletteName = _T("Default") );

    int    GetMainWindowWidth();
    void   SetMainWindowWidth( const int value );

    int    GetMainWindowHeight();
    void   SetMainWindowHeight( const int value );

    int    GetMainWindowTop();
    void   SetMainWindowTop( const int value );

    int    GetMainWindowLeft();
    void   SetMainWindowLeft( const int value );

    bool UseOldSpringLaunchMethod();
    void SetOldSpringLaunchMethod( bool value );

    void SetShowTooltips( bool show);
    bool GetShowTooltips();

    void SetColumnWidth( const wxString& list_name, const int coloumn_ind, const int coloumn_width );
    int GetColumnWidth( const wxString& list_name, const int coloumn );
    //! used to signal unset column width in Get...
    enum { columnWidthUnset };

    /*@}*/

    /* ================================================================ */
    /** @name People/Group management
     * @{
     */
    void SetPeopleList( const wxArrayString& friends, const wxString& group = _T("default") );
    wxArrayString GetPeopleList( const wxString& group = _T("default") ) const;

    wxArrayString GetGroups( ) const;
    void AddGroup( const wxString& group ) ;
    void DeleteGroup( const wxString& group ) ;

    void SetGroupHLColor( const wxColor& color, const wxString& group = _T("default") );
    wxColor GetGroupHLColor( const wxString& group = _T("default") ) const;

    void SetGroupActions( const wxString& group, UserActions::ActionType action );
    UserActions::ActionType GetGroupActions( const wxString& group ) const;

    /*@}*/

    /* ================================================================ */
    /** @name Spring locations
     * @{
     */

    void ConvertOldSpringDirsOptions();

    std::map<wxString, wxString> GetSpringVersionList(); /// index -> version
    wxString GetCurrentUsedSpringIndex();
    void SetUsedSpringIndex( const wxString& index );
    void DeleteSpringVersionbyIndex( const wxString& index );

    /// convenience wrappers to get current used version paths
    wxString GetCurrentUsedDataDir();
    wxString GetCurrentUsedUnitSync();
    wxString GetCurrentUsedSpringBinary();

    wxString GetUnitSync( const wxString& index );
    wxString GetSpringBinary( const wxString& index );

    void SetUnitSync( const wxString& index, const wxString& path );
    void SetSpringBinary( const wxString& index, const wxString& path );

    wxString AutoFindSpringBin();
    wxString AutoFindUnitSync();

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
    void SetChatHistoryLenght( unsigned int historylines );
    unsigned int GetChatHistoryLenght();

    void SetChatPMSoundNotificationEnabled( bool enabled );
    bool GetChatPMSoundNotificationEnabled();

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

    //!@brief expects words to be a ; seperated list
    void SetHighlightedWords( const wxString& words );
    wxString GetHighlightedWords( );

    //!\brief controls if user attention is requested when highlighting a line
    void SetRequestAttOnHighlight( const bool req );
    bool GetRequestAttOnHighlight( );
    /**@}*/

    /* Do these go in Chat? */
    bool GetSmartScrollEnabled();
    void SetSmartScrollEnabled(bool value);
    bool GetAlwaysAutoScrollOnFocusLost();
    void SetAlwaysAutoScrollOnFocusLost(bool value);


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



    /** @name Battle filters
     * @{
     */
    BattleListFilterValues GetBattleFilterValues(const wxString& profile_name = (_T("default")));
    void SetBattleFilterValues(const BattleListFilterValues& blfValues, const wxString& profile_name = _T("default"));
    wxString GetLastBattleFilterProfileName();
    void SetBattleFilterActivState( const bool state );
    bool GetBattleFilterActivState( ) const;
    /**@}*/

    /** @name Replay filters
     * @{
     */
    ReplayListFilterValues GetReplayFilterValues(const wxString& profile_name = (_T("default")));
    void SetReplayFilterValues(const ReplayListFilterValues& blfValues, const wxString& profile_name = _T("default"));
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

    int    GetSettingsWindowWidth();
    void   SetSettingsWindowWidth( const int value );

    int    GetSettingsWindowHeight();
    void   SetSettingsWindowHeight( const int value );

    int    GetSettingsWindowTop();
    void   SetSettingsWindowTop( const int value );

    int    GetSettingsWindowLeft();
    void   SetSettingsWindowLeft( const int value );
  /**@}*/

  protected:
    bool IsSpringBin( const wxString& path );

    #ifdef __WXMSW__
    SL_WinConf* m_config; //!< wxConfig object to store and restore  all settings in.
    #else
    wxConfigBase* m_config; //!< wxConfig object to store and restore  all settings in.
    #endif

    wxString m_chosed_path;
    bool m_portable_mode;

};

Settings& sett();

#endif // SPRINGLOBBY_HEADERGUARD_SETTINGS_H
