#include "se_settings.h"
#include "se_utils.h"
#include "Defs.hpp"

#include <wx/config.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/dir.h>
#include <wx/file.h>
#include "../utils.h"
#include "presets.h"
#include "custom_dialogs.h"
//#include <string>


se_settings* se_settings::instance = 0;

se_settings& se_settings::getInstance()
{
	if (!instance){
			instance = new se_settings();
	}
	return *instance;
}

void se_settings::save()
{
	se_config->Flush();
}

void se_settings::reload()
{
	delete se_config;
	se_config = new wxConfig( _T("SpringLobby"), wxEmptyString, _T(".springlobby/springlobby.conf"), _T("springlobby.global.conf") );
}

se_settings::se_settings()
{
	se_config = new wxConfig( _T("SpringLobby"), wxEmptyString, _T(".springlobby/springlobby.conf"), _T("springlobby.global.conf") );
}

se_settings::~se_settings()
{
	delete se_config;
}

int se_settings::getMode()
{
	int mode;
	se_config->Read( _T("/SpringSettings/mode"), &mode, SET_MODE_SIMPLE );
	return mode;
}

void se_settings::setMode(int mode)
{
	se_config->Write( _T("/SpringSettings/mode"), mode );
}

bool se_settings::getDisableWarning()
{
	bool tmp;
	se_config->Read( _T("/SpringSettings/disableWarning"), &tmp, false );
	return tmp;
}

void se_settings::setDisableWarning(bool disable)
{
	se_config->Write( _T("/SpringSettings/disableWarning"), disable);
}

wxString se_settings::getUsyncLoc()
{
	wxString def;
	def = AutoFindUnitSyncLib(def);
	def = (se_config->Read( _T("/Spring/unitsync_loc"), def ));
	return def;
}

void se_settings::setUsyncLoc(wxString loc)
{
	se_config->Write( _T("/Spring/unitsync_loc"), loc );
}

wxString se_settings::getSpringDir()
{
	wxString def;
	def= AutoFindSpringDir(def);

	se_config->Read( _T("/Spring/dir"), def );
	return def;
}
void se_settings::setSpringDir(wxString spring_dir)
{
	se_config->Write( _T("/Spring/dir"), spring_dir );
}

wxString se_settings::getSimpleRes()
{
	wxString def = vl_Resolution_Str[1];
	se_config->Read( _T("/SpringSettings/SimpleRes"),&def);
	return def;
}
void se_settings::setSimpleRes(wxString res)
{
	se_config->Write(_T("/SpringSettings/SimpleRes"),res);
}

wxString se_settings::getSimpleQuality()
{
	wxString def = wxT("medium");
	se_config->Read( _T("/SpringSettings/SimpleQuality"),&def);
	return def;
}

void se_settings::setSimpleQuality(wxString qual)
{
	se_config->Write(_T("/SpringSettings/SimpleQuality"),qual);
}

wxString se_settings::getSimpleDetail()
{
	wxString def = wxT("medium");
	se_config->Read( _T("/SpringSettings/SimpleDetail"),&def);
	return def;
}

void se_settings::setSimpleDetail(wxString det)
{
	se_config->Write(_T("/SpringSettings/SimpleDetail"),det);
}


