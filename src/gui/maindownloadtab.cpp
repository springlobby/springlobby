/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include "maindownloadtab.h"

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

#include "gui/downloaddataviewctrl.h"
#include "downloader/prdownloader.h"
#include "utils/conversion.h"
#include "gui/controls.h"
#include "gui/colorbutton.h"
#include "aui/auimanager.h"
#include "gui/contentdownloaddialog.h"
#include "utils/slpaths.h"
#include "utils/globalevents.h"
#include "gui/customdialogs.h"

BEGIN_EVENT_TABLE(MainDownloadTab, wxPanel)
//(*EventTable(MainTorrentTab)
//*)
	EVT_BUTTON(ID_BUTTON_CANCEL, MainDownloadTab::OnCancelButton)
	EVT_BUTTON(ID_BUTTON_CLEAR, MainDownloadTab::OnClearFinished)
	EVT_BUTTON(ID_DOWNLOAD_DIALOG, MainDownloadTab::OnDownloadDialog)
END_EVENT_TABLE()

MainDownloadTab::MainDownloadTab(wxWindow* parent)
    : wxPanel(parent)
    , m_widgets_dialog(NULL)
    , m_download_dialog(NULL)
{
	GetAui().manager->AddPane(this, wxLEFT, _T( "maintorrenttab" ));

	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* m_buttonbox = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Manage downloads"));

	m_DownloadDataView = new DownloadDataViewCtrl(_T("DownloadDataViewCtrl_MainDownloadTab"), this);
	m_main_sizer->Add(m_DownloadDataView, 2, wxALL | wxEXPAND, 0);

	m_but_cancel = new wxButton(this, ID_BUTTON_CANCEL, _("Cancel Download"));
	m_buttonbox->Add(m_but_cancel, 0, wxALL | wxALIGN_BOTTOM, 5);
	m_but_clear = new wxButton(this, ID_BUTTON_CLEAR, _("Clear finished"));
	m_buttonbox->Add(m_but_clear, 0, wxALL | wxALIGN_BOTTOM, 5);
	m_but_download = new wxButton(this, ID_DOWNLOAD_DIALOG, _("Search file"));
	m_buttonbox->Add(m_but_download, 0, wxALL | wxALIGN_BOTTOM, 5);

	wxStaticBoxSizer* currDownloadDirSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Current download directory"));
	wxString downloadDir = TowxString(SlPaths::GetDownloadDir());
	wxStaticText* m_currDownloadDirText = new wxStaticText(this, wxID_ANY, downloadDir);
	currDownloadDirSizer->Add(m_currDownloadDirText, 1, wxALL | wxEXPAND, 5);

	wxBoxSizer* manageAndInfoSizer = new wxBoxSizer(wxHORIZONTAL);
	manageAndInfoSizer->Add(m_buttonbox, 0, wxALL | wxEXPAND, 5);
	manageAndInfoSizer->Add(currDownloadDirSizer, 1, wxALL | wxEXPAND, 5);

	m_main_sizer->Add(manageAndInfoSizer, 0, wxALL | wxEXPAND, 5);

	SetSizer(m_main_sizer);

	Layout();

	GlobalEventManager::Instance()->Subscribe(this, GlobalEventManager::OnDownloadFailed, wxObjectEventFunction(&MainDownloadTab::OnDownloadFailed));
}

MainDownloadTab::~MainDownloadTab()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);
}

void MainDownloadTab::OnClearFinished(wxCommandEvent& /*event*/)
{
	m_DownloadDataView->ClearFinished();
}

void MainDownloadTab::OnCancelButton(wxCommandEvent& /*unused*/)
{
	//TODO: implement download cancellation in ContentManager
}

void MainDownloadTab::OnDownloadDialog(wxCommandEvent& /*unused*/)
{
	if (m_download_dialog && m_download_dialog->IsShown()) {
		m_download_dialog->SetFocus();
	} else {
		m_download_dialog = new ContentDownloadDialog(this, wxID_ANY, _("Search for maps and games"));
		m_download_dialog->Show(true);
	}
}

void MainDownloadTab::OnDownloadFailed(wxCommandEvent& /*event*/) {
	customMessageBox(SL_MAIN_ICON, _("Failed to download selected item."), _("Failed to download"), wxOK);
}
