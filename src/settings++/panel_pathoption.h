#ifndef PANEL_PATHOPTION_H_
#define PANEL_PATHOPTION_H_

#include <wx/panel.h>

class wxWindow;
class wxButton;
class wxBoxSizer;
class wxStaticBoxSizer;
class wxTextCtrl;
class wxStaticText;
class wxFlexGridSizer;
//class wxGridSizer;
class settings_frame;

class PathOptionPanel : public wxPanel
{
public:
	PathOptionPanel(wxWindow*,settings_frame*);
	virtual ~PathOptionPanel();
	void SetSpringPath(wxCommandEvent& );
	void SetUsyncPath(wxCommandEvent& );
	void UsePaths(wxCommandEvent& );
private:
	wxStaticText* springdir_lbl;
	    wxStaticText* usync_loc_lbl;
	    wxStaticText* explanation_text;
	    wxButton* m_dir_browse_btn;
	    
	    wxButton* springdir_browse_btn;
	    wxButton* paths_ok_btn;
	    wxButton* usync_browse_btn;
	   
	    wxTextCtrl* springdir_ctrl;
	    
	    wxTextCtrl* usync_ctrl;
	    settings_frame* origin;
	    wxFlexGridSizer* usync_sizer;
	    wxFlexGridSizer* springdir_sizer;
	    wxBoxSizer* subSizer;
	    wxStaticBoxSizer* main_sizer;
	    
	    DECLARE_EVENT_TABLE()
};

#endif /*PANEL_PATHOPTION_H_*/