/*copied from springlobby source*/
wxString se_settings::AutoFindSpringDir( const wxString& def )
{
  wxPathList pl;
  wxStandardPathsBase& sp = wxStandardPathsBase::Get();

  pl.Add( wxFileName::GetCwd() );
#ifdef HAVE_WX28
  pl.Add( sp.GetExecutablePath().BeforeLast( wxFileName::GetPathSeparator() ) );
#endif
  pl.Add( wxFileName::GetHomeDir() );
#ifdef __WXMSW__
  wxRegKey programreg( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion") );
  wxString tmp;
  if ( programreg.QueryValue( _T("ProgramFilesDir"), tmp ) ) pl.Add( tmp );

  pl.Add( _T("C:\\Program") );
  pl.Add( _T("C:\\Program Files") );
#endif
  pl.Add( sp.GetUserDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );
  pl.Add( sp.GetDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );
#ifdef HAVE_WX28
  pl.Add( sp.GetResourcesDir().BeforeLast( wxFileName::GetPathSeparator() ) );
#endif

  for ( size_t i = 0; i < pl.GetCount(); i++ ) {
    wxString path = pl[i] + wxFileName::GetPathSeparator();
    if ( IsDataDir( path ) ) return path;
    if ( IsDataDir( path + _T("Spring") ) ) return path + _T("Spring");
    if ( IsDataDir( path + _T("spring") ) ) return path + _T("spring");
    if ( IsDataDir( path + _T("spring_data") ) ) return path + _T("spring_data");
    if ( IsDataDir( path + _T(".spring") ) ) return path + _T(".spring");
    if ( IsDataDir( path + _T(".spring_data") ) ) return path + _T(".spring_data");
  }

  return def;
}

/*copied from springlobby source*/
wxString se_settings::AutoFindUnitSyncLib( const wxString& def )
{
  wxPathList pl;
  wxStandardPathsBase& sp = wxStandardPathsBase::Get();

#ifdef __WXMSW__
	#ifdef HAVE_WX28
	  pl.Add( sp.GetResourcesDir().BeforeLast( wxFileName::GetPathSeparator() ) );
	#endif
  wxRegKey* programreg = new wxRegKey( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Spring") );
  wxString tmp;
  if ( programreg->QueryValue( _T("DisplayIcon"), tmp ) ) pl.Add( tmp.BeforeLast( wxFileName::GetPathSeparator() ) );

   programreg = new wxRegKey( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion") );
if ( programreg->QueryValue( _T("ProgramFilesDir"), tmp ) ) pl.Add( tmp );

  pl.Add( wxGetOSDirectory() );
  pl.Add( _T("C:\\Program") );
  pl.Add( _T("C:\\Program Files") );
#else
  pl.Add( _T("/usr/local/lib64") );
  pl.Add( _T("/usr/local/games") );
  pl.Add( _T("/usr/local/games/lib") );
  pl.Add( _T("/usr/local/lib") );
  pl.Add( _T("/usr/lib64") );
  pl.Add( _T("/usr/lib") );
  pl.Add( _T("/usr/games") );
  pl.Add( _T("/usr/games/lib") );
#endif

  //pl.Add( m_dir_edit->GetValue() );
  pl.Add( wxFileName::GetCwd() );



  pl.Add( wxFileName::GetCwd() );


  pl.Add( wxFileName::GetHomeDir() );
  pl.Add( sp.GetUserDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );
  pl.Add( sp.GetDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );

#ifdef HAVE_WX28
  pl.Add( sp.GetResourcesDir().BeforeLast( wxFileName::GetPathSeparator() ) );
#endif

  //pl.Add( m_dir_edit->GetValue() );

  for ( size_t i = 0; i < pl.GetCount(); i++ ) {
    wxString path = pl[i];
    if ( path.Last() != wxFileName::GetPathSeparator() ) path += wxFileName::GetPathSeparator();
    if ( IsUnitSyncLib( path + _T("unitsync") + GetLibExtension() ) ) return path + _T("unitsync") + GetLibExtension();
    if ( IsUnitSyncLib( path + _T("Spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension() ) ) return path + _T("Spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension();
    if ( IsUnitSyncLib( path + _T("spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension() ) ) return path + _T("spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension();
  }

  return def;
}

bool se_settings::IsUnitSyncLib( const wxString& lib )
{
  if ( !(wxFile::Exists( lib )) ) return false;
  return true;
}

bool se_settings::IsDataDir( const wxString& dir )
{
  if ( wxDir::Exists( dir + wxFileName::GetPathSeparator() + _T("maps") ) ) return true;
  return false;
}

/************* SPRINGSETTINGS WINDOW POS/SIZE   ******************/
//! @brief Get left position of MainWindow.
int se_settings::GetSettingsWindowLeft()
{
  return se_config->Read( _T("/Settwin/left"), DEFSETT_SW_LEFT );
}

//! @brief Set left position of SettingsWindow
void se_settings::SetSettingsWindowLeft( const int value )
{
  se_config->Write( _T("/Settwin/left"), value );
}

//! @brief Get height of SettingsWindow.
int se_settings::GetSettingsWindowHeight()
{
  return se_config->Read( _T("/Settwin/height"), DEFSETT_SW_HEIGHT );
}


//! @brief Set height position of SettingsWindow
void se_settings::SetSettingsWindowHeight( const int value )
{
  se_config->Write( _T("/Settwin/height"), value );
}


//! @brief Get top position of SettingsWindow.
int se_settings::GetSettingsWindowTop()
{
  return se_config->Read( _T("/Settwin/top"), DEFSETT_SW_TOP );
}


//! @brief Set top position of SettingsWindow
void se_settings::SetSettingsWindowTop( const int value )
{
  se_config->Write( _T("/Settwin/top"), value );
}

//! @brief Get width of MainWindow.
int se_settings::GetSettingsWindowWidth()
{
  return se_config->Read( _T("/Settwin/width"), DEFSETT_SW_WIDTH );
}


//! @brief Set width position of MainWindow
void se_settings::SetSettingsWindowWidth( const int value )
{
  se_config->Write( _T("/Settwin/width"), value );
}

/*********** WINDOW SIZE/POS END *****************/
