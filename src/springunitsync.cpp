/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/dynlib.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/file.h>
//#include <wx/txtstrm.h>
//#include <wx/wfstream.h>
#include <wx/textfile.h>
#include <cmath>
#include <stdexcept>
#include <clocale>

#include "springunitsync.h"
#include "utils.h"
#include "settings.h"
#include "springunitsynclib.h"
#include "settings++/custom_dialogs.h"


#define LOCK_UNITSYNC wxCriticalSectionLocker lock_criticalsection(m_lock)


struct CachedMapInfo
{
  char name[256];
  char author[256];
  char description[256];

  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  int width;
  int height;

  int posCount;
  StartPos positions[16];
};


IUnitSync* usync()
{
  static SpringUnitSync* m_sync = 0;
  if (!m_sync)
    m_sync = new SpringUnitSync;
  return m_sync;
}


bool SpringUnitSync::LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
#ifndef HAVE_WX26
  if ( !wxFileName::IsDirWritable( sett().GetSpringDir() ) )
  {
      customMessageBox(SL_MAIN_ICON,_("Unitsync loading was aborted because your spring data directory is not writable. Please check."),_("Unitsync Problem"),wxOK);
      wxLogDebugFunc( _T("sprindatadir not writable") );
      return false;
  }
  else
#endif
  {
     wxLogDebugFunc( _T("") );
     LOCK_UNITSYNC;
     return _LoadUnitSyncLib( springdir, unitsyncloc );
  }
}


bool SpringUnitSync::_LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
  wxSetWorkingDirectory( springdir );
  try {
    susynclib()->Load( unitsyncloc );
  } catch (...) {
    return false;
  }
  return true;
}


void SpringUnitSync::FreeUnitSyncLib()
{
  wxLogDebugFunc( _T("") );
  susynclib()->Unload();
}


bool SpringUnitSync::IsLoaded()
{
  return susynclib()->IsLoaded();
}


wxString SpringUnitSync::GetSpringVersion()
{
  wxLogDebugFunc( _T("") );
  wxString ret;
  try
  {
    ret = susynclib()->GetSpringVersion();
  }
  catch (...){}
  return ret;
}


bool SpringUnitSync::VersionSupports( GameFeature feature )
{
  wxString ver = GetSpringVersion();
  double nver = 0;
  ver = ver.BeforeFirst('b') + ver.AfterFirst('b'); //remove the beta flag

  const char* old_locale = std::setlocale(LC_NUMERIC, "C"); //temp switch to C locale for the decimal separator

  if ( ver.Contains( _T("+") ) ) //remove the + (development) flag, and increase the version
  {
    ver = ver.BeforeFirst('+');
    ver.ToDouble( &nver); // convert to float
    nver = std::floor ( ( nver * 100 ) + 0.9 ); // increments version and rounds up the decimal to 0
  }
  else
  {
    ver.ToDouble( &nver); // convert to float
    nver = nver * 100;
  }
  std::setlocale(LC_NUMERIC, old_locale);
  switch (feature) {
    case GF_XYStartPos: return nver >= 76.0;
    case USYNC_Sett_Handler: return nver >= 76.0;
  }
  return false;
}


int SpringUnitSync::GetNumMods()
{
  wxLogDebugFunc( _T("") );
  return susynclib()->GetPrimaryModCount();
}


int SpringUnitSync::GetModIndex( const wxString& name )
{
  wxLogDebugFunc( _T("name = \"") + name + _T("\"") );
  try
  {
    return susynclib()->GetModIndex( name );
  } catch (...){}
  return -1;
}


bool SpringUnitSync::ModExists( const wxString& modname )
{
  wxLogDebugFunc( _T("modname = \"") + modname + _T("\"") );
  try {
    return GetModIndex( modname ) >=0;
  } catch (...) {}
  return false;
}


UnitSyncMod SpringUnitSync::GetMod( const wxString& modname )
{
  wxLogDebugFunc( _T("modname = \"") + modname + _T("\"") );
  UnitSyncMod m;

  int i = susynclib()->GetPrimaryModIndex( modname);
  return GetMod( i );
}


UnitSyncMod SpringUnitSync::GetMod( int index )
{
  wxLogDebugFunc( _T("") );
  UnitSyncMod m;

  m.name = susynclib()->GetPrimaryModName( index );
  m.hash = wxString::Format( _T("%d"), susynclib()->GetPrimaryModChecksum( index ) );

  return m;
}


