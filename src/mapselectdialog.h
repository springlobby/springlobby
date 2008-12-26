/* Author: Tobi Vollebregt */

#ifndef SPRINGLOBBY_HEADERGUARD_MAPSELECTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_MAPSELECTDIALOG_H

#include <wx/dialog.h>

class Ui;


class MapSelectDialog : public wxDialog
{
	public:

		MapSelectDialog( wxWindow* parent, Ui& ui );
		~MapSelectDialog();

	protected:

		Ui& m_ui;

		DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPSELECTDIALOG_H
