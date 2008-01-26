#ifndef SPRINGLOBBY_HEADERGUARD_SETTINGS_H
#define SPRINGLOBBY_HEADERGUARD_SETTINGS_H

#include <string>
#include <wx/colour.h>

#define CACHE_VERSION 1

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

class wxConfigBase;
class wxFont;


//! @brief Class used to store and restore application settings.
class Settings
{
  public:
    Settings();
    ~Settings();

    void SetDefaultSettings();
    void SaveSettings();

    bool IsFirstRun();

    bool UseOldSpringLaunchMethod();
    void SetOldSpringLaunchMethod( bool value );

    wxString GetWebBrowserPath();
    void SetWebBrowserPath( const wxString path );

    wxString GetCachePath();
    void SetCachePath( const wxString path );

    void SetCacheVersion();
    int GetCacheVersion();

    std::string GetDefaultServer();
    void SetDefaultServer( const std::string& server_name );

    bool ServerExists( const std::string& server_name );

    std::string GetServerHost( const std::string& server_name );
    void SetServerHost( const std::string& server_name, const std::string& value );

    int GetServerPort( const std::string& server_name );
    void SetServerPort( const std::string& server_name, const int value );

    int GetNumServers();
    void SetNumServers( int num );
    void AddServer( const std::string& server_name );
    int GetServerIndex( const std::string& server_name );

    int GetNumChannelsJoin();
    void SetNumChannelsJoin( int num );
    void AddChannelJoin( const std::string& channel , const std::string& key );
    void RemoveChannelJoin( const std::string& channel );
    int GetChannelJoinIndex( const std::string& channel );
    std::string GetChannelJoinName( int index );

    std::string GetServerName( int index );

    std::string GetServerAccountNick( const std::string& server_name );
    void   SetServerAccountNick( const std::string& server_name, const std::string& value );

    std::string GetServerAccountPass( const std::string& server_name );
    void   SetServerAccountPass( const std::string& server_name, const std::string& value );

    bool   GetServerAccountSavePass( const std::string& server_name );
    void   SetServerAccountSavePass( const std::string& server_name, const bool value );

    int    GetMainWindowWidth();
    void   SetMainWindowWidth( const int value );

    int    GetMainWindowHeight();
    void   SetMainWindowHeight( const int value );

    int    GetMainWindowTop();
    void   SetMainWindowTop( const int value );

    int    GetMainWindowLeft();
    void   SetMainWindowLeft( const int value );

    std::string GetSpringDir();
    void   SetSpringDir( const std::string& pring_dir );

    bool   GetUnitSyncUseDefLoc();
    void   SetUnitSyncUseDefLoc( const bool usedefloc );
    std::string GetUnitSyncLoc();
    void   SetUnitSyncLoc( const std::string& loc );
    std::string GetUnitSyncUsedLoc( bool force = false, bool defloc = false );

    bool   GetSpringUseDefLoc();
    void   SetSpringUseDefLoc( const bool usedefloc );
    std::string GetSpringLoc();
    void   SetSpringLoc( const std::string& loc );
    std::string GetSpringUsedLoc( bool force = false, bool defloc = false );

    bool GetChatLogEnable();
    void SetChatLogEnable( const bool value );
    wxString GetChatLogLoc();
    void   SetChatLogLoc( const wxString& loc );

    std::string GetLastHostDescription();
    std::string GetLastHostMod();
    std::string GetLastHostPassword();
    int GetLastHostPort();
    int GetLastHostPlayerNum();
    int GetLastHostNATSetting();
    std::string GetLastHostMap();
    int GetLastRankLimit();

    void SetLastHostDescription( const std::string& value );
    void SetLastHostMod( const std::string& value );
    void SetLastHostPassword( const std::string& value );
    void SetLastHostPort( int value );
    void SetLastHostPlayerNum( int value );
    void SetLastHostNATSetting( int value );
    void SetLastHostMap( const std::string& value );
    void SetLastRankLimit( int rank );


    void SetLastAI( const std::string& ai );
    std::string GetLastAI();

    void SetDisplayJoinLeave( bool display, const wxString& channel  );
    bool GetDisplayJoinLeave( const wxString& channel );

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

  protected:

    wxConfigBase* m_config; //!< wxConfig object to store and restore  all settings in.

};

Settings& sett();

#endif // SPRINGLOBBY_HEADERGUARD_SETTINGS_H
