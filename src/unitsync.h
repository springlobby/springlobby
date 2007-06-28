//
// Class: Unitsync
//

#ifndef _UNITSYNC_H_
#define _UNITSYNC_H_

#include <string>
#include "settings.h"
#include "utils.h"

#ifdef WIN32

#include <windows.h>
typedef HINSTANCE lib_handle_t;
static char* dllname = "\\unitsync.dll";

#else

#include <dlfcn.h>
typedef void* lib_handle_t;
static char* dllname = "/libunitsync.so";

#endif


struct UnitsyncMod
{
  UnitsyncMod(): name(""),hash("NULL") {}
  std::string name;
  std::string hash;
};


struct UnitsyncMap
{
  UnitsyncMap(): name(""),hash("NULL") {}
  std::string name;
  std::string hash;
};


#ifdef WIN32
typedef int (__stdcall *InitPtr)(bool, int);
typedef int (__stdcall *GetMapCountPtr)();
#else
typedef int (*InitPtr)(bool, int);
typedef int (*GetMapCountPtr)();
#endif


class Unitsync
{
public:
  Unitsync(): m_loaded(false) { LoadUnitsyncLib(); }
  ~Unitsync() { FreeUnitsyncLib(); }

  int GetNumMods() const;
  bool ModExists( const std::string& modname ) const;
  UnitsyncMod GetMod( const std::string& modname );

  int GetNumMaps() const;
  bool MapExists( const std::string& mapname ) const;
  UnitsyncMap GetMap( const std::string& mapname );

  bool LoadUnitsyncLib();
  void FreeUnitsyncLib();

  bool IsLoaded() const;
private:

  bool m_loaded;

  lib_handle_t m_libhandle;

  InitPtr m_init_ptr;
  GetMapCountPtr m_get_map_count_ptr;

  void* _GetLibFuncPtr( const std::string& name );
};


Unitsync& usync();

#endif  //_UNITSYNC_H_
