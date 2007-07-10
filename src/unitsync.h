//
// Class: Unitsync
//

#ifndef _UNITSYNC_H_
#define _UNITSYNC_H_

#include <string>
#include <wx/filefn.h>
#include <wx/msgdlg.h>

#include "settings.h"
#include "utils.h"

#ifdef WIN32
#include <windows.h>
#include <wx/msw/winundef.h>
typedef HINSTANCE lib_handle_t;
#else
#include <dlfcn.h>
typedef void* lib_handle_t;
#endif


struct UnitsyncMod
{
  UnitsyncMod(): name(""),hash("NULL") {}
  std::string name;
  std::string hash;
};


struct StartPos
{
	int x;
	int z;
};


struct MapInfo
{
	char* description;
	int tidalStrength;
	int gravity;
	float maxMetal;
	int extractorRadius;
	int minWind;
	int maxWind;

	// 0.61b1+
	int width;
	int height;
	int posCount;
	StartPos positions[16];		// I'd rather not allocate memory, this should be enough

	// VERSION>=1
	char *author; // max 200 chars
};


struct UnitsyncMap
{
  UnitsyncMap(): name(""),hash("NULL") {}
  std::string name;
  std::string hash;
  MapInfo info;
};


/*


const char* __stdcall GetSpringVersion()
void* __stdcall GetMinimap(const char* filename, int miplevel)

*/

#ifdef WIN32
#define USYNC_CALL_CONV __stdcall
#else
#define USYNC_CALL_CONV
#endif

typedef int (USYNC_CALL_CONV *InitPtr)(bool, int);
typedef void (USYNC_CALL_CONV *UnInitPtr)();

typedef int (USYNC_CALL_CONV *GetMapCountPtr)();
typedef unsigned int (USYNC_CALL_CONV *GetMapChecksumPtr)(int);
typedef const char* (USYNC_CALL_CONV *GetMapNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetMapInfoExPtr)(const char*, MapInfo*, int);

typedef unsigned int (USYNC_CALL_CONV *GetPrimaryModChecksumPtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModIndexPtr)(const char*);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetPrimaryModArchivePtr)(int);

typedef int (USYNC_CALL_CONV *GetSideCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetSideNamePtr)(int);

typedef void (USYNC_CALL_CONV *AddAllArchivesPtr)(const char*);

typedef const char * (USYNC_CALL_CONV *GetFullUnitNamePtr)(int);
typedef const char * (USYNC_CALL_CONV *GetUnitNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetUnitCountPtr)();
typedef int (USYNC_CALL_CONV *ProcessUnitsNoChecksumPtr)();

class Unitsync
{
public:
  Unitsync(): m_loaded(false) { LoadUnitsyncLib(); }
  ~Unitsync() { FreeUnitsyncLib(); }

  int GetNumMods();
  bool ModExists( const std::string& modname );
  UnitsyncMod GetMod( const std::string& modname );
  int GetModIndex( const std::string& name );
  std::string GetModArchive( int index );

  int GetNumMaps();
  bool MapExists( const std::string& mapname );
  bool MapExists( const std::string& mapname, const std::string hash );
  UnitsyncMap GetMap( const std::string& mapname );
  int GetMapIndex( const std::string& name );

  int GetSideCount( const std::string& modname );
  std::string GetSideName( const std::string& modname, int index );

  bool LoadUnitsyncLib();
  void FreeUnitsyncLib();

  bool IsLoaded();

  int GetNumUnits( const std::string& modname );
  int GetUnitIndex( const std::string& modname, const std::string& name );
  std::string GetFullUnitName( const std::string& modname, int index );

private:

  bool m_loaded;

  lib_handle_t m_libhandle;

  InitPtr m_init;
  UnInitPtr m_uninit;
  GetMapCountPtr m_get_map_count;
  GetMapChecksumPtr m_get_map_checksum;
  GetMapNamePtr m_get_map_name;
  GetMapInfoExPtr m_get_map_info_ex;

  GetPrimaryModChecksumPtr m_get_mod_checksum;
  GetPrimaryModIndexPtr m_get_mod_index;
  GetPrimaryModNamePtr m_get_mod_name;
  GetPrimaryModCountPtr m_get_mod_count;
  GetPrimaryModArchivePtr m_get_mod_archive;

  GetSideCountPtr m_get_side_count;
  GetSideNamePtr m_get_side_name;

  AddAllArchivesPtr m_add_all_archives;

  GetUnitCountPtr m_get_unit_count;
  GetUnitNamePtr m_get_unit_name;
  GetFullUnitNamePtr m_get_unit_full_name;
  ProcessUnitsNoChecksumPtr m_proc_units_nocheck;

  void* _GetLibFuncPtr( const std::string& name );
};


Unitsync& usync();

#endif  //_UNITSYNC_H_