int SpringUnitSync::GetNumMaps()
{
  wxLogDebugFunc( _T("") );
  return susynclib()->GetMapCount();
}


bool SpringUnitSync::MapExists( const wxString& mapname )
{
  wxLogDebugFunc( _T("") );
  try {
    return GetMapIndex( mapname ) >= 0;
  } catch (...) {}
  return false;
}


bool SpringUnitSync::MapExists( const wxString& mapname, const wxString hash )
{
  wxLogDebugFunc( _T("") );
  int index;
  wxString usynchash;
  try {
    index = GetMapIndex( mapname );
    if ( index >= 0 ) {
      usynchash =  susynclib()->GetMapChecksum( index );
      return ( usynchash == hash );
    }
  } catch (...) {}
  return false;
}


UnitSyncMap SpringUnitSync::GetMap( const wxString& mapname )
{
  wxLogDebugFunc( _T("") );
  int i = GetMapIndex( mapname );
  return GetMap( i );
}

UnitSyncMap SpringUnitSync::GetMap( int index )
{
  wxLogDebugFunc( _T("") );
  UnitSyncMap m;
  m.name = susynclib()->GetMapName( index );
  m.hash = susynclib()->GetMapChecksum( index );
  return m;
}


UnitSyncMap SpringUnitSync::GetMapEx( int index )
{
  UnitSyncMap m;

  m.name = susynclib()->GetMapName( index );
  m.hash = susynclib()->GetMapChecksum( index );

  m.info = susynclib()->GetMapInfoEx( m.name, 1 );

  return m;
}

GameOptions SpringUnitSync::GetMapOptions( const wxString& name )
{
  wxLogDebugFunc( name );
  GameOptions ret;
  int count = susynclib()->GetMapOptionCount(name);
	for (int i = 0; i < count; ++i)
	{
    wxString key = susynclib()->GetOptionKey(i);
    switch (susynclib()->GetOptionType(i))
    {
    case opt_float:
      ret.float_map[key] = mmOptionFloat(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
          susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i));
      break;
    case opt_bool:
      ret.bool_map[key] = mmOptionBool(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i));
      break;
    case opt_string:
      ret.string_map[key] = mmOptionString(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i));
      break;
    case opt_list:
       ret.list_map[key] = mmOptionList(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionListDef(i));
       for (int j = 0; j < susynclib()->GetOptionListCount(i); ++j)
       {
         ret.list_map[key].addItem(susynclib()->GetOptionListItemKey(i,j),susynclib()->GetOptionListItemName(i,j),
                            susynclib()->GetOptionListItemDesc(i,j));
       }
    }
	}
	return ret;
}


UnitSyncMap SpringUnitSync::GetMapEx( const wxString& mapname )
{
  wxLogDebugFunc( _T("") );
  int i = GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, _T("Map does not exist") );
  return GetMapEx( i );
}


int SpringUnitSync::GetMapIndex( const wxString& name )
{
  try {
    int count = susynclib()->GetMapCount();
    for ( int i = 0; i < count; i++ ) {
      wxString cmp = susynclib()->GetMapName( i );
      if ( name == cmp )
        return i;
    }
  } catch(...) {}
  return -1;
}


wxString SpringUnitSync::GetModArchive( int index )
{
  wxLogDebugFunc( _T("") );
  LOCK_UNITSYNC;

  return _GetModArchive( index );
}


GameOptions SpringUnitSync::GetModOptions( const wxString& name )
{
  wxLogDebugFunc( name );
  GameOptions ret;
  int count = susynclib()->GetModOptionCount(name);
	for (int i = 0; i < count; ++i)
	{
    wxString key = susynclib()->GetOptionKey(i);
    switch (susynclib()->GetOptionType(i))
    {
    case opt_float:
      ret.float_map[key] = mmOptionFloat(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
          susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i));
      break;
    case opt_bool:
      ret.bool_map[key] = mmOptionBool(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i));
      break;
    case opt_string:
      ret.string_map[key] = mmOptionString(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i));
      break;
    case opt_list:
       ret.list_map[key] = mmOptionList(susynclib()->GetOptionName(i),key,
          susynclib()->GetOptionDesc(i),susynclib()->GetOptionListDef(i));
       for (int j = 0; j < susynclib()->GetOptionListCount(i); ++j)
       {
         ret.list_map[key].addItem(susynclib()->GetOptionListItemKey(i,j),susynclib()->GetOptionListItemName(i,j),
                            susynclib()->GetOptionListItemDesc(i,j));
       }
    }
	}
	return ret;
}


