/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/settings.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>

#include <lslutils/globalsmanager.h>
#include <lslutils/misc.h>

#include "mapselectdialog.h"
#include "mapgridctrl.h"
#include "ibattle.h"
#include "iserver.h"
#include "serverselector.h"
#include "settings.h"
#include "ui.h"
#include "uiutils.h"
#include "gui/controls.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "settings.h"
#include "log.h"



const long MapSelectDialog::ID_STATICTEXT2 = wxNewId();
const long MapSelectDialog::ID_VERTICAL_CHOICE = wxNewId();
const long MapSelectDialog::ID_STATICTEXT1 = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL_CHOICE = wxNewId();
const long MapSelectDialog::ID_FILTER_ALL = wxNewId();
const long MapSelectDialog::ID_FILTER_POPULAR = wxNewId();
const long MapSelectDialog::ID_FILTER_RECENT = wxNewId();
const long MapSelectDialog::ID_FILTER_TEXT = wxNewId();
const long MapSelectDialog::ID_MAP_NAME = wxNewId();
const long MapSelectDialog::ID_MAP_OPTS_LIST = wxNewId();
const long MapSelectDialog::ID_MAPGRID = wxNewId();
const long MapSelectDialog::ID_VERTICAL_DIRECTION = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL_DIRECTION = wxNewId();

