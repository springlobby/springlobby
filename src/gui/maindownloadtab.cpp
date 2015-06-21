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

#include "downloader/downloadlistctrl.h"
#include "utils/conversion.h"
#include "gui/controls.h"
#include "gui/colorbutton.h"
#include "downloader/downloadsobserver.h"
#include "aui/auimanager.h"
#include "gui/contentdownloaddialog.h"


BEGIN_EVENT_TABLE(MainDownloadTab, wxPanel)
//(*EventTable(MainTorrentTab)
//*)
EVT_BUTTON(ID_BUTTON_CANCEL, MainDownloadTab::OnCancelButton)
EVT_BUTTON(ID_BUTTON_CLEAR, MainDownloadTab::OnClearFinished)
EVT_BUTTON(ID_DOWNLOAD_DIALOG, MainDownloadTab::OnDownloadDialog)
EVT_TIMER(ID_TIMER, MainDownloadTab::OnUpdate)
END_EVENT_TABLE()

MainDownloadTab::MainDownloadTab(wxWindow* parent)
    : wxPanel(parent)
    , m_widgets_dialog(NULL)
    , m_download_dialog(NULL)
{
	GetAui().manager->AddPane(this, wxLEFT, _T( "maintorrenttab" ));

	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* m_buttonbox = new wxBoxSizer(wxHORIZONTAL);

	m_dl_listctrl = new DownloadListCtrl(this);
	m_main_sizer->Add(m_dl_listctrl, 2, wxALL | wxEXPAND, 0);

	m_but_cancel = new wxButton(this, ID_BUTTON_CANCEL, _("Cancel Download"));
	//m_but_cancel->Disable();
	m_buttonbox->Add(m_but_cancel, 1, wxALL | wxALIGN_BOTTOM, 5);
	m_but_clear = new wxButton(this, ID_BUTTON_CLEAR, _("Clear finished"));
	m_buttonbox->Add(m_but_clear, 1, wxALL | wxALIGN_BOTTOM, 5);
	m_but_download = new wxButton(this, ID_DOWNLOAD_DIALOG, _("Search file"));
	m_buttonbox->Add(m_but_download, 1, wxALL | wxALIGN_BOTTOM, 5);
	//	m_but_widgets = new wxButton( this, ID_BUTTON_WIDGETS, _( "Download Lua widgets" ) );
	//	m_buttonbox->Add( m_but_widgets, 1, wxALL | wxALIGN_RIGHT | wxALIGN_BOTTOM, 5 );

	m_main_sizer->Add(m_buttonbox, 0, wxALL, 5);

	SetSizer(m_main_sizer);

	//	info_map = torrent().CollectGuiInfos();

	//	m_dl_listctrl->Layout();

	//	for ( map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter )
	//	{
	//	    if (iter->first == wxString(_T("global")))
	//            continue;
	//		m_dl_listctrl->AddTorrentInfo( iter->second );
	//	}

	Layout();
	//Disable();

	timer = new wxTimer(this, ID_TIMER);
	timer->Start(1000 * 2);
}

MainDownloadTab::~MainDownloadTab()
{
	timer->Stop();
	wxDELETE(timer);
}

void MainDownloadTab::OnClearFinished(wxCommandEvent& /*event*/)
{
	downloadsObserver().ClearFinished();

	m_dl_listctrl->Clear();
}

void MainDownloadTab::OnUpdate(wxTimerEvent& /*event*/)
{
	m_dl_listctrl->UpdateTorrentsList();
}


void MainDownloadTab::OnCancelButton(wxCommandEvent& /*unused*/)
{
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
