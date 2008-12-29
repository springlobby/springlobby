#ifndef MAPSELECTDIALOG_H
#define MAPSELECTDIALOG_H

//(*Headers(MapSelectDialog)
#include <wx/dialog.h>
class wxStdDialogButtonSizer;
class MapGridCtrl;
class wxStaticText;
class wxBoxSizer;
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
		MapGridCtrl* m_mapgrid;
		wxChoice* m_vertical_choice;
		wxChoice* m_horizontal_choice;
		//*)
		wxButton* m_vertical_direction_button;
		wxButton* m_horizontal_direction_button;

		//(*Identifiers(MapSelectDialog)
		static const long ID_STATICTEXT2;
		static const long ID_VERTICAL_CHOICE;
		static const long ID_STATICTEXT1;
		static const long ID_HORIZONTAL_CHOICE;
		static const long ID_MAPGRID;
		//*)
		static const long ID_VERTICAL_DIRECTION;
		static const long ID_HORIZONTAL_DIRECTION;

		//(*Handlers(MapSelectDialog)
		void OnInit(wxInitDialogEvent& event);
		void OnSortKeySelect(wxCommandEvent& event);
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
