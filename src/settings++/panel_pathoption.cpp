#include "panel_pathoption.h"

#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/stdpaths.h>

#include "se_utils.h"
#include "Defs.hpp"
#include "frame.h"
#include "se_settings.h"
#include "../nonportable.h"
#include "../springunitsynclib.h"


#include "custom_dialogs.h"

PathOptionPanel::PathOptionPanel(wxWindow* parent,settings_frame* _origin) : wxPanel(parent,-1),origin(_origin)
{
	usync_loc_lbl = new wxStaticText (this, -1 , _T("Path to unitsync shared library"));
	explanation_text = new wxStaticText (this, -1 , _T("There was a problem retrieving your settings.\n"
														"Please check that the path below is correct.\n"
														"When you have corrected it, click\n"
														"the \"Use this Paths\" button to try again."),
														wxDefaultPosition,wxSize(450,-1));

	
	paths_ok_btn = new wxButton(this,ID_PATH_OK_BTN,_T("Use this path"),wxDefaultPosition ,wxSize(-1,-1), wxBU_EXACTFIT);
	usync_browse_btn = new wxButton(this, ID_PATH_USYNC_BTN, _T("Browse") );

	

	usync_ctrl = new wxTextCtrl(this,-1,OptionsHandler.getUsyncLoc(), wxDefaultPosition,wxSize(400,-1));
	usync_ctrl->SetToolTip(_T("unitsync.so on linux, unitsync.dll on windows"));

	usync_sizer =  new wxFlexGridSizer(1,5,5);
	
	parentSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* subSizerB = new wxBoxSizer(wxHORIZONTAL);
	main_sizer = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Path settings")) ;

	usync_sizer->Add(usync_loc_lbl,1,wxEXPAND);

	subSizerB->Add(usync_ctrl,1,wxEXPAND);
	subSizerB->Add(usync_browse_btn,0,wxEXPAND);
	usync_sizer->Add(subSizerB,1,wxEXPAND);

	usync_sizer->SetSizeHints(this);
	usync_sizer->Fit(this);

	main_sizer->Add(explanation_text,1,wxALL|wxEXPAND,15);
	main_sizer->Add(usync_sizer,1,wxALL|wxEXPAND,10);
	main_sizer->Add(paths_ok_btn,0,wxALL|wxEXPAND,10);

	parentSizer->Add(main_sizer,0,wxALL,10);

	SetSizer(parentSizer);
}

void PathOptionPanel::SetUsyncPath(wxCommandEvent& event)
{
	wxFileDialog pic( this, _("Choose a unitsync library"), OptionsHandler.getSpringDir(), wxString(UNITSYNC_BIN), CHOOSE_DLL );
	  if ( pic.ShowModal() == wxID_OK )
		  usync_ctrl->SetValue( pic.GetPath() );
}

void PathOptionPanel::UsePaths(wxCommandEvent& event)
{
	OptionsHandler.setUsyncLoc(  usync_ctrl->GetValue() );
		
	loadUnitsync();
	
	if ( !(susynclib()->IsLoaded()) )
	{
		customMessageBox(SS_MAIN_ICON, _("SpringSettings is unable to load your unitsync library.\n\nYou might want to take another look at your unitsync setting."), _("Spring error"), wxOK );
	}
	else
	{
		origin->buildGuiFromErrorPanel();
		OptionsHandler.save();
	}

}

PathOptionPanel::~PathOptionPanel()
{
}


BEGIN_EVENT_TABLE(PathOptionPanel, wxPanel)
	EVT_BUTTON ( ID_PATH_USYNC_BTN, PathOptionPanel::SetUsyncPath )
	EVT_BUTTON ( ID_PATH_OK_BTN, PathOptionPanel::UsePaths )
END_EVENT_TABLE()

