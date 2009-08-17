#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include "maintorrenttab.h"

#ifndef NO_TORRENT_SYSTEM

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

#include "torrentlistctrl.h"
#include "torrentwrapper.h"
#include "ui.h"
#include "utils/conversion.h"
#include "Helper/colorbutton.h"
#include "filelister/filelistdialog.h"
#include "widgets/downloaddialog.h"
#include "aui/auimanager.h"

BEGIN_EVENT_TABLE( MainTorrentTab, wxPanel )
	//(*EventTable(MainTorrentTab)
	//*)
	EVT_BUTTON      ( ID_BUTTON_CANCEL,     MainTorrentTab::OnCancelButton      )
	EVT_BUTTON      ( ID_DOWNLOAD_DIALOG,   MainTorrentTab::OnDownloadDialog    )
	EVT_BUTTON      ( ID_BUTTON_WIDGETS,    MainTorrentTab::OnDLWidgets         )

END_EVENT_TABLE()

MainTorrentTab::MainTorrentTab( wxWindow* parent, Ui& ui )
    : wxScrolledWindow( parent ),
    m_widgets_dialog( NULL ),
    m_ui( ui )
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
	m_torrent_list = new TorrentListCtrl( this, m_ui );
	m_listbox->Add( m_torrent_list, 2, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 5 );
	m_mainbox->Add( m_listbox, 2, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 0 );

	m_outgoing_lbl = new wxStaticText( this, ID_OUTGOING_LBL, _( "Total Outgoing: " ) );
	m_incoming_lbl = new wxStaticText( this, ID_INCOMING_LBL, _( "Total Incoming: " ) );
	m_totalbox->Add( m_outgoing_lbl, 1, wxALL | wxALIGN_CENTER_HORIZONTAL, 10 );
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
	m_but_publish = new wxButton( this, ID_BUTTON_PUB, _( "Publish new file" ) );
	m_buttonbox->Add( m_but_publish, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
	m_but_download = new wxButton( this, ID_DOWNLOAD_DIALOG, _( "Search file" ) );
	m_buttonbox->Add( m_but_download, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 5 );
	m_but_widgets = new wxButton( this, ID_BUTTON_WIDGETS, _( "Download Lua widgets" ) );
	m_buttonbox->Add( m_but_widgets, 1, wxALL | wxALIGN_RIGHT | wxALIGN_BOTTOM, 5 );

	m_mainbox->Add( m_buttonbox, 0, wxALL, 5 );

	SetSizer( m_mainbox );
	m_mainbox->SetSizeHints( this );
	Layout();

	info_map = torrent().CollectGuiInfos();
	m_torrent_list->SetInfoMap( &info_map );
//    m_torrent_list->SetSizeHints(this);
	m_torrent_list->Layout();

	for ( map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter )
	{
		AddTorrentInfo( iter->second );
	}
	m_download_dialog = 0;
}

