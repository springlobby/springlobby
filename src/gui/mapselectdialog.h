/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef MAPSELECTDIALOG_H
#define MAPSELECTDIALOG_H

#include <wx/dialog.h>
#include <vector>
#include "gui/windowattributespickle.h"
class wxStdDialogButtonSizer;
class wxTextCtrl;
class MapGridCtrl;
class wxRadioButton;
class wxStaticText;
class wxListCtrl;
class wxBoxSizer;
class wxStaticBoxSizer;
class wxChoice;
class wxButton;
class Ui;

namespace LSL
{
struct UnitsyncMap;
}

class MapSelectDialog : public wxDialog, public WindowAttributesPickle
{
public:
	explicit MapSelectDialog(wxWindow* parent);
	virtual ~MapSelectDialog();

	LSL::UnitsyncMap* GetSelectedMap() const;

	void OnUnitsyncReloaded(wxCommandEvent& data);

private:
	wxBoxSizer* m_main_sizer;
	wxTextCtrl* m_map_name;
	MapGridCtrl* m_mapgrid;
	wxChoice* m_vertical_choice;
	wxChoice* m_horizontal_choice;
	wxTextCtrl* m_filter_text;
	wxListCtrl* m_map_opts_list;
	wxStaticBoxSizer* m_map_details;
	wxButton* m_vertical_direction_button;
	wxButton* m_horizontal_direction_button;

	static const long ID_STATICTEXT2;
	static const long ID_VERTICAL_CHOICE;
	static const long ID_STATICTEXT1;
	static const long ID_HORIZONTAL_CHOICE;
	static const long ID_FILTER_ALL;
	static const long ID_FILTER_POPULAR;
	static const long ID_FILTER_RECENT;
	static const long ID_FILTER_TEXT;
	static const long ID_MAP_NAME;
	static const long ID_MAP_OPTS_LIST;
	static const long ID_MAPGRID;
	static const long ID_VERTICAL_DIRECTION;
	static const long ID_HORIZONTAL_DIRECTION;

	void OnInit(wxInitDialogEvent& event);
	void OnSortKeySelect(wxCommandEvent& event);
	void OnMapGridLeftDClick(wxMouseEvent& event);
	void OnFilterAllSelect(wxCommandEvent& event);
	void OnFilterPopularSelect(wxCommandEvent& event);
	void OnFilterRecentSelect(wxCommandEvent& event);
	void OnFilterTextChanged(wxCommandEvent& event);

	void OnMapSelected(wxCommandEvent& event);
	void OnMapLoadingCompleted(wxCommandEvent& event);
	void OnVerticalDirectionClicked(wxCommandEvent& event);
	void OnHorizontalDirectionClicked(wxCommandEvent& event);

	void AppendSortKeys(wxChoice* choice);
	void UpdateSortAndFilter();

	void LoadAll();
	void LoadPopular();
	void LoadRecent();

	bool m_horizontal_direction;
	bool m_vertical_direction;
	wxArrayString m_maps;
};

wxString mapSelectDialog(bool hidden = false, wxWindow* parent = NULL);

#endif