MapSelectDialog::MapSelectDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("Select map (click and drag to scroll)"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX, _T("wxID_ANY"))
    , WindowAttributesPickle(_T("MapSelector"), this, wxSize(880, 600))
    , m_horizontal_direction(sett().GetHorizontalSortorder())
    , m_vertical_direction(sett().GetVerticalSortorder())
{

	wxBoxSizer* BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	wxStaticText* StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Vertical sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer2->Add(StaticText2, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	wxBoxSizer* boxSizerVertical = new wxBoxSizer(wxHORIZONTAL);
	m_vertical_choice = new wxChoice(this, ID_VERTICAL_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_VERTICAL_CHOICE"));
	boxSizerVertical->Add(m_vertical_choice, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(boxSizerVertical, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	wxStaticText* StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Horizontal sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	wxBoxSizer* boxSizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
	m_horizontal_choice = new wxChoice(this, ID_HORIZONTAL_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_HORIZONTAL_CHOICE"));
	boxSizerHorizontal->Add(m_horizontal_choice, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(boxSizerHorizontal, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	wxStaticBoxSizer* StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Show"));
	m_filter_all = new wxRadioButton(this, ID_FILTER_ALL, _("All maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_ALL"));
	m_filter_all->SetToolTip(_("Shows all available maps"));
	StaticBoxSizer1->Add(m_filter_all, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	m_filter_popular = new wxRadioButton(this, ID_FILTER_POPULAR, _("Popular maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_POPULAR"));
	m_filter_popular->SetToolTip(_("Shows only maps which are currently being player on the server. You must be online to use this."));
	StaticBoxSizer1->Add(m_filter_popular, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	m_filter_recent = new wxRadioButton(this, ID_FILTER_RECENT, _("Recently played maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_RECENT"));
	m_filter_recent->SetValue(true);
	m_filter_recent->SetToolTip(_("Shows only maps you played recently. (Based on your replays.)"));
	StaticBoxSizer1->Add(m_filter_recent, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2->Add(StaticBoxSizer1, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	wxStaticBoxSizer* StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Filter"));
	m_filter_text = new wxTextCtrl(this, ID_FILTER_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_TEXT"));
	m_filter_text->SetToolTip(_("Shows only maps which contain this text in their name or description."));
	StaticBoxSizer2->Add(m_filter_text, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2->Add(StaticBoxSizer2, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	m_map_details = new wxStaticBoxSizer(wxVERTICAL, this, _("Map details"));
	m_map_name = new wxStaticText(this, ID_MAP_NAME, wxEmptyString, wxDefaultPosition, wxSize(170, 90), wxST_NO_AUTORESIZE, _T("ID_MAP_NAME"));
	m_map_name->SetLabel(wxEmptyString);
	m_map_details->Add(m_map_name, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	m_map_opts_list = new wxListCtrl(this, ID_MAP_OPTS_LIST, wxDefaultPosition, wxSize(170, 120), wxLC_REPORT | wxLC_NO_HEADER | wxNO_BORDER, wxDefaultValidator, _T("ID_MAP_OPTS_LIST"));
	m_map_details->Add(m_map_opts_list, 1, wxBOTTOM | wxLEFT | wxRIGHT | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(m_map_details, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	wxStdDialogButtonSizer* StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer2->Add(StdDialogButtonSizer1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	m_mapgrid = new MapGridCtrl(this, wxSize(600, 400), ID_MAPGRID);
	BoxSizer1->Add(m_mapgrid, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();


	m_vertical_direction_button = new wxButton(this, ID_VERTICAL_DIRECTION, _T("ᴧ"), wxDefaultPosition, wxSize(CONTROL_HEIGHT, CONTROL_HEIGHT), 0, wxDefaultValidator, _T("ID_VERTICAL_DIRECTION"));
	boxSizerVertical->Add(m_vertical_direction_button, 0, wxALL | wxEXPAND | wxSHAPED | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	m_horizontal_direction_button = new wxButton(this, ID_HORIZONTAL_DIRECTION, _T("<"), wxDefaultPosition, wxSize(CONTROL_HEIGHT, CONTROL_HEIGHT), 0, wxDefaultValidator, _T("ID_HORIZONTAL_DIRECTION"));
	boxSizerHorizontal->Add(m_horizontal_direction_button, 0, wxALL | wxEXPAND | wxSHAPED | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);


	// TODO: refactor, this is copied from battlemaptab.cpp
	m_map_opts_list->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	m_map_opts_list->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT));

	m_map_opts_list->InsertColumn(0, _("Option"),  wxLIST_FORMAT_LEFT, 90);
	m_map_opts_list->InsertColumn(1, _("Value"),  wxLIST_FORMAT_LEFT, 80);

	m_map_opts_list->InsertItem(0, _("Size"));
	m_map_opts_list->InsertItem(1, _("Windspeed"));
	m_map_opts_list->InsertItem(2, _("Tidal strength"));
	m_map_opts_list->InsertItem(3, _("Gravity"));
	m_map_opts_list->InsertItem(4, _("Extractor radius"));
	m_map_opts_list->InsertItem(5, _("Max metal"));
	m_map_opts_list->InsertItem(6, _("Start positions"));

	Layout();

	Connect(ID_VERTICAL_CHOICE, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_HORIZONTAL_CHOICE, wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_FILTER_ALL, wxEVT_COMMAND_RADIOBUTTON_SELECTED, (wxObjectEventFunction)&MapSelectDialog::OnFilterAllSelect);
	Connect(ID_FILTER_POPULAR, wxEVT_COMMAND_RADIOBUTTON_SELECTED, (wxObjectEventFunction)&MapSelectDialog::OnFilterPopularSelect);
	Connect(ID_FILTER_RECENT, wxEVT_COMMAND_RADIOBUTTON_SELECTED, (wxObjectEventFunction)&MapSelectDialog::OnFilterRecentSelect);
	Connect(ID_FILTER_TEXT, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&MapSelectDialog::OnFilterTextChanged);
	m_mapgrid->Connect(ID_MAPGRID, wxEVT_LEFT_DCLICK, (wxObjectEventFunction)&MapSelectDialog::OnMapGridLeftDClick, 0, this);
	Connect(wxID_ANY, wxEVT_INIT_DIALOG, (wxObjectEventFunction)&MapSelectDialog::OnInit);
	Connect(ID_MAPGRID, MapGridCtrl::MapSelectedEvt, (wxObjectEventFunction)&MapSelectDialog::OnMapSelected, 0, this);
	Connect(ID_MAPGRID, MapGridCtrl::LoadingCompletedEvt, (wxObjectEventFunction)&MapSelectDialog::OnMapLoadingCompleted, 0, this);
	Connect(ID_VERTICAL_DIRECTION, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MapSelectDialog::OnVerticalDirectionClicked);
	Connect(ID_HORIZONTAL_DIRECTION, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MapSelectDialog::OnHorizontalDirectionClicked);

	ConnectGlobalEvent(this, GlobalEvent::OnUnitsyncReloaded, wxObjectEventFunction(&MapSelectDialog::OnUnitsyncReloaded));
}

MapSelectDialog::~MapSelectDialog()
{
	sett().SetHorizontalSortkeyIndex(m_horizontal_choice->GetSelection());
	sett().SetVerticalSortkeyIndex(m_vertical_choice->GetSelection());
	sett().SetHorizontalSortorder(m_horizontal_direction);
	sett().SetVerticalSortorder(m_vertical_direction);
	if (m_filter_popular->GetValue())
		sett().SetMapSelectorFilterRadio(m_filter_popular_sett);
	else if (m_filter_recent->GetValue())
		sett().SetMapSelectorFilterRadio(m_filter_recent_sett);
	else
		sett().SetMapSelectorFilterRadio(m_filter_all_sett);

	if (IsShown())
		EndModal(0);
}


void MapSelectDialog::OnInit(wxInitDialogEvent& /*unused*/)
{
	slLogDebugFunc("");

	AppendSortKeys(m_horizontal_choice);
	AppendSortKeys(m_vertical_choice);

	m_horizontal_choice->SetSelection(sett().GetHorizontalSortkeyIndex());
	m_vertical_choice->SetSelection(sett().GetVerticalSortkeyIndex());

	m_horizontal_direction_button->SetLabel(m_horizontal_direction ? _T(">") : _T("<"));
	m_vertical_direction_button->SetLabel(m_vertical_direction ? _T("ᴠ") : _T("ᴧ"));

	m_maps = lslTowxArrayString(LSL::usync().GetMapList());
	//true meaning replays, false meaning savegames
	m_replays = lslTowxArrayString(LSL::usync().GetPlaybackList(true));

	const unsigned int lastFilter = sett().GetMapSelectorFilterRadio();
	m_filter_popular->Enable(ui().IsConnected());

	if ((lastFilter == m_filter_popular_sett) && (ui().IsConnected())) {
		m_filter_popular->SetValue(true);
		LoadPopular();
	} else if ((lastFilter == m_filter_recent_sett) && (!m_replays.empty())) {
		m_filter_recent->Enable(true);
		m_filter_recent->SetValue(true);
		LoadRecent();
	} else {
		m_filter_all->SetValue(true);
		LoadAll();
	}

	UpdateSortAndFilter();

	m_filter_text->SetFocus();
}


void MapSelectDialog::OnSortKeySelect(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	UpdateSortAndFilter();
}


void MapSelectDialog::AppendSortKeys(wxChoice* choice)
{
	// see MapGridCtrl for available SortKeys
	choice->Append(_("Name"), (void*)MapGridCtrl::SortKey_Name);
	choice->Append(_("Tidal strength"), (void*)MapGridCtrl::SortKey_TidalStrength);
	choice->Append(_("Gravity"), (void*)MapGridCtrl::SortKey_Gravity);
	choice->Append(_("Max metal"), (void*)MapGridCtrl::SortKey_MaxMetal);
	choice->Append(_("Extractor radius"), (void*)MapGridCtrl::SortKey_ExtractorRadius);
	choice->Append(_("Minimum wind"), (void*)MapGridCtrl::SortKey_MinWind);
	choice->Append(_("Maximum wind"), (void*)MapGridCtrl::SortKey_MaxWind);
	choice->Append(_("Average wind"), (void*)MapGridCtrl::SortKey_Wind);
	choice->Append(_("Size (map area)"), (void*)MapGridCtrl::SortKey_Area);
	choice->Append(_("Aspect ratio"), (void*)MapGridCtrl::SortKey_AspectRatio);
	choice->Append(_("Number of start positions"), (void*)MapGridCtrl::SortKey_PosCount);
}

static MapGridCtrl::SortKey GetSelectedSortKey(wxChoice* choice)
{
	const int selection = choice->GetSelection();
	if (selection == wxNOT_FOUND) //default to first entry
		return MapGridCtrl::SortKey_Name;
	return (MapGridCtrl::SortKey)(int)(long) choice->GetClientData(selection);
}

namespace
{
struct FilterPredicate
{
	FilterPredicate(const wxString& _searchText)
	    : searchText(STD_STRING(_searchText.Lower()))
	{
	}
	bool operator()(const LSL::UnitsyncMap& map) const
	{
		return boost::to_lower_copy(map.name).find(searchText) != std::string::npos || boost::to_lower_copy(map.info.description).find(searchText) != std::string::npos || boost::to_lower_copy(map.info.author).find(searchText) != std::string::npos;
	}
	const std::string searchText;
};
}

void MapSelectDialog::UpdateSortAndFilter()
{
	FilterPredicate predicate(m_filter_text->GetValue());
	m_mapgrid->Filter(predicate);
	m_mapgrid->Sort(GetSelectedSortKey(m_vertical_choice), GetSelectedSortKey(m_horizontal_choice), m_vertical_direction, m_horizontal_direction);
	m_mapgrid->Refresh();
}

LSL::UnitsyncMap* MapSelectDialog::GetSelectedMap() const
{
	slLogDebugFunc("");
	return m_mapgrid->GetSelectedMap();
}

void MapSelectDialog::OnMapSelected(wxCommandEvent& /*event*/)
{
	//	const wxString& mapname = event.GetString();

	slLogDebugFunc("");

	const LSL::UnitsyncMap* pMap = m_mapgrid->GetSelectedMap();
	if (pMap == NULL)
		return;
	const LSL::UnitsyncMap& map = *pMap;

	m_map_name->SetLabel(TowxString(map.name + "\n\n" + map.info.description));

	// TODO: refactor, this is copied from battlemaptab.cpp
	m_map_opts_list->SetItem(0, 1, wxString::Format(_T("%dx%d"), map.info.width / 512, map.info.height / 512));
	m_map_opts_list->SetItem(1, 1, wxString::Format(_T("%d-%d"), map.info.minWind, map.info.maxWind));
	m_map_opts_list->SetItem(2, 1, wxString::Format(_T("%d"), map.info.tidalStrength));
	m_map_opts_list->SetItem(3, 1, wxString::Format(_T("%d"), map.info.gravity));
	m_map_opts_list->SetItem(4, 1, wxString::Format(_T("%d"), map.info.extractorRadius));
	m_map_opts_list->SetItem(5, 1, wxString::Format(_T("%.3f"), map.info.maxMetal));
	m_map_opts_list->SetItem(6, 1, wxString::Format(_T("%d"), (int)map.info.positions.size()));
}

void MapSelectDialog::OnMapLoadingCompleted(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	// to apply stored sorting settings we need to re-apply sorting after loading finished
	UpdateSortAndFilter();
}

void MapSelectDialog::OnVerticalDirectionClicked(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	m_vertical_direction = !m_vertical_direction;
	m_vertical_direction_button->SetLabel(m_vertical_direction ? _T("ᴠ") : _T("ᴧ"));
	UpdateSortAndFilter();
}

void MapSelectDialog::OnHorizontalDirectionClicked(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	m_horizontal_direction = !m_horizontal_direction;
	m_horizontal_direction_button->SetLabel(m_horizontal_direction ? _T(">") : _T("<"));
	UpdateSortAndFilter();
}

void MapSelectDialog::OnMapGridLeftDClick(wxMouseEvent& /*unused*/)
{
	slLogDebugFunc("");

	if (m_mapgrid->GetSelectedMap()) {
		EndModal(wxID_OK);
	}
}

void MapSelectDialog::LoadAll()
{
	slLogDebugFunc("");
	const int count = m_maps.size();

	m_mapgrid->Clear();

	for (int i = 0; i < count; ++i) {
		m_mapgrid->AddMap(m_maps[i]);
	}

	m_mapgrid->Refresh();
}

void MapSelectDialog::LoadPopular()
{
	slLogDebugFunc("");

	m_mapgrid->Clear();

	try {
		serverSelector().GetServer().battles_iter->IteratorBegin();
		while (!serverSelector().GetServer().battles_iter->EOL()) {
			IBattle* b = serverSelector().GetServer().battles_iter->GetBattle();
			const std::string mapname = b->GetHostMapName();
			assert(!mapname.empty());
			if (b != NULL)
				m_mapgrid->AddMap(TowxString(mapname));
		}
	} catch (...) {
	} // ui().GetServer may throw when disconnected...

	m_mapgrid->Refresh();
}

void MapSelectDialog::LoadRecent()
{
	slLogDebugFunc("");
	const int count = m_maps.size();

	m_mapgrid->Clear();

	// just check whether map names are contained in replay names
	// not the most elegant solution but, hey, it works
	for (int i = 0; i < count; ++i) {
		// prefix and suffix with underscore to prevent most common partial matches
		const wxString mapname = _T("_") + m_maps[i].BeforeLast('.') + _T("_");
		long replaycount = long(m_replays.GetCount());
		for (int replaynum = 0; replaynum < replaycount; replaynum++) {
			if (m_replays[replaynum].Find(mapname) != wxNOT_FOUND)
				m_mapgrid->AddMap(m_maps[i]);
		}
	}

	m_mapgrid->Refresh();
}

// filter event handlers

void MapSelectDialog::OnFilterAllSelect(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	LoadAll();
}

void MapSelectDialog::OnFilterPopularSelect(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	LoadPopular();
}

void MapSelectDialog::OnFilterRecentSelect(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	LoadRecent();
}

void MapSelectDialog::OnFilterTextChanged(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	UpdateSortAndFilter();
}

void MapSelectDialog::OnUnitsyncReloaded(wxCommandEvent& /*data*/)
{
	wxInitDialogEvent dummy;
	AddPendingEvent(dummy);
}

wxString mapSelectDialog(bool hidden, wxWindow* parent)
{
	wxString mapname = wxEmptyString;
	assert((hidden && parent != NULL) || (!hidden && parent == NULL)); //at the first call, the window is created hidden
	static MapSelectDialog* m = new MapSelectDialog(parent);
	if ((!hidden) && (m->ShowModal() == wxID_OK) && (m->GetSelectedMap() != NULL)) {
		mapname = TowxString(m->GetSelectedMap()->name);
	}
	return mapname;
}
