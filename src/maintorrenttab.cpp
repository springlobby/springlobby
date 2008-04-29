#include "maintorrenttab.h"

#ifndef NO_TORRENT_SYSTEM

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include "torrentlistctrl.h"
#include "torrentwrapper.h"
#include "ui.h"
#include "utils.h"

//const long MainTorrentTab::ID_STATICTEXT2 = wxNewId();
//const long MainTorrentTab::ID_STATICTEXT1 = wxNewId();
//const long MainTorrentTab::ID_LISTBOX1 = wxNewId();
//const long MainTorrentTab::ID_BUTTON1 = wxNewId();
//const long MainTorrentTab::ID_BUTTON2 = wxNewId();


BEGIN_EVENT_TABLE(MainTorrentTab,wxPanel)
	//(*EventTable(MainTorrentTab)
	//*)
  EVT_BUTTON      ( ID_BUTTON_CANCEL, MainTorrentTab::OnCancelButton )
END_EVENT_TABLE()

MainTorrentTab::MainTorrentTab(wxWindow* parent, Ui& ui)
    : wxPanel(parent), m_ui(ui)
{
	m_mainbox = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* m_listbox = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer* m_totalbox = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer* m_buttonbox = new wxBoxSizer (wxHORIZONTAL);

    wxStaticText* m_list_lbl = new wxStaticText( this, ID_OUTGOING_LBL, _("Tranfers in progress: ") );
    m_listbox->Add(m_list_lbl, 0, wxBOTTOM, 5);
	m_torrent_list = new TorrentListCtrl(this, m_ui);
	m_listbox->Add( m_torrent_list, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5);
	m_mainbox->Add(m_listbox, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5);

	m_outgoing_lbl = new wxStaticText( this, ID_OUTGOING_LBL, _("Total Outgoing: ") );
    m_incoming_lbl = new wxStaticText( this, ID_INCOMING_LBL, _("Total Incoming: ") );
	m_totalbox->Add(m_outgoing_lbl, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP, 10);
	m_totalbox->Add(m_incoming_lbl, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP, 10);
	m_mainbox->Add(m_totalbox, 1, wxALL, 5);

	m_but_cancel= new wxButton(this, ID_BUTTON_CANCEL, _("Cancel Download") );
	//m_but_cancel->Disable();
	m_buttonbox->Add(m_but_cancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM, 5);
	m_but_publish = new wxButton(this, ID_BUTTON_PUB, _("Publish new file") );
	m_buttonbox->Add( m_but_publish, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM, 5);
	m_mainbox->Add(m_buttonbox, 1, wxALL, 5);

	SetSizer(m_mainbox);
	m_mainbox->SetSizeHints(this);
    Layout();

    info_map = torrent()->CollectGuiInfos();
    m_torrent_list->SetInfoMap( &info_map );
//    m_torrent_list->SetSizeHints(this);
    m_torrent_list->Layout();

	for (map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter)
    {
        AddTorrentInfo(iter->second);
    }
}

MainTorrentTab::~MainTorrentTab()
{

}

void MainTorrentTab::UpdateInfo( const TorrentInfos& info )
{
 int index = -1;
  for (int i = 0; i < m_torrent_list->GetItemCount() ; i++ ) {
    if ( info.hash == i2s((int)m_torrent_list->GetItemData( i ) ) ) {
      index = i;
      break;
    }
  }

  //ASSERT_LOGIC( index != -1, _T("index = -1") );
    if ( index > 0 )
        SetInfo(index, info );
    else
        AddTorrentInfo( info );
}

void MainTorrentTab::SetInfo(int index, const TorrentInfos& info )
{
    float kfactor = 1/float(1024);
 float mfactor = 1/float(1024*1024);

 int eta_seconds = -1;
 if ( info.progress > 0 && info.inspeed > 0)
    eta_seconds = int (  (info.filesize - info.downloaded ) / info.inspeed );

 // m_torrent_list->SetItemImage( index, icons().GetBattleStatusIcon( battle ) );
  m_torrent_list->SetItem( index, 0, info.name );
  m_torrent_list->SetItem( index, 1, info.numcopies > 0 ? f2s( info.numcopies ) : wxString(_("not available")));
  m_torrent_list->SetItem( index, 2, f2s( info.downloaded*mfactor ) );
  m_torrent_list->SetItem( index, 3, f2s( info.uploaded*mfactor ) );
  m_torrent_list->SetItem( index, 4, info.seeding == 0 ? _("no") : _("yes") );
  m_torrent_list->SetItem( index, 5, f2s( info.progress * 100 ) );
  m_torrent_list->SetItem( index, 6, f2s( info.outspeed*kfactor ) );
  m_torrent_list->SetItem( index, 7, f2s( info.inspeed*kfactor ) );
  m_torrent_list->SetItem( index, 8, (eta_seconds > -1 ? i2s(eta_seconds) : _T("inf.") ) + _T(" s") );
  m_torrent_list->SetItem( index, 9, wxString::Format(_T("%.3f"),info.filesize*mfactor) );

  m_torrent_list->Sort();
}

void MainTorrentTab::AddTorrentInfo( const TorrentInfos& info )
{
  int index = m_torrent_list->InsertItem( 0, info.name );
//  ASSERT_LOGIC( index != -1, _T("index = -1") );
  m_torrent_list->SetItemData(index, s2l(info.hash) );


  //ASSERT_LOGIC( index != -1, _T("index = -1") );

 // ASSERT_LOGIC( m_torrent_list->GetItem( item ), _T("!GetItem") );
    SetInfo(index, info );
}

void MainTorrentTab::OnUpdate()
{
    long currentselection = m_torrent_list->GetSelectedIndex();
    info_map = torrent()->CollectGuiInfos();
    m_outgoing_lbl->SetLabel( wxString::Format(_("Total Outgoing: %.2f KB/s"), (info_map[0].outspeed/float(1024)) ) );
    m_incoming_lbl->SetLabel( wxString::Format(_("Total Incoming: %.2f KB/s"), (info_map[0].inspeed/ float(1024)) ) );
    m_torrent_list->DeleteAllItems();
    for (map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter)
    {
      if (iter->first == 0) continue; ///skip global torrent stats
      AddTorrentInfo(iter->second);

    }
    Layout();
    m_torrent_list->SetItemState( currentselection, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
}


void MainTorrentTab::OnCancelButton( wxCommandEvent& event )
{
  torrent()->RemoveFile( info_map[m_torrent_list->GetSelectedIndex()].hash );
}

#endif
