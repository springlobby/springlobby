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
    Settings();
    ~Settings();

    bool IsPortableMode();

    void SetDefaultSettings();
    void SaveSettings();

    bool IsFirstRun();

    bool UseOldSpringLaunchMethod();
    void SetOldSpringLaunchMethod( bool value );

    bool GetNoUDP();
    void SetNoUDP(bool value);

    int GetClientPort();
    void SetClientPort(int value);

    bool GetShowIPAddresses();
    void SetShowIPAddresses(bool value);

    /** Fetch the "Use Default" setting for the web browser.
     *
     * @returns the current setting.
     */
    bool GetWebBrowserUseDefault();


    /** Set the "Use Default" setting for the web browser.  If @c
     * true, we use wxLaunchDefaultBrowser when we want to open a web
     * browser.
     *
     * @sa Ui::OpenWebBrowser
     */
    void SetWebBrowserUseDefault(bool useDefault);

    wxString GetWebBrowserPath();
    void SetWebBrowserPath( const wxString path );

    wxString GetCachePath();
    void SetCachePath( const wxString path );

    void SetCacheVersion();
    int GetCacheVersion();

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

    int GetNumChannelsJoin();
    void SetNumChannelsJoin( int num );
    void AddChannelJoin( const wxString& channel , const wxString& key );
    void RemoveChannelJoin( const wxString& channel );
    int GetChannelJoinIndex( const wxString& channel );
    wxString GetChannelJoinName( int index );

    wxString GetServerName( int index );

    wxString GetServerAccountNick( const wxString& server_name );
    void   SetServerAccountNick( const wxString& server_name, const wxString& value );

    wxString GetServerAccountPass( const wxString& server_name );
    void   SetServerAccountPass( const wxString& server_name, const wxString& value );

    bool   GetServerAccountSavePass( const wxString& server_name );
    void   SetServerAccountSavePass( const wxString& server_name, const bool value );

    int    GetMainWindowWidth();
    void   SetMainWindowWidth( const int value );

    int    GetMainWindowHeight();
    void   SetMainWindowHeight( const int value );

    int    GetMainWindowTop();
    void   SetMainWindowTop( const int value );

    int    GetMainWindowLeft();
    void   SetMainWindowLeft( const int value );

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

    bool GetChatLogEnable();
    void SetChatLogEnable( const bool value );
    wxString GetChatLogLoc();
    void   SetChatLogLoc( const wxString& loc );

    wxString GetLastHostDescription();
    wxString GetLastHostMod();
    wxString GetLastHostPassword();
    int GetLastHostPort();
    int GetLastHostPlayerNum();
    int GetLastHostNATSetting();
    wxString GetLastHostMap();
    int GetLastRankLimit();
    bool GetTestHostPort();

    wxColour GetBattleLastColour();
    void SetBattleLastColour( const wxColour& col );

    void SetLastHostDescription( const wxString& value );
    void SetLastHostMod( const wxString& value );
    void SetLastHostPassword( const wxString& value );
    void SetLastHostPort( int value );
    void SetLastHostPlayerNum( int value );
    void SetLastHostNATSetting( int value );
    void SetLastHostMap( const wxString& value );
    void SetLastRankLimit( int rank );
    void SetTestHostPort( bool value );


    void SetLastAI( const wxString& ai );
    wxString GetLastAI();

    void SetBalanceMethod(int value);
    int GetBalanceMethod();

    void SetBalanceClans(bool value);
    bool GetBalanceClans();

    void SetBalanceStrongClans(bool value);
    bool GetBalanceStrongClans();


    void SetDisplayJoinLeave( bool display, const wxString& channel  );
    bool GetDisplayJoinLeave( const wxString& channel );

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

    bool GetSmartScrollEnabled();
    void SetSmartScrollEnabled(bool value);
    bool GetAlwaysAutoScrollOnFocusLost();
    void SetAlwaysAutoScrollOnFocusLost(bool value);

    BattleListFilterValues GetBattleFilterValues(const wxString& profile_name = (_T("default")));
    void SetBattleFilterValues(const BattleListFilterValues& blfValues, const wxString& profile_name = _T("default"));
    wxString GetLastFilterProfileName();


	  bool GetDisableSpringVersionCheck();

    /// not get/set naming because set may refer to battle or to options, thatd be ambiguous
    void SaveBattleMapOptions(IBattle *battle);
    void LoadBattleMapOptions(IBattle *battle);

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

    void SetShowTooltips( bool show);
    bool GetShowTooltips();

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
