/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef RECONNECTDIALOG_H
#define RECONNECTDIALOG_H

#include <wx/dialog.h>
#include "gui/windowattributespickle.h"
#include "utils/globalevents.h"

class wxCommandEvent;

class ReconnectDialog : public wxDialog, public GlobalEvent, public WindowAttributesPickle
{
public:
    ReconnectDialog();
    void OnQuit( wxCommandEvent& data );

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
