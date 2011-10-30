#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include "maindownloadtab.h"

#ifndef NO_TORRENT_SYSTEM

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

#include "downloader/downloadlistctrl.h"
#include "utils/conversion.h"
#include "utils/controls.h"
#include "Helper/colorbutton.h"
//#include "downloader/filelister/filelistdialog.h"
#include "downloader/prdownloader.h"
#include "widgets/downloaddialog.h"
#include "aui/auimanager.h"

BEGIN_EVENT_TABLE( MainTorrentTab, wxPanel )
	//(*EventTable(MainTorrentTab)
	//*)
	EVT_BUTTON      ( ID_BUTTON_CANCEL,     MainTorrentTab::OnCancelButton      )
	EVT_BUTTON      ( ID_BUTTON_CLEAR,      MainTorrentTab::OnClearFinished     )
//	EVT_BUTTON      ( ID_DOWNLOAD_DIALOG,   MainTorrentTab::OnDownloadDialog    )
	EVT_BUTTON      ( ID_BUTTON_WIDGETS,    MainTorrentTab::OnDLWidgets         )

END_EVENT_TABLE()

MainTorrentTab::MainTorrentTab( wxWindow* parent )
    : wxScrolledWindow( parent ),
    m_widgets_dialog( NULL )
{
	GetAui().manager->AddPane( this, wxLEFT, _T( "maintorrenttab" ) );

	m_mainbox = new wxBoxSizer ( wxVERTICAL );

	wxBoxSizer* m_listbox = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer* m_totalbox = new wxBoxSizer ( wxHORIZONTAL );
	wxBoxSizer* m_buttonbox = new wxBoxSizer ( wxHORIZONTAL );
	wxBoxSizer* m_status_box = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* m_firstrow_box = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* m_list_lbl = new wxStaticText( this, ID_OUTGOING_LBL, _( "Transfers in progress: " ) );
	m_listbox->Add( m_list_lbl, 0, wxALL, 5 );
	m_torrent_list = new TorrentListCtrl( this );
	m_listbox->Add( m_torrent_list, 2, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 5 );
	m_mainbox->Add( m_listbox, 2, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 0 );

	m_incoming_lbl = new wxStaticText( this, ID_INCOMING_LBL, _( "Total Incoming: " ) );
	m_totalbox->Add( m_incoming_lbl, 1, wxALL | wxALIGN_CENTER_HORIZONTAL, 10 );

	m_firstrow_box->Add( m_totalbox, 0, wxALL, 5  );

	m_status_color = new ColorButton( this, wxID_ANY, wxBitmap(), wxDefaultPosition, wxSize( 20, 20 ), 0 );
	m_status_color_text = new wxStaticText( this, wxID_ANY, _( "unknown" ) );
	m_status_box->Add( m_status_color ,  0, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 10 );
	m_status_box->Add( m_status_color_text, 0,  wxALL | wxALIGN_CENTER_VERTICAL, 10 );
	m_firstrow_box->Add( m_status_box, 1, wxALL, 5 );

	m_mainbox->Add( m_firstrow_box, 0, wxALL, 5 );

	m_but_cancel = new wxButton( this, ID_BUTTON_CANCEL, _( "Cancel Download" ) );
	//m_but_cancel->Disable();
	m_buttonbox->Add( m_but_cancel, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
	m_but_clear = new wxButton( this, ID_BUTTON_CLEAR, _( "Clear finished" ) );
	m_buttonbox->Add( m_but_clear, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
//	m_but_publish = new wxButton( this, ID_BUTTON_PUB, _( "Publish new file" ) );
//	m_buttonbox->Add( m_but_publish, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
//	m_but_download = new wxButton( this, ID_DOWNLOAD_DIALOG, _( "Search file" ) );
//	m_buttonbox->Add( m_but_download, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
	m_but_widgets = new wxButton( this, ID_BUTTON_WIDGETS, _( "Download Lua widgets" ) );
	m_buttonbox->Add( m_but_widgets, 1, wxALL | wxALIGN_RIGHT | wxALIGN_BOTTOM, 5 );

	m_mainbox->Add( m_buttonbox, 0, wxALL, 5 );

	SetSizer( m_mainbox );
	SetScrollRate( SCROLL_RATE, SCROLL_RATE );

//	info_map = torrent().CollectGuiInfos();

//	m_torrent_list->Layout();

//	for ( map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter )
//	{
//	    if (iter->first == wxString(_T("global")))
//            continue;
//		m_torrent_list->AddTorrentInfo( iter->second );
//	}

    Layout();
}

MainTorrentTab::~MainTorrentTab()
{}

void MainTorrentTab::OnClearFinished( wxCommandEvent& /*event*/ )
{
    prDownloader().ClearFinished();
    m_torrent_list->Clear();
}

void MainTorrentTab::OnDLWidgets( wxCommandEvent& /*unused*/ )
{
	if ( m_widgets_dialog && m_widgets_dialog->IsShown() ) {
		m_widgets_dialog->SetFocus();
	}
	else {
		m_widgets_dialog = new WidgetDownloadDialog( this, wxID_ANY, _( "Lua widget downloader" ) );
		m_widgets_dialog->Show( true );
	}
}

void MainTorrentTab::OnUpdate()
{

}


void MainTorrentTab::OnCancelButton( wxCommandEvent& /*unused*/ )
{

}

void MainTorrentTab::OnDownloadDialog( wxCommandEvent& /*unused*/ )
{}

#endif
