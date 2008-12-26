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

class MapSelectDialog: public wxDialog
{
	public:

		MapSelectDialog(wxWindow* parent,Ui& ui);
		virtual ~MapSelectDialog();

	protected:

		//(*Declarations(MapSelectDialog)
		MapGridCtrl* m_mapgrid;
		wxStaticText* StaticText1;
		wxChoice* Choice1;
		//*)

		//(*Identifiers(MapSelectDialog)
		static const long ID_STATICTEXT1;
		static const long ID_CHOICE1;
		static const long ID_MAPGRID;
		//*)

		//(*Handlers(MapSelectDialog)
		//*)

		Ui& m_ui;

		DECLARE_EVENT_TABLE()
};

#endif
