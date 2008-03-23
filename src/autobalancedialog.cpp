#include "autobalancedialog.h"

//(*InternalHeaders(AutoBalanceDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(AutoBalanceDialog)
//*)

BEGIN_EVENT_TABLE(AutoBalanceDialog,wxDialog)
	//(*EventTable(AutoBalanceDialog)
	//*)
END_EVENT_TABLE()

AutoBalanceDialog::AutoBalanceDialog(wxWindow* parent)
{
	//(*Initialize(AutoBalanceDialog)
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	//*)
}

AutoBalanceDialog::~AutoBalanceDialog()
{
	//(*Destroy(AutoBalanceDialog)
	//*)
}

