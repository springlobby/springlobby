/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef __TAB_ABSTRACT_h__
#define __TAB_ABSTRACT_h__

#include <wx/scrolwin.h>
#include <map>
class wxString;
class wxCommandEvent;

class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxSpinEvent;
class wxSpinDoubleEvent;


typedef std::map<wxString, int> intMap;
typedef std::map<wxString, wxString> stringMap;
typedef std::map<wxString, float> floatMap;

template <int numerator, int denominator = 1>
class Scaler
{
private:
public:
	template <typename T>
	static T Up(const T val)
	{
		const double fac = denominator != 0 ? numerator / (double)denominator : numerator;
		return (T)(fac * val);
	}

	template <typename T>
	static T Down(const T val)
	{
		const double fac = denominator != 0 ? numerator / (double)denominator : numerator;
		return (T)(val / fac);
	}
};


class abstract_panel : public wxScrolledWindow
{

public:
	abstract_panel(wxWindow* parent, wxWindowID id = 1, const wxString& title = wxT("Project2"),
		       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~abstract_panel();

	void OnSliderMove(wxCommandEvent&);
	void OnTextUpdate(wxCommandEvent&);
	void OnCheckBoxTick(wxCommandEvent&);
	void OnRadioButtonToggle(wxCommandEvent&);
	//void update(wxIdleEvent&);
	virtual void OnComboBoxChange(wxCommandEvent& event);
	static intMap intSettings;
	static stringMap stringSettings;
	static floatMap floatSettings;
	static bool settingsChanged;
	static bool saveSettings();

	static void loadDefaults();
	virtual void updateControls(int what_to_update);
	static bool loadValuesIntoMap();
	void OnSpinControlChange(wxSpinEvent& event);
	void OnSpinCtrlDoubleChange(wxSpinDoubleEvent& event);

private:
	void OnClose(wxCloseEvent& event);
	void CreateGUIControls();

	DECLARE_EVENT_TABLE()
};

#endif
