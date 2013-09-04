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
#include "utils/controls.h"
#include "helper/colorbutton.h"
//#include "downloader/filelister/filelistdialog.h"
#include "downloader/downloadsobserver.h"
#include "aui/auimanager.h"
#include "gui/contentdownloaddialog.h"

BEGIN_EVENT_TABLE( MainDownloadTab, wxPanel )
	//(*EventTable(MainTorrentTab)
	//*)
	EVT_BUTTON      ( ID_BUTTON_CANCEL,     MainDownloadTab::OnCancelButton      )
	EVT_BUTTON      ( ID_BUTTON_CLEAR,      MainDownloadTab::OnClearFinished     )
	EVT_BUTTON      ( ID_DOWNLOAD_DIALOG,   MainDownloadTab::OnDownloadDialog    )
END_EVENT_TABLE()

MainDownloadTab::MainDownloadTab( wxWindow* parent )
    : wxScrolledWindow( parent ),
      m_widgets_dialog( NULL ),
      m_download_dialog( NULL )
{
	GetAui().manager->AddPane( this, wxLEFT, _T( "maintorrenttab" ) );

	m_mainbox = new wxBoxSizer ( wxVERTICAL );

	wxBoxSizer* m_buttonbox = new wxBoxSizer ( wxHORIZONTAL );

	m_dl_listctrl = new DownloadListCtrl( this );
	m_mainbox->Add( m_dl_listctrl, 2, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 0 );

	m_but_cancel = new wxButton( this, ID_BUTTON_CANCEL, _( "Cancel Download" ) );
	//m_but_cancel->Disable();
	m_buttonbox->Add( m_but_cancel, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
	m_but_clear = new wxButton( this, ID_BUTTON_CLEAR, _( "Clear finished" ) );
	m_buttonbox->Add( m_but_clear, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
	m_but_download = new wxButton( this, ID_DOWNLOAD_DIALOG, _( "Search file" ) );
	m_buttonbox->Add( m_but_download, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
//	m_but_widgets = new wxButton( this, ID_BUTTON_WIDGETS, _( "Download Lua widgets" ) );
//	m_buttonbox->Add( m_but_widgets, 1, wxALL | wxALIGN_RIGHT | wxALIGN_BOTTOM, 5 );

	m_mainbox->Add( m_buttonbox, 0, wxALL, 5 );

	SetSizer( m_mainbox );
	SetScrollRate( SCROLL_RATE, SCROLL_RATE );

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
}

MainDownloadTab::~MainDownloadTab()
{}

void MainDownloadTab::OnClearFinished( wxCommandEvent& /*event*/ )
{
    downloadsObserver().ClearFinished();

	m_dl_listctrl->Clear();
}

void MainDownloadTab::OnUpdate()
{
    m_dl_listctrl->UpdateTorrentsList();
}


void MainDownloadTab::OnCancelButton( wxCommandEvent& /*unused*/ )
{

}

void MainDownloadTab::OnDownloadDialog( wxCommandEvent& /*unused*/ )
{
    if ( m_download_dialog && m_download_dialog->IsShown() )
    {
	m_download_dialog->SetFocus();
    }else{


      m_download_dialog = new ContentDownloadDialog(this,wxID_ANY,_("Search for maps and games") );
      m_download_dialog->Show(true);
    }
}