wxString SpringUnitSync::_GetModArchive( int index )
{
  return susynclib()->GetPrimaryModArchive( index );
}


int SpringUnitSync::GetSideCount( const wxString& modname )
{
  wxLogDebugFunc( _T("") );
  if ( !ModExists( modname ) ) return 0;
  return susynclib()->GetSideCount( modname );
}


wxString SpringUnitSync::GetSideName( const wxString& modname, int index )
{
  wxLogDebugFunc( _T("") );

  if ( (index < 0) || (!ModExists( modname )) ) return _T("unknown");
  susynclib()->AddAllArchives( _GetModArchive( susynclib()->GetModIndex( modname )  ) );
  if ( index >= GetSideCount( modname ) ) return _T("unknown");
  ASSERT_LOGIC( GetSideCount( modname ) > index, _T("Side index too high.") );
  return susynclib()->GetSideName( modname, index );
}


wxImage SpringUnitSync::GetSidePicture( const wxString& modname, const wxString& SideName )
{
  wxLogDebugFunc( _T("") );

  susynclib()->SetCurrentMod( modname );
  wxLogDebugFunc( _T("SideName = \"") + SideName + _T("\"") );
  wxString ImgName = _T("SidePics");
  ImgName += _T("/");
  ImgName += SideName.Upper();
  ImgName += _T(".bmp");

  int ini = susynclib()->OpenFileVFS (ImgName );
  ASSERT_RUNTIME( ini, _T("cannot find side image") );

  int FileSize = susynclib()->FileSizeVFS(ini);
  if (FileSize == 0) {
    susynclib()->CloseFileVFS(ini);
    ASSERT_RUNTIME( FileSize, _T("side image has size 0") );
  }

  char* FileContent = new char [FileSize];
  susynclib()->ReadFileVFS(ini, FileContent, FileSize);
  wxMemoryInputStream FileContentStream( FileContent, FileSize );

  wxImage ret( FileContentStream, wxBITMAP_TYPE_ANY, -1);
  delete[] FileContent;
  ret.InitAlpha();
  for ( int x = 0; x < ret.GetWidth(); x++ )
    for ( int y = 0; y < ret.GetHeight(); y++ )
      if ( ret.GetBlue( x, y ) == 255 && ret.GetGreen( x, y ) == 255 && ret.GetRed( x, y ) == 255 ) ret.SetAlpha( x, y, 0 ); /// set pixel to be transparent
  return ret;
}


wxArrayString SpringUnitSync::GetAIList( const wxString& modname )
{
  wxLogDebugFunc( _T("") );

  int ini = susynclib()->InitFindVFS(  wxDynamicLibrary::CanonicalizeName(_T("AI/Bot-libs/*"), wxDL_MODULE) );

  wxArrayString ret;
  wxString FileName;

  ini = susynclib()->FindFilesVFS( ini, FileName );
  while ( ini ) {
    if ( ret.Index( FileName.BeforeLast( '/') ) == wxNOT_FOUND ) ret.Add ( FileName ); // don't add duplicates
    ini = susynclib()->FindFilesVFS( ini, FileName );
  }

  try { // Older versions of unitsync does not have these functions.
    const int LuaAICount = susynclib()->GetLuaAICount( modname );
    for ( int i = 0; i < LuaAICount; i++ ) ret.Add( _( "LuaAI" ) +  susynclib()->GetLuaAIName( i ) );
  } catch (...) {}

  return ret;
}


int SpringUnitSync::GetNumUnits( const wxString& modname )
{
  wxLogDebugFunc( _T("") );

  susynclib()->AddAllArchives( susynclib()->GetPrimaryModArchive( susynclib()->GetModIndex( modname ) ) );
  susynclib()->ProcessUnitsNoChecksum();

  return susynclib()->GetUnitCount();
}


wxString _GetCachedModUnitsFileName( const wxString& mod )
{
  wxString path = sett().GetCachePath(); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname =  mod;
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  wxLogMessage( _T("%s"), path.c_str() );
  return path + fname + _T(".units");
}


