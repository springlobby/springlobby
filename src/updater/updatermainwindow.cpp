/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "updatermainwindow.h"

#include <wx/sizer.h>
#include <wx/icon.h>

#include "gui/activitynotice.h"
#include "images/springlobby12x12.xpm"

class UpdaterPanel : public wxPanel
{

private:
	wxBoxSizer* m_main_sizer;
	ActivityNoticePanel* m_activity_panel;

	DECLARE_EVENT_TABLE()

public:
	UpdaterPanel ( wxWindow* parent)
		: wxPanel( parent, -1 ) {
		m_main_sizer = new wxBoxSizer( wxVERTICAL );
		m_activity_panel = new ActivityNoticePanel( this,
				wxString::Format ( _T("Updating SpringLobby, please wait.")),
				wxSize(450, 60) , wxSize(420, 15)  );
		m_main_sizer->Add( m_activity_panel, 0, wxALL, 0 );

		SetSizer( m_main_sizer);
		Layout();
	}
};

BEGIN_EVENT_TABLE( UpdaterPanel, wxPanel )
//    EVT_BUTTON ( ID_BUT_CHANGELOG, UpdaterPanel::OnChangelog )
END_EVENT_TABLE()


BEGIN_EVENT_TABLE( UpdaterMainwindow, wxFrame )
	EVT_CLOSE( UpdaterMainwindow::OnClose )
END_EVENT_TABLE()

/** @brief ~UpdaterMainwindow
  *
  * @todo: document this function
  */
UpdaterMainwindow::~UpdaterMainwindow()
{

}

/** @brief UpdaterMainwindow
  *
  * @todo: document this function
  */
UpdaterMainwindow::UpdaterMainwindow():
	wxFrame( NULL, -1, _("Springlobby Updater"), wxPoint(150, 150), wxSize(450, 120) )
{
	SetIcon( wxIcon(springlobby12x12_xpm) );

	wxBoxSizer* top_sizer = new wxBoxSizer( wxVERTICAL );
	UpdaterPanel* panel = new UpdaterPanel(this);

	top_sizer->Add( panel, 1, wxEXPAND|wxALL, 0 );

	SetSizer( top_sizer );
	Layout();
	Center();
}

/** @brief OnClose
  *
  * @todo: document this function
  */
void UpdaterMainwindow::OnClose(wxCloseEvent&)
{
	Destroy();
}

void UpdaterMainwindow::OnUpdateFinished( wxCommandEvent&/*data*/ )
{
	Destroy();
}
