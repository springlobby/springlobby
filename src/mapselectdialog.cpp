#include "mapselectdialog.h"
#include "settings.h"
#include "utils.h"

//(*InternalHeaders(MapSelectDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include "mapgridctrl.h"
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(MapSelectDialog)
const long MapSelectDialog::ID_STATICTEXT2 = wxNewId();
const long MapSelectDialog::ID_VERTICAL_CHOICE = wxNewId();
const long MapSelectDialog::ID_STATICTEXT1 = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL_CHOICE = wxNewId();
const long MapSelectDialog::ID_FILTER_ALL = wxNewId();
const long MapSelectDialog::ID_FILTER_POPULAR = wxNewId();
const long MapSelectDialog::ID_FILTER_RECENT = wxNewId();
const long MapSelectDialog::ID_FILTER_CONTAIN = wxNewId();
const long MapSelectDialog::ID_FILTER_TEXT = wxNewId();
const long MapSelectDialog::ID_MAPGRID = wxNewId();
//*)
const long MapSelectDialog::ID_VERTICAL_DIRECTION = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL_DIRECTION = wxNewId();

BEGIN_EVENT_TABLE(MapSelectDialog,wxDialog)
	//(*EventTable(MapSelectDialog)
	//*)
END_EVENT_TABLE()

