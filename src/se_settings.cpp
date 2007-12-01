#include "se_settings.h"

se_settings* se_settings::instance = 0;

se_settings& se_settings::getInstance()
{
	if (!instance){
			instance = new se_settings();
	}
	return *instance;
}

se_settings::se_settings()
{
	se_config = new wxConfig( _T("SpringLobby"), wxEmptyString, _T(".springlobby/springlobby.conf"), _T("springlobby.global.conf") );
	  //if ( !se_config->Exists( _T("/Server") ) ) SetDefaultSettings();
}

se_settings::~se_settings()
{
}

int se_settings::getMode()
{
	int mode;
	se_config->Read( _T("/SettingsPP/mode"), &mode, SET_MODE_SIMPLE );
	return mode;
}

void se_settings::setMode(int mode)
{
	se_config->Write( _T("/SettingsPP/mode"), mode );
}

bool se_settings::getDisableWarning()
{
	bool tmp;
	se_config->Read( _T("/SettingsPP/disableWarning"), &tmp, false );
	return tmp;
}

void se_settings::setDisableWarning(bool disable)
{
	se_config->Write( _T("/SettingsPP/disableWarning"), disable);
}

std::string se_settings::getUsyncLoc()
{
	wxString def;
	def = AutoFindUnitSyncLib(def);
	
	return STD_STRING(se_config->Read( _T("/Spring/unitsync_loc"), def ));
}

void se_settings::setUsyncLoc(std::string loc)
{
	se_config->Write( _T("/Spring/unitsync_loc"), _S(loc) );
}


std::string se_settings::getDefUsyncLoc()
{
	
}

/*copied from springlobby source*/
wxString se_settings::AutoFindUnitSyncLib( const wxString& def )
{
  wxPathList pl;
  wxStandardPathsBase& sp = wxStandardPathsBase::Get();

#ifdef __WXMSW__
  wxRegKey programreg( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion") );
  wxString tmp;
  if ( programreg.QueryValue( _T("ProgramFilesDir"), tmp ) ) pl.Add( tmp );

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

#ifdef HAVE_WX28
  pl.Add( sp.GetExecutablePath() );
#endif

  pl.Add( wxFileName::GetCwd() );

#ifdef HAVE_WX28
  pl.Add( sp.GetExecutablePath() );
#endif

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
    if ( IsUnitSyncLib( path + UNITSYNC_BIN ) ) return path + UNITSYNC_BIN;
    if ( IsUnitSyncLib( path + _T("Spring") + wxFileName::GetPathSeparator() + UNITSYNC_BIN ) ) return path + _T("Spring") + wxFileName::GetPathSeparator() + UNITSYNC_BIN;
    if ( IsUnitSyncLib( path + _T("spring") + wxFileName::GetPathSeparator() + UNITSYNC_BIN ) ) return path + _T("spring") + wxFileName::GetPathSeparator() + UNITSYNC_BIN;
  }

  return def;
}

bool se_settings::IsUnitSyncLib( const wxString& lib )
{
  if ( !wxFile::Exists( lib ) ) return false;
  return true;
}
