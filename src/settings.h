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
#include <string>
#include "utils.h"

using namespace std;


#define DEFSETT_DEFAULT_SERVER "TA Spring Server"
#define DEFSETT_DEFAULT_SERVER_HOST "taspringmaster.clan-sy.com"
#define DEFSETT_DEFAULT_SERVER_PORT 8200
#define DEFSETT_SAVE_PASSWORD false
#define DEFSETT_MW_WIDTH 600
#define DEFSETT_MW_HEIGHT 500
#define DEFSETT_MW_TOP 50
#define DEFSETT_MW_LEFT 50

//! @brief Class used to store and restore application settings.
class Settings
{
  public:
    Settings();
    virtual ~Settings();
  
    // Settings interface
  
    void SetDefaultSettings();
    void SaveSettings();
  
    string GetDefaultServer();
    void   SetDefaultServer( const string server_name );
  
    bool   ServerExists( const string server_name );
  
    string GetServerHost( const string server_name );
    void   SetServerHost( const string server_name, const string value );
    
    int    GetServerPort( const string server_name );
    void   SetServerPort( const string server_name, const int value );
    
    string GetServerName( const string server_name );
    void   SetServerName( const string server_name, const string value );
    
    string GetServerAccountNick( const string server_name );
    void   SetServerAccountNick( const string server_name, const string value );
    
    string GetServerAccountPass( const string server_name );
    void   SetServerAccountPass( const string server_name, const string value );
    
    bool   GetServerAccountSavePass( const string server_name );
    void   SetServerAccountSavePass( const string server_name, const bool value );
    
    int    GetMainWindowWidth();
    void   SetMainWindowWidth( const int value );

    int    GetMainWindowHeight();
    void   SetMainWindowHeight( const int value );

    int    GetMainWindowTop();
    void   SetMainWindowTop( const int value );

    int    GetMainWindowLeft();
    void   SetMainWindowLeft( const int value );

/*  
    bool   GetServer( const string server_name );
    void   SetServer( const string server_name, const bool value );
    
    int    GetServer( const string server_name );
    void   SetServer( const string server_name, const int value );
    
    string GetServer( const string server_name );
    void   SetServer( const string server_name, const string value );
    
*/
  
  protected:
    // Settings variables
    
    wxConfig* m_config; //!< wxConfig object to store and restore  all settings in.
    
};


Settings& sett();

#endif  //_SETTINGS_H_