MapSelectDialog::MapSelectDialog(wxWindow* parent,Ui& ui)
	: m_ui(ui)
	, m_vertical_direction(false)
	, m_horizontal_direction(false)
{
	//(*Initialize(MapSelectDialog)
	wxStaticText* StaticText2;
	wxStaticText* StaticText1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* boxSizerHorizontal;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* boxSizerVertical;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Select map"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Vertical sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer2->Add(StaticText2, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	boxSizerVertical = new wxBoxSizer(wxHORIZONTAL);
	m_vertical_choice = new wxChoice(this, ID_VERTICAL_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_VERTICAL_CHOICE"));
	boxSizerVertical->Add(m_vertical_choice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(boxSizerVertical, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Horizontal sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	boxSizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
	m_horizontal_choice = new wxChoice(this, ID_HORIZONTAL_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_HORIZONTAL_CHOICE"));
	boxSizerHorizontal->Add(m_horizontal_choice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(boxSizerHorizontal, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Show"));
	m_filter_all = new wxRadioButton(this, ID_FILTER_ALL, _("All maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_ALL"));
	StaticBoxSizer1->Add(m_filter_all, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_filter_popular = new wxRadioButton(this, ID_FILTER_POPULAR, _("Popular maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_POPULAR"));
	StaticBoxSizer1->Add(m_filter_popular, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_filter_recent = new wxRadioButton(this, ID_FILTER_RECENT, _("Recently played maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_RECENT"));
	m_filter_recent->SetValue(true);
	StaticBoxSizer1->Add(m_filter_recent, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	m_filter_contain = new wxRadioButton(this, ID_FILTER_CONTAIN, _("Containing "), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_CONTAIN"));
	BoxSizer3->Add(m_filter_contain, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_filter_text = new wxTextCtrl(this, ID_FILTER_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_TEXT"));
	BoxSizer3->Add(m_filter_text, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_APPLY, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer2->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_mapgrid = new MapGridCtrl(this, m_ui, wxSize(400,400), ID_MAPGRID);
	BoxSizer1->Add(m_mapgrid, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_VERTICAL_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_HORIZONTAL_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	m_mapgrid->Connect(ID_MAPGRID,wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&MapSelectDialog::OnMapGridLeftDClick,0,this);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&MapSelectDialog::OnInit);
	//*)

	Connect(ID_MAPGRID,MapGridCtrl::MapSelectedEvt,(wxObjectEventFunction)&MapSelectDialog::OnMapSelected,0,this);

	// Ugh.. Can not have these created by generated code because wxSmith doesn't accept a symbolic size,
	// (ie. wxSize(CONTROL_HEIGHT,CONTROL_HEIGHT)) and all Set*Size() methods don't seem to have any effect.
	m_vertical_direction_button = new wxButton(this, ID_VERTICAL_DIRECTION, _T("ᴠ"), wxDefaultPosition, wxSize(CONTROL_HEIGHT,CONTROL_HEIGHT), 0, wxDefaultValidator, _T("ID_VERTICAL_DIRECTION"));
	boxSizerVertical->Add(m_vertical_direction_button, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_horizontal_direction_button = new wxButton(this, ID_HORIZONTAL_DIRECTION, _T(">"), wxDefaultPosition, wxSize(CONTROL_HEIGHT,CONTROL_HEIGHT), 0, wxDefaultValidator, _T("ID_HORIZONTAL_DIRECTION"));
	boxSizerHorizontal->Add(m_horizontal_direction_button, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

  //<>ᴠᴧ

	Connect(ID_VERTICAL_DIRECTION, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MapSelectDialog::OnVerticalDirectionClicked);
	Connect(ID_HORIZONTAL_DIRECTION, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MapSelectDialog::OnHorizontalDirectionClicked);
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
	// see MapGridCtrl for available SortKeys
	choice->Append( _("Name"), (void*) MapGridCtrl::SortKey_Name );
	choice->Append( _("Tidal strength"), (void*) MapGridCtrl::SortKey_TidalStrength );
	choice->Append( _("Gravity"), (void*) MapGridCtrl::SortKey_Gravity );
	choice->Append( _("Metal scale"), (void*) MapGridCtrl::SortKey_MaxMetal );
	choice->Append( _("Extractor radius"), (void*) MapGridCtrl::SortKey_ExtractorRadius );
	choice->Append( _("Minimum wind"), (void*) MapGridCtrl::SortKey_MinWind );
	choice->Append( _("Maximum wind"), (void*) MapGridCtrl::SortKey_MaxWind );
	choice->Append( _("Average wind"), (void*) MapGridCtrl::SortKey_Wind );
	choice->Append( _("Size (map area)"), (void*) MapGridCtrl::SortKey_Area );
	choice->Append( _("Aspect ratio"), (void*) MapGridCtrl::SortKey_AspectRatio );
	choice->Append( _("Number of start positions"), (void*) MapGridCtrl::SortKey_PosCount );
}


static MapGridCtrl::SortKey GetSelectedSortKey( wxChoice* choice )
{
	return (MapGridCtrl::SortKey) (int) choice->GetClientData( choice->GetSelection() );
}

void MapSelectDialog::Sort()
{
	m_mapgrid->Sort( GetSelectedSortKey( m_vertical_choice ), GetSelectedSortKey( m_horizontal_choice ), m_vertical_direction, m_horizontal_direction );
}

UnitSyncMap* MapSelectDialog::GetSelectedMap() const
{
	return m_mapgrid->GetSelectedMap();
}

void MapSelectDialog::OnMapSelected( wxCommandEvent& event )
{
	wxLogDebugFunc( event.GetString() );
	//EndModal( wxID_OK );
}

void MapSelectDialog::OnVerticalDirectionClicked( wxCommandEvent& event )
{
	m_vertical_direction = !m_vertical_direction;
	m_vertical_direction_button->SetLabel( m_vertical_direction ? _T("ᴧ") : _T("ᴠ") );
	Sort();
}

void MapSelectDialog::OnHorizontalDirectionClicked( wxCommandEvent& event )
{
	m_horizontal_direction = !m_horizontal_direction;
	m_horizontal_direction_button->SetLabel( m_horizontal_direction ? _T("<") : _T(">") );
	Sort();
}

void MapSelectDialog::OnMapGridLeftDClick(wxMouseEvent& event)
{
	wxLogDebugFunc( _T("") );

	if ( m_mapgrid->GetSelectedMap() ) {
		EndModal( wxID_OK );
	}
}
