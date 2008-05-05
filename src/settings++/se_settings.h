#ifndef SE_SETTINGS_H_
#define SE_SETTINGS_H_

class wxString;
class wxConfigBase;

#define SET_MODE_EXPERT 5000
#define SET_MODE_SIMPLE 5001

#define DEFSETT_SW_WIDTH 770
#define DEFSETT_SW_HEIGHT 580
#define DEFSETT_SW_TOP 50
#define DEFSETT_SW_LEFT 50

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
	wxString getSpringDir();
	void setUsyncLoc(wxString);
	void setSpringDir(wxString);
	wxString getSimpleRes();
	void setSimpleRes(wxString);
	wxString getSimpleQuality();
	void setSimpleQuality(wxString);
	wxString getSimpleDetail();
	void setSimpleDetail(wxString);

    int    GetSettingsWindowWidth();
    void   SetSettingsWindowWidth( const int value );

    int    GetSettingsWindowHeight();
    void   SetSettingsWindowHeight( const int value );

    int    GetSettingsWindowTop();
    void   SetSettingsWindowTop( const int value );

    int    GetSettingsWindowLeft();
    void   SetSettingsWindowLeft( const int value );

	static se_settings& getInstance();
	void reload();
	void save();

private:
	wxConfigBase* se_config;

	//** copied from springlobby source
	wxString AutoFindUnitSyncLib( const wxString& def );
	wxString AutoFindSpringDir( const wxString& def );
	bool IsUnitSyncLib( const wxString& lib );
	bool IsDataDir( const wxString& dir );
	//***

	static se_settings* instance ;
};

#endif /*SE_SETTINGS_H_*/
