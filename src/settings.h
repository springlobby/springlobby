/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: Settings
// Created on: Thu May 10 23:28:05 2007
//

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

//#include <wx/wx.h>
#include <wx/config.h>
#include <wx/intl.h>
#include <wx/filefn.h>
#include <string>
#include "utils.h"


#define DEFSETT_DEFAULT_SERVER _T("TAS Server")
#define DEFSETT_DEFAULT_SERVER_HOST _T("taspringmaster.clan-sy.com")
#define DEFSETT_DEFAULT_SERVER_PORT 8200
#define DEFSETT_SAVE_PASSWORD false
#define DEFSETT_MW_WIDTH 600
#define DEFSETT_MW_HEIGHT 500
#define DEFSETT_MW_TOP 50
#define DEFSETT_MW_LEFT 50
#define DEFSETT_SPRING_DIR wxGetCwd()

//! @brief Class used to store and restore application settings.
class Settings
{
  public:
    Settings() {
      m_config = new wxConfig( _T("SpringLobby") );
      if ( !m_config->Exists( _T("/Servers") ) )
        SetDefaultSettings();
    }
    ~Settings() { m_config->Write( _T("/General/firstrun"), false ); delete m_config; }
  
    // Settings interface
  
    void SetDefaultSettings();
    void SaveSettings();
  
    bool IsFirstRun();

    std::string GetDefaultServer();
    void   SetDefaultServer( const std::string& server_name );
  
    bool   ServerExists( const std::string& server_name );
  
    std::string GetServerHost( const std::string& server_name );
    void   SetServerHost( const std::string& server_name, const std::string& value );
    
    int    GetServerPort( const std::string& server_name );
    void   SetServerPort( const std::string& server_name, const int value );
    
    std::string GetServerName( const std::string& server_name );
    void   SetServerName( const std::string& server_name, const std::string& value );
    
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

    bool   GetUnitsyncUseDefLoc();
    void   SetUnitsyncUseDefLoc( const bool usedefloc );
    std::string GetUnitsyncLoc();
    void   SetUnitsyncLoc( const std::string& loc );

    bool   GetSpringUseDefLoc();
    void   SetSpringUseDefLoc( const bool usedefloc );
    std::string GetSpringLoc();
    void   SetSpringLoc( const std::string& loc );
    std::string GetSpringUsedLoc();

  protected:
    // Settings variables
    
    wxConfig* m_config; //!< wxConfig object to store and restore  all settings in.
    
};


Settings& sett();

#endif  //_SETTINGS_H_

