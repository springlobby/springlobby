#ifndef NEW_CONFIGHANDLER_H
#define NEW_CONFIGHANDLER_H

#include <string>
#include <wx/wx.h>
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
#include <wx/textfile.h>

#ifdef WIN32
  #define USYNC_CALL_CONV __stdcall
#else
  #define USYNC_CALL_CONV
#endif

#define configHandler (ConfigHandler::GetInstance())

typedef int (USYNC_CALL_CONV *GetSpringConfigInt)(const char* , int );
typedef const char* (USYNC_CALL_CONV *GetSpringConfigString)(const char* , const char* );

typedef void (USYNC_CALL_CONV *SetSpringConfigString)(const char* , const char* );
typedef void (USYNC_CALL_CONV *SetSpringConfigInt)(const char* , int );

class wxDynamicLibrary;

class wxCriticalSection;

class ConfigHandler
{
public:
	
	 void SetInt(std::string name, int value);

	
	 void SetString(std::string name, std::string value) ;

	
	 std::string GetString(std::string name, std::string def) ;

	int GetInt(std::string name, int def) ;
	
	//float GetFloat(const std::string& name, const float def) ;

	static ConfigHandler& GetInstance();
	
	static void Deallocate();
	ConfigHandler(): is_loaded(false){};
	virtual ~ConfigHandler();

protected:
	
	static ConfigHandler* instance;
	wxDynamicLibrary* m_libhandle;
	wxCriticalSection m_lock;
	
	bool LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
	void* GetLibFuncPtr( const std::string& name );
	
	GetSpringConfigInt h_GetSpringConfigInt;
	GetSpringConfigString h_GetSpringConfigString;
	
	SetSpringConfigString h_SetSpringConfigString;
	SetSpringConfigInt h_SetSpringConfigInt;
	
	bool is_loaded;
};

#endif /* CONFIGHANDLER_H */
