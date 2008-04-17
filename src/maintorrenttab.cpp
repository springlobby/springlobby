#include "MainTorrentTab.h"

//(*InternalHeaders(MainTorrentTab)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(MainTorrentTab)
const long MainTorrentTab::ID_STATICTEXT2 = wxNewId();
const long MainTorrentTab::ID_STATICTEXT1 = wxNewId();
const long MainTorrentTab::ID_LISTBOX1 = wxNewId();
const long MainTorrentTab::ID_BUTTON1 = wxNewId();
const long MainTorrentTab::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainTorrentTab,wxFrame)
	//(*EventTable(MainTorrentTab)
	//*)
END_EVENT_TABLE()

MainTorrentTab::MainTorrentTab(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(MainTorrentTab)
	wxGridSizer* GridSizer4;
	wxGridSizer* GridSizer5;
	wxGridSizer* GridSizer6;
	wxGridSizer* GridSizer1;
	wxGridSizer* GridSizer3;
	wxGridSizer* GridSizer2;
	
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxSize(1092,262));
	GridSizer1 = new wxGridSizer(2, 1, 0, 0);
	GridSizer3 = new wxGridSizer(0, 3, 0, 0);
	GridSizer5 = new wxGridSizer(0, 3, 0, 0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Total Outgoing: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	GridSizer5->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer3->Add(GridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer6 = new wxGridSizer(0, 3, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Total Incoming: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	GridSizer6->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer3->Add(GridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1->Add(GridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer4 = new wxGridSizer(1, 1, 0, 0);
	ListBox1 = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	GridSizer4->Add(ListBox1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1->Add(GridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer2 = new wxGridSizer(0, 3, 0, 0);
	Button1 = new wxButton(this, ID_BUTTON1, _("Cancel Download"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Button1->Disable();
	GridSizer2->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer2->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON2, _("Publish new file"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	GridSizer2->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

