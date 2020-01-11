/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: MainOptionsTab
//
#include "mainoptionstab.h"

#include <wx/button.h>
#include <wx/icon.h>
#include <wx/imaglist.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/notebook.h>
#include <wx/sizer.h>

#include "aui/auimanager.h"
#include "chatoptionstab.h"
#include "downloadoptionspanel.h"
#include "groupoptionspanel.h"
#include "gui/controls.h"
#include "gui/slbook.h"
#include "gui/uiutils.h"
#include "images/spring.xpm"
#include "images/springlobby12x12.xpm"
#include "images/torrentoptionspanel_icon.png.h"
#include "images/userchat.xpm"
#include "lobbyoptionstab.h"
#include "log.h"
#include "settings.h"
#include "springoptionstab.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/version.h"

BEGIN_EVENT_TABLE(MainOptionsTab, wxPanel)

EVT_BUTTON(wxID_APPLY, MainOptionsTab::OnApply)
EVT_BUTTON(wxID_REVERT, MainOptionsTab::OnRestore)
EVT_BUTTON(wxID_OK, MainOptionsTab::OnOk)

END_EVENT_TABLE()

/** \brief A container for the various option panels
 * Contains a notebook holding the real option panels as pages. Handles "apply" and "restore" events for those pages,
 * rather then those having to implement (and duplicate) this functionality. \n
 * See SpringOptionsTab, TorrentOptionsPanel, ChatOptionsTab
 */
MainOptionsTab::MainOptionsTab(wxWindow* parent)
    : wxScrolledWindow(parent, -1)
{
	frame = parent;
	GetAui().manager->AddPane(this, wxLEFT, _T("mainoptionstab"));
	m_tabs = new SLNotebook(this, _T("mainoptionstab"), OPTIONS_TABS, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE);
	m_tabs->SetArtProvider(new wxAuiDefaultTabArt);

	m_spring_opts = new SpringOptionsTab(m_tabs);
	m_tabs->AddPage(m_spring_opts, _("Spring"), true, wxIcon(spring_xpm));

	m_download_opts = new DownloadOptionsPanel(m_tabs);
	m_tabs->AddPage(m_download_opts, _("Downloads"), true, charArr2wxBitmap(torrentoptionspanel_icon_png, sizeof(torrentoptionspanel_icon_png)));

	m_chat_opts = new ChatOptionsTab(m_tabs);
	m_tabs->AddPage(m_chat_opts, _("Chat"), true, wxIcon(userchat_xpm));

	m_lobby_opts = new LobbyOptionsTab(m_tabs);
	m_tabs->AddPage(m_lobby_opts, _("General"), true, wxIcon(springlobby12x12_xpm));

	m_groups_opts = new GroupOptionsPanel(m_tabs);
	m_tabs->AddPage(m_groups_opts, _("Groups"), true, wxIcon(userchat_xpm));

	m_apply_btn = new wxButton(this, wxID_APPLY, _("Apply"));
	m_cancel_btn = new wxButton(this, wxID_CANCEL, _("Cancel"));
	m_ok_btn = new wxButton(this, wxID_OK, _("Ok"));

	m_button_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_button_sizer->AddStretchSpacer();
	m_button_sizer->Add(m_apply_btn, 0, wxALL, 2);
	m_button_sizer->Add(m_cancel_btn, 0, wxALL, 2);
	m_button_sizer->Add(m_ok_btn, 0, wxALL, 2);

	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_main_sizer->Add(m_tabs, 1, wxEXPAND);
	m_main_sizer->Add(m_button_sizer, 0, wxEXPAND);

	SetSizer(m_main_sizer);
	SetScrollRate(SCROLL_RATE, SCROLL_RATE);
	SetSelection(0); //activate first tab
	Layout();
	Refresh();
}


MainOptionsTab::~MainOptionsTab()
{
	//   if ( sett().GetAutosavePerspective() )
	//        SavePerspective();
}


GroupOptionsPanel& MainOptionsTab::GetGroupOptionsPanel()
{
	ASSERT_EXCEPTION(m_groups_opts != 0, _T( "m_groups_opts == 0" ));
	return *m_groups_opts;
}

void MainOptionsTab::OnApply(wxCommandEvent& event)
{
	m_spring_opts->OnApply(event);
	m_chat_opts->OnApply(event);
	m_download_opts->OnApply(event);
	m_lobby_opts->OnApply(event);

	sett().SaveSettings();

	GlobalEventManager::Instance()->Send(GlobalEventManager::OnUnitsyncReloaded);
}

void MainOptionsTab::OnOk(wxCommandEvent& event)
{
	OnApply(event);
	frame->Close();
}

void MainOptionsTab::OnRestore(wxCommandEvent& event)
{
	m_spring_opts->OnRestore(event);
	m_chat_opts->OnRestore(event);
	m_download_opts->OnRestore(event);
	m_lobby_opts->OnRestore(event);
	frame->Close();
}

void MainOptionsTab::OnOpenGroupsTab()
{
	//m_groups_opts->ReloadGroupSizer();
}

void MainOptionsTab::SetSelection(const unsigned int page)
{
	if (page < m_tabs->GetPageCount()) {
		m_tabs->SetSelection(page);
		//m_groups_opts->ReloadGroupSizer();
	} else
		m_tabs->SetSelection(0);
}


void MainOptionsTab::LoadPerspective(const wxString& perspective_name)
{
	LoadNotebookPerspective(m_tabs, perspective_name);
}

void MainOptionsTab::SavePerspective(const wxString& perspective_name)
{
	SaveNotebookPerspective(m_tabs, perspective_name);
}

OptionsDialog::OptionsDialog(wxWindow* parent)
    : wxDialog(parent, -1, wxString::Format(_("%s Preferences"), GetSpringlobbyName()),
	    wxDefaultPosition, wxSize(1055, 620),
	    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
    , WindowAttributesPickle(_T("preferences"), this, wxSize(1055, 620))
{
	SetMinSize(wxSize(1055, 620));
	m_options = new MainOptionsTab(this);
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_main_sizer->Add(m_options, 1, wxEXPAND | wxALL, 0);
	SetSizer(m_main_sizer);
	Layout();
}

void OptionsDialog::SetSelection(const unsigned int page)
{
	m_options->SetSelection(page);
}

bool OptionsDialog::Show(bool show)
{
	if (!show)
		SaveAttributes();
	return wxDialog::Show(show);
}
