#ifndef AUTOBALANCEDIALOG_H
#define AUTOBALANCEDIALOG_H

//(*Headers(AutoBalanceDialog)
#include <wx/dialog.h>
//*)

class AutoBalanceDialog: public wxDialog
{
	public:

		AutoBalanceDialog(wxWindow* parent);
		virtual ~AutoBalanceDialog();

		//(*Declarations(AutoBalanceDialog)
		//*)

	protected:

		//(*Identifiers(AutoBalanceDialog)
		//*)

	private:

		//(*Handlers(AutoBalanceDialog)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
