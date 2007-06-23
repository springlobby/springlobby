//
// Class: Unitsync
//

#include <string>

#ifdef WIN32
#include <windows.h>
typedef HINSTANCE lib_handle_t;
#else
#include <dlfcn.h>
typedef void* lib_handle_t;
#endif

#include "utils.h"


struct UnitsyncMod
{
  std::string name;
  int hash;
};

struct UnitsyncMap
{
  std::string name;
  int hash;
};


typedef int (*InitPtr)(bool, int);

class Unitsync
{
public:
  Unitsync(): m_loaded(false) {}
  ~Unitsync() { FreeUnitsyncLib(); }

  int GetNumMods() const {}
  bool ModExists( const std::string& modname ) const {}
  UnitsyncMod GetMod( const std::string& modname ) const {}

  int GetNumMaps() const {}
  bool MapExists( const std::string& mapname ) const {}
  UnitsyncMap GetMap( const std::string& mapname ) const {}

  bool LoadUnitsyncLib();
  void FreeUnitsyncLib();

  bool IsLoaded() const;
private:

  bool m_loaded;

  lib_handle_t m_libhandle;

  InitPtr m_init_ptr;

  void* _GetLibFuncPtr( const std::string& name );
};
