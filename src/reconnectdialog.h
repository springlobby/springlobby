#ifndef RECONNECTDIALOG_H
#define RECONNECTDIALOG_H

#include <wx/dialog.h>
#include "gui/windowattributespickle.h"

class wxCommandEvent;

class ReconnectDialog : public wxDialog, public WindowAttributesPickle
{
public:
    ReconnectDialog();
private:
	void OnReconnect ( wxCommandEvent& event );
	void OnCancel ( wxCommandEvent& event );
	void OnTryNext ( wxCommandEvent& event );
	enum
	{
		CONNECT_RECONNECT = wxID_HIGHEST,
		CONNECT_NEXT,
		CONNECT_CANCEL
	};

	DECLARE_EVENT_TABLE()
};

#endif // RECONNECTDIALOG_H