MainTorrentTab::~MainTorrentTab()
{
	if ( m_download_dialog )
	{
		delete m_download_dialog;
		m_download_dialog = 0;
	}
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

void MainTorrentTab::UpdateInfo(  TorrentInfos& info )
{
	int index = -1;
	for ( int i = 0; i < m_torrent_list->GetItemCount() ; i++ ) {
		if ( info.hash == TowxString( ( int )m_torrent_list->GetItemData( i ) ) ) {
			index = i;
			break;
		}
	}

	//ASSERT_LOGIC( index != -1, _T("index = -1") );
	if ( index > 0 ) {
		SetInfo( index, info );
	}
	else {
		AddTorrentInfo( info );
	}
}

void MainTorrentTab::SetInfo( int index,  TorrentInfos& info )
{

	float kfactor = 1 / float( 1024 );
	float mfactor = 1 / float( 1024 * 1024 );

	int eta_seconds = -1;
	if ( info.progress > 0 && info.inspeed > 0 )
		eta_seconds = int (  ( info.filesize - info.downloaded ) / info.inspeed );

	info.eta = eta_seconds;

// m_torrent_list->SetItemImage( index, icons().GetBattleStatusIcon( battle ) );
	m_torrent_list->SetItem( index, 0, info.name );
	m_torrent_list->SetItem( index, 1, info.numcopies > 0 ? TowxString( info.numcopies ) : wxString( _( "not available" ) ) );
	m_torrent_list->SetItem( index, 2, TowxString( info.downloaded*mfactor ) );
	m_torrent_list->SetItem( index, 3, TowxString( info.uploaded*mfactor ) );
	if ( info.downloadstatus  == P2P::seeding ) m_torrent_list->SetItem( index, 4, _( "seeding" ) );
	else if ( info.downloadstatus  == P2P::leeching ) m_torrent_list->SetItem( index, 4, _( "leeching" ) );
	else if ( info.downloadstatus  == P2P::queued ) m_torrent_list->SetItem( index, 4, _( "queued" ) );
	m_torrent_list->SetItem( index, 5, TowxString( info.progress * 100 ) );
	m_torrent_list->SetItem( index, 6, TowxString( info.outspeed*kfactor ) );
	m_torrent_list->SetItem( index, 7, TowxString( info.inspeed*kfactor ) );
	m_torrent_list->SetItem( index, 8, ( eta_seconds > -1 ? TowxString( eta_seconds ) : _T( "inf." ) ) + _T( " s" ) );
	m_torrent_list->SetItem( index, 9, wxString::Format( _T( "%.3f" ), info.filesize*mfactor ) );

	m_torrent_list->Sort();
}

void MainTorrentTab::AddTorrentInfo(  TorrentInfos& info )
{
	int index = m_torrent_list->InsertItem( m_torrent_list->GetItemCount(), info.name );

//  ASSERT_LOGIC( index != -1, _T("index = -1") );
	m_torrent_list->SetItemData( index, FromwxString<long>( info.hash ) );


	//ASSERT_LOGIC( index != -1, _T("index = -1") );

// ASSERT_LOGIC( m_torrent_list->GetItem( item ), _T("!GetItem") );

	SetInfo( index, info );

}

void MainTorrentTab::OnUpdate()
{

	if ( torrent().IsConnectedToP2PSystem() )
	{
		m_but_cancel->Enable();
		m_but_publish->Enable();
		m_but_download->Enable();
	}
	else
	{
		m_but_cancel->Disable();
		m_but_publish->Disable();
		m_but_download->Disable();
	}

	switch ( torrent().GetTorrentSystemStatus() )
	{
		case 0:
			m_status_color->SetColor( wxColour( 255, 0, 0 ) ); //not connected
			m_status_color_text->SetLabel( _( "Status: not connected" ) );
			break;
		case 1:
			m_status_color->SetColor( wxColour( 0, 255, 0 ) ); //connected
			m_status_color_text->SetLabel( _( "Status: connected" ) );
			break;
		case 2:
			m_status_color->SetColor( wxColour( 0, 0, 255 ) ); //ingame
			m_status_color_text->SetLabel( _( "Status: throttled or paused (ingame)" ) );
			break;
		default:
			m_status_color->SetColor( wxColour( 255, 255, 255 ) ); //unknown
			m_status_color_text->SetLabel( _( "Status: unknown" ) );
			break;
	}

	m_torrent_list->SetSelectionRestorePoint();
	info_map = torrent().CollectGuiInfos();
	m_outgoing_lbl->SetLabel( wxString::Format( _( "Total Outgoing: %.2f KB/s" ), ( info_map[0].outspeed / float( 1024 ) ) ) );
	m_incoming_lbl->SetLabel( wxString::Format( _( "Total Incoming: %.2f KB/s" ), ( info_map[0].inspeed / float( 1024 ) ) ) );
	m_torrent_list->DeleteAllItems();
	for ( map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter )
	{
		if ( iter->first == 0 ) continue; //skip global torrent stats
		AddTorrentInfo( iter->second );

	}
	Layout();
	m_torrent_list->RestoreSelection();

}


void MainTorrentTab::OnCancelButton( wxCommandEvent& /*unused*/ )
{
	torrent().RemoveTorrentByHash( TowxString( m_torrent_list->GetSelectedData() ) );
}

void MainTorrentTab::OnDownloadDialog( wxCommandEvent& /*unused*/ )
{
	m_download_dialog = new FileListDialog( this );
	m_download_dialog->Show();
}

#endif