wxArrayString SpringUnitSync::GetUnitsList( const wxString& modname )
{
  wxLogDebugFunc( _T("") );

  if ( m_mod_units.GetCount() > 0 ) return m_mod_units;

  wxArrayString ret;

  wxString path = _GetCachedModUnitsFileName( modname );
  try {

    ASSERT_RUNTIME( wxFileName::FileExists( path ), _T("Cache file does not exist") );
    wxTextFile f;
    ASSERT_RUNTIME( f.Open(path), _T("Failed to open file") );
    ASSERT_RUNTIME( f.GetLineCount() > 0, _T("File empty") );

    wxString str;
    for ( str = f.GetFirstLine(); !f.Eof(); str = f.GetNextLine() ) ret.Add( str );

    return ret;

  } catch(...) {}

  susynclib()->SetCurrentMod( modname );
  while ( susynclib()->ProcessUnitsNoChecksum() );
  for ( int i = 0; i < susynclib()->GetUnitCount(); i++ ) {
    wxString tmp = susynclib()->GetFullUnitName(i) + _T("(");
    tmp += susynclib()->GetUnitName(i) + _T(")");
    ret.Add( tmp );
  }

  m_mod_units = ret;
  try {

    wxFile f( path, wxFile::write );
    ASSERT_RUNTIME( f.IsOpened(), _T("Couldn't create file") );

    for ( unsigned int i = 0; i < ret.GetCount(); i++ ) {
      wxString tmp =  ret.Item(i);
      tmp += _T("\n");
      f.Write( tmp.c_str(), tmp.length() );
    }

    f.Close();

  } catch(...) {}

  return ret;
}


wxString SpringUnitSync::_GetCachedMinimapFileName( const wxString& mapname, int width, int height )
{
  wxString path = sett().GetCachePath(); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname =  mapname;
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  if ( width != -1 ) fname += wxString::Format( _T("%dx%d"), width, height );
  fname += _T(".png");
  return path + fname;
}


wxImage SpringUnitSync::_GetCachedMinimap( const wxString& mapname, int max_w, int max_h, bool store_size )
{
  wxString fname = store_size? _GetCachedMinimapFileName( mapname, max_w, max_h ) : _GetCachedMinimapFileName( mapname );
  ASSERT_RUNTIME( wxFileExists( fname ), _T("File cached image does not exist") );

  wxImage img( fname, wxBITMAP_TYPE_PNG );
  ASSERT_RUNTIME( img.Ok(), _T("Failed to load chache image") );

  if ( !store_size ) {

    UnitSyncMap map = GetMap( mapname );
    if ( map.hash != m_map.hash ) map = m_map = GetMapEx( mapname );
    else map = m_map;

    int height, width;

    width = max_w;
    height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
    if ( height > max_h ) {
      width = (int)((double)((double)width * (double)max_h) / (double)height);
      height = max_h;
    }

    img.Rescale( width, height );

  }

  return img;
}


wxImage SpringUnitSync::GetMinimap( const wxString& mapname, int max_w, int max_h, bool store_size )
{
  wxLogDebugFunc( _T("") );
  CacheMinimap( mapname );
  return _GetCachedMinimap( mapname, max_w, max_h, store_size );
}

MapInfo SpringUnitSync::_GetMapInfoEx( const wxString& mapname, bool force )
{
  wxLogMessage( _T("GetMapInfoEx cache lookup failed.") );
  MapInfo info = susynclib()->GetMapInfoEx( mapname, 0 );
  return info;
}


bool SpringUnitSync::CacheMapInfo( const wxString& map )
{
  return false;
}


