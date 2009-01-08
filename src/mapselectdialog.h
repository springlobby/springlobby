#ifndef MAPSELECTDIALOG_H
#define MAPSELECTDIALOG_H

// If this isn't included errors result when trying to pass the MainWindow*
// to MapSelectDialog constructor as wxWindow* (because compiler doesn't know
// MainWindow inherits wxWindow in that case.)
#include "mainwindow.h"

//(*Headers(MapSelectDialog)
#include <wx/dialog.h>
#include <wx/timer.h>
class wxStdDialogButtonSizer;
class wxTextCtrl;
class MapGridCtrl;
class wxRadioButton;
class wxStaticText;
class wxBoxSizer;
class wxStaticBoxSizer;
class wxChoice;
//*)
class wxButton;

class Ui;
class UnitSyncMap;


class MapSelectDialog: public wxDialog
{
	public:

		MapSelectDialog( wxWindow* parent, Ui& ui );
		virtual ~MapSelectDialog();

		UnitSyncMap* GetSelectedMap() const;

	protected:

		enum State
		{
			State_Idle,
			State_LoadAll,
			State_LoadPopular,
			State_LoadRecent,
		};

		//(*Declarations(MapSelectDialog)
		wxRadioButton* m_filter_recent;
		wxRadioButton* m_filter_all;
		MapGridCtrl* m_mapgrid;
		wxChoice* m_vertical_choice;
		wxChoice* m_horizontal_choice;
		wxTextCtrl* m_filter_text;
		wxTimer m_timer;
		wxRadioButton* m_filter_popular;
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
		static const long ID_MAPGRID;
		static const long ID_TIMER;
		//*)
		static const long ID_VERTICAL_DIRECTION;
		static const long ID_HORIZONTAL_DIRECTION;

		//(*Handlers(MapSelectDialog)
		void OnInit(wxInitDialogEvent& event);
		void OnSortKeySelect(wxCommandEvent& event);
		void OnMapGridLeftDClick(wxMouseEvent& event);
		void OnTimerTrigger(wxTimerEvent& event);
		void OnFilterAllSelect(wxCommandEvent& event);
		void OnFilterPopularSelect(wxCommandEvent& event);
		void OnFilterRecentSelect(wxCommandEvent& event);
		void OnFilterTextChanged(wxCommandEvent& event);
		//*)

		void OnMapSelected( wxCommandEvent& event );
		void OnVerticalDirectionClicked( wxCommandEvent& event );
		void OnHorizontalDirectionClicked( wxCommandEvent& event );

		void AppendSortKeys( wxChoice* choice );
		void UpdateSortAndFilter();

		void Idle();
		void Load( State newstate );
		bool LoadAllStep();
		bool LoadPopularStep();
		bool LoadRecentStep();

		Ui& m_ui;
		bool m_horizontal_direction;
		bool m_vertical_direction;
		wxArrayString m_maps;
		State m_state;
		int m_index;

		DECLARE_EVENT_TABLE()
};

#endif
