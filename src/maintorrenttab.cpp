#include "maintorrenttab.h"


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
END_EVENT_TABLE()

MainTorrentTab::MainTorrentTab(wxWindow* parent, Ui& ui)
    : wxPanel(parent), m_ui(ui)
{
	//(*Initialize(MainTorrentTab)
	wxGridSizer* GridSizer4;
	wxGridSizer* GridSizer5;
	wxGridSizer* GridSizer6;
	wxGridSizer* GridSizer1;
	wxGridSizer* GridSizer3;
	wxGridSizer* GridSizer2;

	GridSizer1 = new wxGridSizer(2, 1, 0, 0);
	GridSizer3 = new wxGridSizer(0, 3, 0, 0);
	GridSizer5 = new wxGridSizer(0, 3, 0, 0);
	m_outgoing_lbl = new wxStaticText( this, ID_OUTGOING_LBL, _("Total Outgoing: ") );

	GridSizer5->Add(m_outgoing_lbl, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer3->Add(GridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer6 = new wxGridSizer(0, 3, 0, 0);
	m_incoming_lbl = new wxStaticText( this, ID_INCOMING_LBL, _("Total Incoming: ") );
	GridSizer6->Add(m_incoming_lbl, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer3->Add(GridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1->Add(GridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer4 = new wxGridSizer(1, 1, 0, 0);
	m_torrent_list = new TorrentListCtrl(this, m_ui);
	GridSizer4->Add( m_torrent_list, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1->Add(GridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer2 = new wxGridSizer(0, 3, 0, 0);
	m_but_cancel= new wxButton(this, ID_BUTTON_CANCEL, _("Cancel Download") );
	m_but_cancel->Disable();
	GridSizer2->Add(m_but_cancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer2->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_but_publish = new wxButton(this, ID_BUTTON_PUB, _("Publish new file") );
	GridSizer2->Add( m_but_publish, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1->Add(GridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(GridSizer1);
	GridSizer1->SetSizeHints(this);

    info_map = torrent()->CollectGuiInfos();
    m_torrent_list->SetInfoMap( &info_map );

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
    if ( info.filehash == (int)m_torrent_list->GetItemData( i ) ) {
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
 int eta_seconds = -1;
 if ( info.progress > 0 && info.inspeed > 0)
    eta_seconds = int ( ( info.downloaded / info.progress ) / info.inspeed );

 // m_torrent_list->SetItemImage( index, icons().GetBattleStatusIcon( battle ) );
  m_torrent_list->SetItem( index, 0, info.name );
  m_torrent_list->SetItem( index, 1, i2s( info.numcopies ) );
  m_torrent_list->SetItem( index, 2, i2s( info.downloaded ) );
  m_torrent_list->SetItem( index, 3, i2s( info.uploaded ) );
  m_torrent_list->SetItem( index, 4, i2s( info.leeching ) );
  m_torrent_list->SetItem( index, 5, i2s( info.progress ) );
  m_torrent_list->SetItem( index, 6, i2s( info.outspeed ) );
  m_torrent_list->SetItem( index, 7, i2s( info.inspeed ) );
  m_torrent_list->SetItem( index, 8, (eta_seconds > -1 ? i2s(eta_seconds) : _T("inf.") ) );

  m_torrent_list->Sort();
}

void MainTorrentTab::AddTorrentInfo( const TorrentInfos& info )
{
  int index = m_torrent_list->InsertItem( 0, info.name );
//  ASSERT_LOGIC( index != -1, _T("index = -1") );
  m_torrent_list->SetItemData(index, (long)info.filehash );


  //ASSERT_LOGIC( index != -1, _T("index = -1") );

 // ASSERT_LOGIC( m_torrent_list->GetItem( item ), _T("!GetItem") );
    SetInfo(index, info );
}

void MainTorrentTab::OnUpdate()
{
    info_map = torrent()->CollectGuiInfos();
    for (map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter)
    {
        UpdateInfo(iter->second);

    }
}
