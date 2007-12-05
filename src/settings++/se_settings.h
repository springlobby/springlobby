#ifndef SE_SETTINGS_H_
#define SE_SETTINGS_H_

//#include <wx/wx.h>

class wxString;
class wxConfigBase;

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
	wxString getUsyncLoc();
	void setUsyncLoc(wxString);
	static se_settings& getInstance();
	void save();
	
private:
	wxConfigBase* se_config;
	
	wxString AutoFindUnitSyncLib( const wxString& def );
	bool IsUnitSyncLib( const wxString& lib );
	
	static se_settings* instance ;
};

#endif /*SE_SETTINGS_H_*/
