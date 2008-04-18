#include "maintorrenttab.h"


#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include "torrentlistctrl.h"
#include "torrentwrapper.h"
#include "ui.h"

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
	//*)
}

MainTorrentTab::~MainTorrentTab()
{
	//(*Destroy(MainTorrentTab)
	//*)
}

void MainTorrentTab::AddTorrentInfo( TorrentInfos& info )
{
  int index = m_torrent_list->InsertItem( 0, info.name );
  ASSERT_LOGIC( index != -1, _T("index = -1") );
  m_battle_list->SetItemData(index, (long)battle.GetBattleId() );
  battle.SetGUIListActiv( true );

  ASSERT_LOGIC( index != -1, _T("index = -1") );
  //wxListItem item;
  //item.SetId( index );

 // ASSERT_LOGIC( m_battle_list->GetItem( item ), _T("!GetItem") );

  m_battle_list->SetItemImage( index, icons().GetBattleStatusIcon( battle ) );
  m_battle_list->SetItemColumnImage( index, 2, icons().GetRankIcon( battle.GetRankNeeded(), false ) );
  m_battle_list->SetItemColumnImage( index, 1, icons().GetFlagIcon( battle.GetFounder().GetCountry() ) );
  m_battle_list->SetItem( index, 3, battle.GetDescription() );
  m_battle_list->SetItem( index, 4, RefineMapname( battle.GetMapName() ), battle.MapExists()?icons().ICON_EXISTS:icons().ICON_NEXISTS );
  m_battle_list->SetItem( index, 5, RefineModname( battle.GetModName() ), battle.ModExists()?icons().ICON_EXISTS:icons().ICON_NEXISTS );
  m_battle_list->SetItem( index, 6, battle.GetFounder().GetNick() );
  m_battle_list->SetItem( index, 7, wxString::Format(_T("%d"), battle.GetSpectators()) );
  m_battle_list->SetItem( index, 8, wxString::Format(_T("%d"), battle.GetNumUsers() - battle.GetSpectators() ) );
  m_battle_list->SetItem( index, 9, wxString::Format(_T("%d"), battle.GetMaxPlayers()) );

  m_battle_list->Sort();
}
