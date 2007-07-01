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

const char * __stdcall GetUnitName(int unit)
int __stdcall GetUnitCount()
const char* __stdcall GetSpringVersion()
const char* __stdcall GetSideName(int side)
int __stdcall GetSideCount()
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

class Unitsync
{
public:
  Unitsync(): m_loaded(false) { LoadUnitsyncLib(); }
  ~Unitsync() { FreeUnitsyncLib(); }

  int GetNumMods();
  bool ModExists( const std::string& modname );
  UnitsyncMod GetMod( const std::string& modname );
  int GetModIndex( const std::string& name );

  int GetNumMaps();
  bool MapExists( const std::string& mapname );
  bool MapExists( const std::string& mapname, const std::string hash );
  UnitsyncMap GetMap( const std::string& mapname );
  int GetMapIndex( const std::string& name );

  bool LoadUnitsyncLib();
  void FreeUnitsyncLib();

  bool IsLoaded();
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

  void* _GetLibFuncPtr( const std::string& name );
};


Unitsync& usync();

#endif  //_UNITSYNC_H_
