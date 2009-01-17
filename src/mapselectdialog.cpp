/* Author: Tobi Vollebregt */

#include "mapselectdialog.h"

#include "battle.h"
#include "server.h"
#include "settings.h"
#include "ui.h"
#include "uiutils.h"
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
const long MapSelectDialog::ID_FILTER_TEXT = wxNewId();
const long MapSelectDialog::ID_MAPGRID = wxNewId();
const long MapSelectDialog::ID_TIMER = wxNewId();
//*)
const long MapSelectDialog::ID_VERTICAL_DIRECTION = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL_DIRECTION = wxNewId();

BEGIN_EVENT_TABLE(MapSelectDialog,wxDialog)
	//(*EventTable(MapSelectDialog)
	//*)
END_EVENT_TABLE()

MapSelectDialog::MapSelectDialog(wxWindow* parent,Ui& ui)
	: m_ui(ui)
	, m_horizontal_direction(false)
	, m_vertical_direction(false)
	, m_state(State_Idle)
{
	//(*Initialize(MapSelectDialog)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticText* StaticText2;
	wxStaticText* StaticText1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* boxSizerHorizontal;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
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
	m_filter_all->SetToolTip(_("Shows all available maps"));
	StaticBoxSizer1->Add(m_filter_all, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_filter_popular = new wxRadioButton(this, ID_FILTER_POPULAR, _("Popular maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_POPULAR"));
	m_filter_popular->SetToolTip(_("Shows only maps which are currently being player on the server. You must be online to use this."));
	StaticBoxSizer1->Add(m_filter_popular, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_filter_recent = new wxRadioButton(this, ID_FILTER_RECENT, _("Recently played maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_RECENT"));
	m_filter_recent->SetValue(true);
	m_filter_recent->SetToolTip(_("Shows only maps you played recently. (Based on your replays.)"));
	StaticBoxSizer1->Add(m_filter_recent, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Filter"));
	m_filter_text = new wxTextCtrl(this, ID_FILTER_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_TEXT"));
	m_filter_text->SetToolTip(_("Shows only maps which contain this text in their name or description."));
	StaticBoxSizer2->Add(m_filter_text, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2->Add(StaticBoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer2->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_mapgrid = new MapGridCtrl(this, m_ui, wxSize(400,400), ID_MAPGRID);
	BoxSizer1->Add(m_mapgrid, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	m_timer.SetOwner(this, ID_TIMER);
	m_timer.Start(50, true);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_VERTICAL_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_HORIZONTAL_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_FILTER_ALL,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnFilterAllSelect);
	Connect(ID_FILTER_POPULAR,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnFilterPopularSelect);
	Connect(ID_FILTER_RECENT,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnFilterRecentSelect);
	Connect(ID_FILTER_TEXT,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MapSelectDialog::OnFilterTextChanged);
	m_mapgrid->Connect(ID_MAPGRID,wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&MapSelectDialog::OnMapGridLeftDClick,0,this);
	Connect(ID_TIMER,wxEVT_TIMER,(wxObjectEventFunction)&MapSelectDialog::OnTimerTrigger);
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
	wxLogDebugFunc( _T("") );

	AppendSortKeys( m_horizontal_choice );
	AppendSortKeys( m_vertical_choice );

	m_horizontal_choice->SetSelection( 0 );
	m_vertical_choice->SetSelection( 0 );

	m_maps = usync().GetMapList();
	usync().GetReplayList( m_replays );

	m_filter_popular->Enable( m_ui.IsConnected() );

	// due to a bug / crappy design in SpringUnitSync / unitsync itself we
	// get a replay list with one empty item when there are no replays..
	if ( m_replays.empty() || ( m_replays.size() == 1 && m_replays[0] == wxEmptyString ) ) {
		m_filter_all->SetValue( true );
		m_filter_recent->Enable( false );
		Load( State_LoadAll );
	}
	else {
		Load( State_LoadRecent );
	}
}


void MapSelectDialog::OnSortKeySelect( wxCommandEvent& event )
{
	wxLogDebugFunc( _T("") );
	UpdateSortAndFilter();
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
	return (MapGridCtrl::SortKey) (int) (long) choice->GetClientData( choice->GetSelection() );
}

namespace {
struct FilterPredicate
{
	FilterPredicate( const wxString& searchText ) : searchText(searchText.Lower()) {}
	bool operator () ( const UnitSyncMap& map ) const
	{
		return map.name.Lower().Contains( searchText )
			|| map.info.description.Lower().Contains( searchText )
			|| map.info.author.Lower().Contains( searchText );
	}
	wxString searchText;
};
}

void MapSelectDialog::UpdateSortAndFilter()
{
	FilterPredicate predicate( m_filter_text->GetValue() );
	m_mapgrid->Filter( predicate );
	m_mapgrid->Sort( GetSelectedSortKey( m_vertical_choice ), GetSelectedSortKey( m_horizontal_choice ), m_vertical_direction, m_horizontal_direction );
	m_mapgrid->CheckInBounds();
	m_mapgrid->Refresh();
}

UnitSyncMap* MapSelectDialog::GetSelectedMap() const
{
	wxLogDebugFunc( _T("") );
	return m_mapgrid->GetSelectedMap();
}

void MapSelectDialog::OnMapSelected( wxCommandEvent& event )
{
	wxLogDebugFunc( event.GetString() );
	//EndModal( wxID_OK );
}

void MapSelectDialog::OnVerticalDirectionClicked( wxCommandEvent& event )
{
	wxLogDebugFunc( _T("") );
	m_vertical_direction = !m_vertical_direction;
	m_vertical_direction_button->SetLabel( m_vertical_direction ? _T("ᴧ") : _T("ᴠ") );
	UpdateSortAndFilter();
}

void MapSelectDialog::OnHorizontalDirectionClicked( wxCommandEvent& event )
{
	wxLogDebugFunc( _T("") );
	m_horizontal_direction = !m_horizontal_direction;
	m_horizontal_direction_button->SetLabel( m_horizontal_direction ? _T("<") : _T(">") );
	UpdateSortAndFilter();
}

void MapSelectDialog::OnMapGridLeftDClick(wxMouseEvent& event)
{
	wxLogDebugFunc( _T("") );

	if ( m_mapgrid->GetSelectedMap() ) {
		EndModal( wxID_OK );
	}
}

void MapSelectDialog::OnTimerTrigger(wxTimerEvent& event)
{
	wxLogDebugFunc( _T("") );

	switch (m_state) {
		case State_Idle:
			// should never be reached because timer should have been stopped
			break;
		case State_LoadAll:
			if ( !LoadAllStep() ) Idle();
			break;
		case State_LoadPopular:
			if ( !LoadPopularStep() ) Idle();
			break;
		case State_LoadRecent:
			if ( !LoadRecentStep() ) Idle();
			break;
		default:
			ASSERT_EXCEPTION( false, _T("unknown state in MapSelectDialog::OnTimerTrigger") );
			break;
	}
}

// state transitioners

void MapSelectDialog::Idle()
{
	wxLogDebugFunc( _T("") );
	m_state = State_Idle;
	if ( m_timer.IsRunning() ) m_timer.Stop();
	UpdateSortAndFilter();
}

void MapSelectDialog::Load( State newstate )
{
	wxLogDebugFunc( _T("") );
	m_mapgrid->Clear();
	m_index = 0;
	m_state = newstate;
	bool timer_success = m_timer.Start( -1, true /* one shot */ );
	ASSERT_EXCEPTION( timer_success, _T("Failed to start timer") );
}

// implementation of the states

bool MapSelectDialog::LoadAllStep()
{
	wxLogDebugFunc( _T("") );
	const int count = m_maps.size();

	for ( ; m_index < count; ++m_index ) {
		m_mapgrid->AddMap( m_maps[m_index] );
	}
	return false;
}

bool MapSelectDialog::LoadPopularStep()
{
	wxLogDebugFunc( _T("") );
	try {
		m_ui.GetServer().battles_iter->IteratorBegin();
		while ( !m_ui.GetServer().battles_iter->EOL() ) {
			Battle* b = m_ui.GetServer().battles_iter->GetBattle();
			if ( b != NULL ) m_mapgrid->AddMap( b->GetHostMapName() );
		}
	}
	catch (...) {} // m_ui.GetServer may throw when disconnected...
	return false;
}

bool MapSelectDialog::LoadRecentStep()
{
	wxLogDebugFunc( _T("") );
	const int count = m_maps.size();

	// just check whether map names are contained in replay names
	// not the most elegant solution but, hey, it works
	for ( ; m_index < count; ++m_index ) {
		// prefix and suffix with underscore to prevent most common partial matches
		const wxString mapname = _T("_") + m_maps[m_index].BeforeLast( '.' ) + _T("_");
		for (std::vector< wxString >::const_iterator it = m_replays.begin(); it != m_replays.end(); ++it) {
			if ( it->Contains( mapname ) )
				m_mapgrid->AddMap( m_maps[m_index] );
		}
	}

	return false;
}

// filter event handlers

void MapSelectDialog::OnFilterAllSelect(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	Load( State_LoadAll );
}

void MapSelectDialog::OnFilterPopularSelect(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	Load( State_LoadPopular );
}

void MapSelectDialog::OnFilterRecentSelect(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	Load( State_LoadRecent );
}

void MapSelectDialog::OnFilterTextChanged(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	if ( m_state == State_Idle ) UpdateSortAndFilter();
}