bool SpringUnitSync::CacheMinimap( const wxString& mapname )
{
  wxLogDebug( _T("") );
  if ( wxFileExists( _GetCachedMinimapFileName( mapname ) ) &&
       wxFileExists( _GetCachedMinimapFileName( mapname, 160, 160 ) ) &&
       wxFileExists( _GetCachedMinimapFileName( mapname, 98, 98 ) )
     ) return false;

  int width = 512, height = 1024;

  wxImage ret;
  try {
    ret = susynclib()->GetMinimap( mapname );
  } catch (...) {
    return false;
  }

  UnitSyncMap map;
  try {
    map = GetMapEx( mapname );
  } catch(...) {
    return false;
  }

  ret.Rescale( 512, 512 );
  wxString fname = _GetCachedMinimapFileName( mapname );
  if ( !wxFileExists( fname ) ) ret.SaveFile( fname, wxBITMAP_TYPE_PNG );

  int max_w, max_h;
  for ( int i = 0; i <= 1; i++ ) {

    switch ( i ) {
      case 0: max_w = 160; max_h = 160; break;
      case 1: max_w = 98; max_h = 98; break;
    };

    width = max_w;
    height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
    if ( height > max_h ) {
      width = (int)((double)((double)width * (double)max_h) / (double)height);
      height = max_h;
    }

    ret.Rescale( width, height );
    ret.SaveFile( _GetCachedMinimapFileName( mapname, max_w, max_h ), wxBITMAP_TYPE_PNG );

  }
  return true;
}


bool SpringUnitSync::CacheModUnits( const wxString& mod )
{
  return false;
}


bool SpringUnitSync::ReloadUnitSyncLib()
{
  return false;
}


void SpringUnitSync::SetSpringDataPath( const wxString& path )
{
  susynclib()->SetSpringConfigString( _T("SpringData"), path );
}


wxString SpringUnitSync::GetSpringDataPath()
{
  return susynclib()->GetSpringConfigString( _T("SpringData"), sett().GetSpringDir() );
}


void SpringUnitSync::_ConvertSpringMapInfo( const CachedMapInfo& in, MapInfo& out )
{
  out.author = WX_STRINGC(in.author);
  out.description = WX_STRINGC(in.description);

  out.extractorRadius = in.extractorRadius;
  out.gravity = in.gravity;
  out.tidalStrength = in.tidalStrength;
  out.maxMetal = in.maxMetal;
  out.minWind = in.minWind;
  out.maxWind = in.maxWind;

  out.width = in.width;
  out.height = in.height;
  out.posCount = in.posCount;
  for ( int i = 0; i < in.posCount; i++) out.positions[i] = in.positions[i];
}


void SpringUnitSync::_ConvertSpringMapInfo( const SpringMapInfo& in, CachedMapInfo& out, const wxString& mapname )
{
  strncpy( &out.name[0], mapname.mb_str(), 256 );
  strncpy( &out.author[0], in.author, 256 );
  strncpy( &out.description[0], in.description, 256 );

  out.tidalStrength = in.tidalStrength;
  out.gravity = in.gravity;
  out.maxMetal = in.maxMetal;
  out.extractorRadius = in.extractorRadius;
  out.minWind = in.minWind;
  out.maxWind = in.maxWind;

  out.width = in.width;
  out.height = in.height;

  out.posCount = in.posCount;
  for ( int i = 0; i < 16; i++ ) out.positions[i] = in.positions[i];
}


void SpringUnitSync::_LoadMapInfoExCache()
{
  wxLogDebugFunc( _T("") );

  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  if ( !wxFileName::FileExists( path ) ) {
    wxLogMessage( _T("No cache file found.") );
    return;
  }

  wxFile f( path.c_str(), wxFile::read );
  if ( !f.IsOpened() ) {
    wxLogMessage( _T("failed to open file for reading.") );
    return;
  }

  m_mapinfo.clear();

  CachedMapInfo cinfo;
  while ( !f.Eof() ) {
    if ( (unsigned int)f.Read( &cinfo, sizeof(CachedMapInfo) ) < sizeof(CachedMapInfo) ) {
      wxLogError( _T("Cache file invalid") );
      m_mapinfo.clear();
      break;
    }
    m_mapinfo[ WX_STRINGC( &cinfo.name[0] ) ] = cinfo;
  }
  f.Close();
}


void SpringUnitSync::_SaveMapInfoExCache()
{
  wxLogDebugFunc( _T("") );
  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  wxFile f( path.c_str(), wxFile::write );
  if ( !f.IsOpened() ) {
    wxLogMessage( _T("failed to open file for writing.") );
    return;
  }

  MapCacheType::iterator i = m_mapinfo.begin();
  while ( i != m_mapinfo.end() ) {
    f.Write( &i->second, sizeof(CachedMapInfo) );
    i++;
  }
  f.Close();
}

bool SpringUnitSync::FileExists( const wxString& name )
{
    return ( susynclib()->OpenFileVFS(name) != 0 );
}
