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
#include <wx/dynlib.h>

#include "se_utils.h"
#include "ctrlconstants.h"
#include "frame.h"
#include "../settings.h"
#include "../nonportable.h"
#include <lslunitsync/unitsync.h>
#include <lslunitsync/c_api.h>
#include "se_utils.h"


#include "../utils/customdialogs.h"

PathOptionPanel::PathOptionPanel(wxWindow* parent,settings_frame* _origin) : wxPanel(parent,-1),origin(_origin)
{
	usync_loc_lbl = new wxStaticText (this, -1 , _("Path to unitsync shared library"));
	explanation_text = new wxStaticText (this, -1 , _("There was a problem retrieving your settings.\n\
														Please check that the path below is correct.\n\
														When you have corrected it, click\n\
														the \"Use this Path\" button to try again."),
														wxDefaultPosition,wxSize(450,-1));


	paths_ok_btn = new wxButton(this,ID_PATH_OK_BTN,_("Use this path"),wxDefaultPosition ,wxSize(-1,-1), wxBU_EXACTFIT);
	usync_browse_btn = new wxButton(this, ID_PATH_USYNC_BTN, _("Browse") );



	usync_ctrl = new wxTextCtrl(this,-1, sett().GetCurrentUsedUnitSync(), wxDefaultPosition,wxSize(400,-1));
	usync_ctrl->SetToolTip(_("libunitsync.so on linux, unitsync.dll on windows"));

	usync_sizer =  new wxFlexGridSizer(1,5,5);

	parentSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* subSizerB = new wxBoxSizer(wxHORIZONTAL);
	main_sizer = new wxStaticBoxSizer(wxVERTICAL ,this,_("Path settings")) ;

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

void PathOptionPanel::SetUsyncPath(wxCommandEvent& /*unused*/)
{
  wxString lib_ext = wxDynamicLibrary::CanonicalizeName(_T(""), wxDL_MODULE);
#if defined(__WXGTK__)
  wxString lib_pre = _T("lib");
#else
  wxString lib_pre = _T("");
#endif

  wxFileDialog pic( this, _("Choose an unitsync library"),
                wxPathOnly( sett().AutoFindSpringBin() ),
                lib_pre + _T("unitsync") + lib_ext, wxString(_T("Library")) + _T("(*") + lib_ext + _T(")|*") + lib_ext + _T("|") + wxString(_("Any File")) + _T(" (*.*)|*.*")  );
	  if ( pic.ShowModal() == wxID_OK )
		  usync_ctrl->SetValue( pic.GetPath() );
}

void PathOptionPanel::UsePaths(wxCommandEvent& /*unused*/)
{
	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(),  usync_ctrl->GetValue() );
    LSL::usync().ReloadUnitSyncLib();

    if ( !(LSL::susynclib().IsLoaded()) )
	{
		customMessageBox(SS_MAIN_ICON, _("SpringSettings is unable to load your unitsync library.\n\
				You might want to take another look at your unitsync setting.\n\
				Your Spring version has to be 0.76 or newer, otherwise \n\
				this will fail in any case."), _("Spring error"), wxOK );
	}
	else
	{
		origin->buildGuiFromErrorPanel();
		sett().SaveSettings();
	}
}

PathOptionPanel::~PathOptionPanel()
{
}


BEGIN_EVENT_TABLE(PathOptionPanel, wxPanel)
	EVT_BUTTON ( ID_PATH_USYNC_BTN, PathOptionPanel::SetUsyncPath )
	EVT_BUTTON ( ID_PATH_OK_BTN, PathOptionPanel::UsePaths )
END_EVENT_TABLE()

