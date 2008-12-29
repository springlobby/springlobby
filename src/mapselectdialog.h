#ifndef MAPSELECTDIALOG_H
#define MAPSELECTDIALOG_H

//(*Headers(MapSelectDialog)
#include <wx/dialog.h>
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

		//(*Declarations(MapSelectDialog)
		wxRadioButton* m_filter_recent;
		wxRadioButton* m_filter_all;
		MapGridCtrl* m_mapgrid;
		wxChoice* m_vertical_choice;
		wxChoice* m_horizontal_choice;
		wxRadioButton* m_filter_contain;
		wxTextCtrl* m_filter_text;
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
		static const long ID_FILTER_CONTAIN;
		static const long ID_FILTER_TEXT;
		static const long ID_MAPGRID;
		//*)
		static const long ID_VERTICAL_DIRECTION;
		static const long ID_HORIZONTAL_DIRECTION;

		//(*Handlers(MapSelectDialog)
		void OnInit(wxInitDialogEvent& event);
		void OnSortKeySelect(wxCommandEvent& event);
		void OnMapGridLeftDClick(wxMouseEvent& event);
		//*)

		void OnMapSelected( wxCommandEvent& event );
		void OnVerticalDirectionClicked( wxCommandEvent& event );
		void OnHorizontalDirectionClicked( wxCommandEvent& event );

		void AppendSortKeys( wxChoice* choice );
		void Sort();

		Ui& m_ui;
		bool m_horizontal_direction;
		bool m_vertical_direction;

		DECLARE_EVENT_TABLE()
};

#endif
