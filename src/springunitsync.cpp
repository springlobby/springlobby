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
#include <wx/tokenzr.h>
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
     bool ret = _LoadUnitSyncLib( springdir, unitsyncloc );
     if (ret) PopulateArchiveList();
     return ret;
  }
}


void SpringUnitSync::PopulateArchiveList()
{
  m_maps_list.empty();
  m_mods_list.empty();
  m_mod_array.Empty();
  m_map_array.Empty();

  int numMaps = GetNumMaps();
  for ( int i = 0; i < numMaps; i++ )
  {
    wxString name, hash;
    try
    {
     name = susynclib()->GetMapName( i );
     hash = susynclib()->GetMapChecksum( i );
    } catch (...) { continue; }
    try
    {
      m_maps_list[name] = hash;
      m_map_array.Add( name );
    } catch (...)
    {
      wxLogError( _T("Found map with hash collision: ") + name + _T(" hash: ") + hash );
    }
  }
  int numMods = GetNumMods();
  for ( int i = 0; i < numMods; i++ )
  {
    wxString name, hash;
    try
    {
     name = susynclib()->GetPrimaryModName( i );
     hash = i2s(susynclib()->GetPrimaryModChecksum( i ));
    } catch (...) { continue; }
    try
    {
      m_mods_list[name] = hash;
      m_mod_array.Add( name );
    } catch (...)
    {
      wxLogError( _T("Found mod with hash collision: ") + name + _T(" hash: ") + hash );
    }
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


wxArrayString SpringUnitSync::GetModList()
{
  return m_mod_array;
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
  return (m_mods_list.find(modname) != m_mods_list.end());
}


bool SpringUnitSync::ModExists( const wxString& modname, const wxString& hash )
{
  LocalArchivesVector::iterator itor = m_mods_list.find(modname);
  if ( itor == m_mods_list.end() ) return false;
  return itor->second == hash;
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
  m.hash = i2s( susynclib()->GetPrimaryModChecksum( index ) );

  return m;
}


int SpringUnitSync::GetNumMaps()
{
  wxLogDebugFunc( _T("") );
  return susynclib()->GetMapCount();
}


wxArrayString SpringUnitSync::GetMapList()
{
  return m_map_array;
}


bool SpringUnitSync::MapExists( const wxString& mapname )
{
  return (m_maps_list.find(mapname) != m_maps_list.end());
}


bool SpringUnitSync::MapExists( const wxString& mapname, const wxString& hash )
{
  LocalArchivesVector::iterator itor = m_maps_list.find(mapname);
  if ( itor == m_maps_list.end() ) return false;
  return itor->second == hash;
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

  m.info = _GetMapInfoEx( m.name );

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


wxString SpringUnitSync::_GetModArchive( int index )
{
  return susynclib()->GetPrimaryModArchive( index );
}


wxString SpringUnitSync::GetMapArchive( int index )
{
  wxLogDebugFunc( _T("") );
  LOCK_UNITSYNC;

  int count = susynclib()->GetMapArchiveCount( index );

  if ( count > 0 )
    return susynclib()->GetMapArchiveName( 0 );
  else
    return _T("");
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

  /// list dynamic link libraries
  int dllini = susynclib()->InitFindVFS(  wxDynamicLibrary::CanonicalizeName(_T("AI/Bot-libs/*"), wxDL_MODULE) );

  wxArrayString ret;
  wxString FileName;

  dllini = susynclib()->FindFilesVFS( dllini, FileName );
  while ( dllini ) {
    if ( ret.Index( FileName.BeforeLast( '/') ) == wxNOT_FOUND ) ret.Add ( FileName ); /// don't add duplicates
    dllini = susynclib()->FindFilesVFS( dllini, FileName );
  }
  /// list jar files (java AIs)
  int jarini = susynclib()->InitFindVFS(  _T("AI/Bot-libs/*.jar") );

  jarini = susynclib()->FindFilesVFS( jarini, FileName );
  while ( jarini ) {
    if ( ret.Index( FileName.BeforeLast( '/') ) == wxNOT_FOUND ) ret.Add ( FileName ); /// don't add duplicates
    jarini = susynclib()->FindFilesVFS( jarini, FileName );
  }

  /// list mod's LuaAI
  try { /// Older versions of unitsync does not have these functions.
    const int LuaAICount = susynclib()->GetLuaAICount( modname );
    for ( int i = 0; i < LuaAICount; i++ ) ret.Add( _T( "LuaAI:" ) +  susynclib()->GetLuaAIName( i ) );
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
  return path << fname << _T(".units");
}


wxArrayString SpringUnitSync::GetUnitsList( const wxString& modname )
{
  wxLogDebugFunc( modname );

  wxArrayString ret;

  wxString path = _GetCachedModUnitsFileName( modname );
  try {

    ASSERT_RUNTIME( wxFileName::FileExists( path ), _T("Cache file does not exist") );
    wxFile f;
    ASSERT_RUNTIME( f.Open(path), _T("Failed to open mod unit cache file") );
    ASSERT_RUNTIME( f.Length() > 0, _T("mod unit cache file has lenght 0") );

    char* buff = new char[f.Length()];
    f.Read( buff, f.Length() );
    std::string tempstring = buff;
    delete buff;
    ASSERT_RUNTIME( tempstring.length() > 0, _T("mod unit list file is empty") );
    wxString stringbuff = WX_STRING(tempstring);
    ASSERT_RUNTIME( !stringbuff.IsEmpty(), _T("failed to convert to wxString the mod unit list cache file") );
    ret = wxStringTokenize( stringbuff, _T('\n'), wxTOKEN_RET_EMPTY );

  } catch(...)
  {
    ret.Empty();
    susynclib()->SetCurrentMod( modname );
    while ( susynclib()->ProcessUnitsNoChecksum() );
    unsigned int unitcount = susynclib()->GetUnitCount();
    for ( unsigned int i = 0; i < unitcount; i++ )
    {
      ret.Add( susynclib()->GetFullUnitName(i) << _T(" (") << susynclib()->GetUnitName(i) << _T(")") );
    }

    try {

      wxFile f( path, wxFile::write );
      ASSERT_RUNTIME( f.IsOpened(), _T("Couldn't create file") );

      wxString tmp;

      for ( unsigned int i = 0; i < ret.GetCount(); i++ ) tmp <<  ret[i] << _T('\n');

      f.Write( tmp );
      f.Close();

    } catch(...) {}
  }

  return ret;
}


wxString SpringUnitSync::_GetCachedMinimapFileName( const wxString& mapname, int width, int height )
{
  wxString path = sett().GetCachePath();
  wxString fname =  mapname;
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  if ( width != -1 ) fname << wxString::Format( _T("%dx%d"), width, height );
  fname << _T(".png");
  return path << fname;
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

MapInfo SpringUnitSync::_GetMapInfoEx( const wxString& mapname )
{
  MapInfo info;
  try
  {
    info = _LoadMapInfoExCache( mapname );
  }
  catch (...)
  {
    info = susynclib()->GetMapInfoEx( mapname, 1 );
    try
    {
      _SaveMapInfoExCache( mapname, info );
    }
    catch (...) {}
  }

  return info;
}


bool SpringUnitSync::CacheMapInfo( const wxString& mapname )
{
    try
    {
      _SaveMapInfoExCache( mapname, susynclib()->GetMapInfoEx( mapname, 1 ) );
    }
    catch (...) { return false; }
    return true;
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
  max_w = max_h = 160;
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
  usync()->FreeUnitSyncLib();
  usync()->LoadUnitSyncLib( sett().GetSpringDir(), sett().GetUnitSyncUsedLoc() );
  return true;
}


void SpringUnitSync::SetSpringDataPath( const wxString& path )
{
  susynclib()->SetSpringConfigString( _T("SpringData"), path );
}


wxString SpringUnitSync::GetSpringDataPath()
{
  return susynclib()->GetSpringConfigString( _T("SpringData"), sett().GetSpringDir() );
}


MapInfo SpringUnitSync::_LoadMapInfoExCache( const wxString& mapname )
{
  wxLogDebugFunc( mapname );

  wxString path = sett().GetCachePath() + mapname + _T(".infoex");

  ASSERT_RUNTIME( wxFileName::FileExists( path ), _T("No map info ex cache file found.") );

  wxFile f( path, wxFile::read );

  ASSERT_RUNTIME( f.IsOpened(), _T("failed to open map info ex cache file for reading.") );
  ASSERT_RUNTIME( f.Length() > 0 , _T("map info ex cache file has lenght 0") );
  char* buff = new char[f.Length()];
  f.Read( buff, f.Length() );
  std::string tempstring = buff;
  delete buff;
  ASSERT_RUNTIME( tempstring.length() > 0, _T("map info ex cache file is empty") );
  wxString stringbuff = WX_STRING(tempstring);
  ASSERT_RUNTIME( !stringbuff.IsEmpty(), _T("failed to convert to wxString the map info ex cache file") );
  wxArrayString data = wxStringTokenize( stringbuff , _T('\t'), wxTOKEN_RET_EMPTY );
  MapInfo info;

  ASSERT_RUNTIME( data.GetCount() > 0, _T("no lines found in cache info ex") );
  info.description = data[0];
  ASSERT_RUNTIME( data[1].ToLong( (long*)&info.tidalStrength ), _T("failed to parse the tidal in map info ex cache file") );
  ASSERT_RUNTIME( data[2].ToLong( (long*)&info.gravity ), _T("failed to parse the gravity in map info ex cache file") );
  ASSERT_RUNTIME( data[3].ToLong( (long*)&info.maxMetal ), _T("failed to parse the extractor maxmetal in map info ex cache file") );
  ASSERT_RUNTIME( data[4].ToDouble( (double*)&info.extractorRadius ), _T("failed to parse the mexradius in map info ex cache file") );
  ASSERT_RUNTIME( data[5].ToLong( (long*)&info.minWind ), _T("failed to parse the minwind in map info ex cache file") );
  ASSERT_RUNTIME( data[6].ToLong( (long*)&info.maxWind ), _T("failed to parse the maxwind in map info ex cache file") );
  ASSERT_RUNTIME( data[7].ToLong( (long*)&info.width ), _T("failed to parse the width in map info ex cache file") );
  ASSERT_RUNTIME( data[8].ToLong( (long*)&info.height ), _T("failed to parse the height in map info ex cache file") );
  ASSERT_RUNTIME( data[9].ToLong( (long*)&info.posCount ), _T("failed to parse the poscount in map info ex cache file") );

  wxArrayString posinfo = wxStringTokenize( data[10], _T(' '), wxTOKEN_RET_EMPTY );
  for ( int i = 0; i < info.posCount; i++)
  {
     StartPos position;
     ASSERT_RUNTIME( posinfo[i].BeforeFirst( _T('-') ).ToLong( (long*)&position.x ), _T("failed to parse the x coordinate in map info ex cache file") );
     ASSERT_RUNTIME( posinfo[i].AfterFirst( _T('-') ).ToLong( (long*)&position.y ), _T("failed to parse the y coordinate in map info ex cache file") );
     info.positions[i] = position;
  }
  if ( data.GetCount() > 11 )
  {
    info.author = data[11];
  }

  f.Close();
  return info;
}


void SpringUnitSync::_SaveMapInfoExCache( const wxString& mapname, const MapInfo& info )
{
  wxLogDebugFunc( _T("") );
  wxString path = sett().GetCachePath() + mapname + _T(".infoex");

  wxFile f( path.c_str(), wxFile::write );
  ASSERT_RUNTIME( f.IsOpened(), _T("failed to open map info ex cache file for writing.") );

  wxString buff;
  buff << info.description << _T('\t');
  buff << TowxString( info.tidalStrength ) << _T('\t');
  buff << TowxString( info.gravity ) << _T('\t');
  buff << TowxString( info.maxMetal ) << _T('\t');
  buff << TowxString( info.extractorRadius ) << _T('\t');
  buff << TowxString( info.minWind ) << _T('\t');
  buff << TowxString( info.maxWind ) << _T('\t');
  buff << TowxString( info.width ) << _T('\t');
  buff << TowxString( info.height ) << _T('\t');
  buff << TowxString( info.posCount ) << _T('\t');
  for ( int i = 0; i < info.posCount; i++)
  {
     buff << TowxString( info.positions[i].x ) << _T('-') << TowxString( info.positions[i].y ) << _T(' ');
  }
  buff << _T('\t');
  buff << info.author << _T('\t');

  f.Write( buff );
  f.Close();
}


bool SpringUnitSync::FileExists( const wxString& name )
{
  int handle = susynclib()->OpenFileVFS(name);
  if ( handle == 0 ) return false;
  susynclib()->CloseFileVFS(handle);
  return true;
}


wxString SpringUnitSync::GetArchivePath( const wxString& name )
{
  wxLogDebugFunc( _T("") );
  LOCK_UNITSYNC;

  return susynclib()->GetArchivePath( name );
}
