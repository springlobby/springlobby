#ifndef SPRINGLOBBY_HEADERGUARD_SETTINGS_H
#define SPRINGLOBBY_HEADERGUARD_SETTINGS_H

#include <wx/string.h>
#include <wx/colour.h>

#define CACHE_VERSION 3

#define DEFSETT_DEFAULT_SERVER "TAS Server"
#define DEFSETT_DEFAULT_SERVER_HOST "taspringmaster.clan-sy.com"
#define DEFSETT_DEFAULT_SERVER_PORT 8200
#define DEFSETT_SAVE_PASSWORD false
#define DEFSETT_MW_WIDTH 880
#define DEFSETT_MW_HEIGHT 600
#define DEFSETT_MW_TOP 50
#define DEFSETT_MW_LEFT 50
#define DEFSETT_SPRING_DIR wxGetCwd()
#define DEFSETT_SPRING_PORT 8452

/** Default value for config path /General/WebBrowserUseDefault.
 */
#define DEFSETT_WEB_BROWSER_USE_DEFAULT true

#include <wx/fileconf.h>
#include "utils.h"
#include <wx/wfstream.h>

class wxConfigBase;
class wxFont;
struct BattleListFilterValues;
class IBattle;
class wxFileInputStream ;
//class wxFileConfig;
class myconf : public wxFileConfig
{
    public:
    myconf (const wxString& appName, const wxString& vendorName,
                           const wxString& strLocal, const wxString& strGlobal,
                           long style,
                           const wxMBConv& conv)
            : wxFileConfig(appName, vendorName,
                           strLocal, strGlobal,
                           style)

    {

    }

    myconf(wxFileInputStream& in) : wxFileConfig(in) {}

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

    /** Initialize all settings to default.
     */
    void SetDefaultSettings();
    void SaveSettings();

    bool IsFirstRun();

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
    void SetCachePath( const wxString path );

    void SetCacheVersion();
    int GetCacheVersion();

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
    /*@}*/

    /* ================================================================ */
    /** @name Spring locations
     * @{
     */
    wxString GetSpringDir();
    void   SetSpringDir( const wxString& pring_dir );

    bool   GetUnitSyncUseDefLoc();
    void   SetUnitSyncUseDefLoc( const bool usedefloc );
    wxString GetUnitSyncLoc();
    void   SetUnitSyncLoc( const wxString& loc );
    wxString GetUnitSyncUsedLoc( bool force = false, bool defloc = false );

    bool   GetSpringUseDefLoc();
    void   SetSpringUseDefLoc( const bool usedefloc );
    wxString GetSpringLoc();
    void   SetSpringLoc( const wxString& loc );
    wxString GetSpringUsedLoc( bool force = false, bool defloc = false );
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
    /**@}*/

    /* Do these go in Chat? */
    bool GetSmartScrollEnabled();
    void SetSmartScrollEnabled(bool value);
    bool GetAlwaysAutoScrollOnFocusLost();
    void SetAlwaysAutoScrollOnFocusLost(bool value);


    /* ================================================================ */
    /** @name Hosting
     *
     * Settings to use when hosting games.  Includes "sticky" settings from the
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

    void SetLastHostDescription( const wxString& value );
    void SetLastHostMod( const wxString& value );
    void SetLastHostPassword( const wxString& value );
    void SetLastHostPort( int value );
    void SetLastHostPlayerNum( int value );
    void SetLastHostNATSetting( int value );
    void SetLastHostMap( const wxString& value );
    void SetLastRankLimit( int rank );
    void SetTestHostPort( bool value );
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
    wxString GetLastFilterProfileName();
    /**@}*/

    bool GetDisableSpringVersionCheck();

    /// not get/set naming because set may refer to battle or to options, thatd be ambiguous
    void SaveBattleMapOptions(IBattle *battle);
    void LoadBattleMapOptions(IBattle *battle);

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
    /**@}*/

  protected:
    #ifdef __WXMSW__
    myconf* m_config; //!< wxConfig object to store and restore  all settings in.
    #else
    wxConfigBase* m_config; //!< wxConfig object to store and restore  all settings in.
    #endif

    wxString m_chosed_path;
    bool m_portable_mode;

};

Settings& sett();

#endif // SPRINGLOBBY_HEADERGUARD_SETTINGS_H
