/* Author: Tobi Vollebregt */

#ifndef MAPSELECTDIALOG_H
#define MAPSELECTDIALOG_H

#include <vector>
#include "gui/windowattributespickle.h"
#include "utils/isink.h"
//(*Headers(MapSelectDialog)
#include <wx/dialog.h>
class wxStdDialogButtonSizer;
class wxTextCtrl;
class MapGridCtrl;
class wxRadioButton;
class wxStaticText;
class wxListCtrl;
class wxBoxSizer;
class wxStaticBoxSizer;
class wxChoice;
//*)
class wxButton;

class Ui;
struct UnitSyncMap;


class MapSelectDialog: public wxDialog, public WindowAttributesPickle, public UnitsyncReloadedSink<MapSelectDialog>
{
	public:

		MapSelectDialog( wxWindow* parent = 0 );
		virtual ~MapSelectDialog();

		UnitSyncMap* GetSelectedMap() const;

		void OnUnitsyncReloaded( GlobalEvents::GlobalEventData data );

	protected:

		//(*Declarations(MapSelectDialog)
		wxRadioButton* m_filter_recent;
		wxStaticText* m_map_name;
		wxRadioButton* m_filter_all;
		MapGridCtrl* m_mapgrid;
		wxChoice* m_vertical_choice;
		wxChoice* m_horizontal_choice;
		wxTextCtrl* m_filter_text;
		wxListCtrl* m_map_opts_list;
		wxRadioButton* m_filter_popular;
		wxStaticBoxSizer* m_map_details;
		//*)
		wxButton* m_vertical_direction_button;
		wxButton* m_horizontal_direction_button;

		//(*Identifiers(MapSelectDialog)
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
		//*)
		static const long ID_VERTICAL_DIRECTION;
		static const long ID_HORIZONTAL_DIRECTION;

		//(*Handlers(MapSelectDialog)
		void OnInit(wxInitDialogEvent& event);
		void OnSortKeySelect(wxCommandEvent& event);
		void OnMapGridLeftDClick(wxMouseEvent& event);
		void OnFilterAllSelect(wxCommandEvent& event);
		void OnFilterPopularSelect(wxCommandEvent& event);
		void OnFilterRecentSelect(wxCommandEvent& event);
		void OnFilterTextChanged(wxCommandEvent& event);
		//*)

		void OnMapSelected( wxCommandEvent& event );
		void OnMapLoadingCompleted( wxCommandEvent& event );
		void OnVerticalDirectionClicked( wxCommandEvent& event );
		void OnHorizontalDirectionClicked( wxCommandEvent& event );

		void AppendSortKeys( wxChoice* choice );
		void UpdateSortAndFilter();

		void LoadAll();
		void LoadPopular();
		void LoadRecent();

		bool m_horizontal_direction;
		bool m_vertical_direction;
		wxArrayString m_maps;
		wxArrayString m_replays;

		static const wxString m_dialog_name;
		static const unsigned int m_filter_all_sett = 0;
		static const unsigned int m_filter_recent_sett = 1;
		static const unsigned int m_filter_popular_sett = 2;

		DECLARE_EVENT_TABLE()
};

MapSelectDialog& mapSelectDialog();
#endif

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

