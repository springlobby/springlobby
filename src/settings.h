#ifndef SPRINGLOBBY_HEADERGUARD_SETTINGS_H
#define SPRINGLOBBY_HEADERGUARD_SETTINGS_H

#include <string>

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

    std::string GetDefaultServer();
    void SetDefaultServer( const std::string& server_name );

    bool ServerExists( const std::string& server_name );

    std::string GetServerHost( const std::string& server_name );
    void   SetServerHost( const std::string& server_name, const std::string& value );

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

    void SetLastHostDescription( const std::string& value );
    void SetLastHostMod( const std::string& value );
    void SetLastHostPassword( const std::string& value );
    void SetLastHostPort( int value );
    void SetLastHostPlayerNum( int value );
    void SetLastHostNATSetting( int value );
    void SetLastHostMap( const std::string& value );


    void SetLastAI( const std::string& ai );
    std::string GetLastAI();

  protected:

    wxConfigBase* m_config; //!< wxConfig object to store and restore  all settings in.

};

Settings& sett();

#endif // SPRINGLOBBY_HEADERGUARD_SETTINGS_H
