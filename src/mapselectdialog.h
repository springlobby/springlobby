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

		//(*Identifiers(MapSelectDialog)
		static const long ID_STATICTEXT2;
		static const long ID_VERTICAL;
		static const long ID_STATICTEXT1;
		static const long ID_HORIZONTAL;
		static const long ID_MAPGRID;
		//*)

		//(*Handlers(MapSelectDialog)
		void OnInit(wxInitDialogEvent& event);
		void OnSortKeySelect(wxCommandEvent& event);
		//*)

		void OnMapSelected( wxCommandEvent& event );

		void AppendSortKeys( wxChoice* choice );
		void Sort();

		Ui& m_ui;

		DECLARE_EVENT_TABLE()
};

#endif
