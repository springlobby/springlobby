#include "mapselectdialog.h"
#include "settings.h"

//(*InternalHeaders(MapSelectDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include "mapgridctrl.h"
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(MapSelectDialog)
const long MapSelectDialog::ID_STATICTEXT2 = wxNewId();
const long MapSelectDialog::ID_VERTICAL = wxNewId();
const long MapSelectDialog::ID_STATICTEXT1 = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL = wxNewId();
const long MapSelectDialog::ID_MAPGRID = wxNewId();
//*)

BEGIN_EVENT_TABLE(MapSelectDialog,wxDialog)
	//(*EventTable(MapSelectDialog)
	//*)
END_EVENT_TABLE()

MapSelectDialog::MapSelectDialog(wxWindow* parent,Ui& ui)
	: m_ui(ui)
{
	//(*Initialize(MapSelectDialog)
	wxStaticText* StaticText2;
	wxStaticText* StaticText1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Select map"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Vertical sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer2->Add(StaticText2, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_vertical_choice = new wxChoice(this, ID_VERTICAL, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_VERTICAL"));
	BoxSizer2->Add(m_vertical_choice, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Horizontal sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_horizontal_choice = new wxChoice(this, ID_HORIZONTAL, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_HORIZONTAL"));
	BoxSizer2->Add(m_horizontal_choice, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_APPLY, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer2->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_mapgrid = new MapGridCtrl(this, m_ui, wxSize(400,400));
	BoxSizer1->Add(m_mapgrid, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_VERTICAL,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_HORIZONTAL,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&MapSelectDialog::OnInit);
	//*)
}

MapSelectDialog::~MapSelectDialog()
{
	//(*Destroy(MapSelectDialog)
	//*)
}


void MapSelectDialog::OnInit( wxInitDialogEvent& event )
{
	AppendSortKeys( m_horizontal_choice );
	AppendSortKeys( m_vertical_choice );

	m_horizontal_choice->SetSelection( 0 );
	m_vertical_choice->SetSelection( 0 );

	m_mapgrid->LoadMaps();
	Sort();
}


void MapSelectDialog::OnSortKeySelect( wxCommandEvent& event )
{
	Sort();
}


void MapSelectDialog::AppendSortKeys( wxChoice* choice )
{
	choice->Append( _("Name"), (void*) MapGridCtrl::SortKey_Name );
	choice->Append( _("Size"), (void*) MapGridCtrl::SortKey_Area );
	choice->Append( _("Number of start positions"), (void*) MapGridCtrl::SortKey_PosCount );
}


static MapGridCtrl::SortKey GetSelectedSortKey( wxChoice* choice )
{
	return (MapGridCtrl::SortKey) (int) choice->GetClientData( choice->GetSelection() );
}

void MapSelectDialog::Sort()
{
	m_mapgrid->Sort( GetSelectedSortKey( m_vertical_choice ), GetSelectedSortKey( m_horizontal_choice ) );
}
