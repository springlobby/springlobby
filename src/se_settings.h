#ifndef SE_SETTINGS_H_
#define SE_SETTINGS_H_

//#include <wx/wx.h>
#include "Defs.hpp"

#include <wx/config.h>
//#include <wx/filefn.h>
//#include <wx/intl.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
//
#include <string>


#define SET_MODE_EXPERT 5000
#define SET_MODE_SIMPLE 5001

#define OptionsHandler (se_settings::getInstance()) 

class se_settings
{
public:
	se_settings();
	virtual ~se_settings();
	int getMode();
	void setMode(int);
	bool getDisableWarning();
	void setDisableWarning(bool);
	std::string getUsyncLoc();
	void setUsyncLoc(std::string);
	static se_settings& getInstance();
	
private:
	wxConfig* se_config;
	std::string getDefUsyncLoc();
	wxString AutoFindUnitSyncLib( const wxString& def );
	bool IsUnitSyncLib( const wxString& lib );
	
	static se_settings* instance ;
};

#endif /*SE_SETTINGS_H_*/
