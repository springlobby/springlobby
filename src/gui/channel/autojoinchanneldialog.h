/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef AUTOJOINCHANNELDIALOG_H
#define AUTOJOINCHANNELDIALOG_H

#include <wx/dialog.h>
#include <wx/string.h>
class wxCommandEvent;
class wxTextCtrl;
class wxWindow;

/** \brief A dialog that lets the user manullay edit his list of channels to join on startup (possibly with passwords)
 * Passwords for existing channels aren't displayed. The dialog expects a [chanName|password] pair per line.
 * \todo See that parsing input doesn't crap out when user enters rubbish
 */
class AutojoinChannelDialog : public wxDialog
{
public:
	/** Default constructor */
	explicit AutojoinChannelDialog(wxWindow* parent);
	/** Default destructor */
	virtual ~AutojoinChannelDialog();

private:
	wxTextCtrl* m_channel_list;

	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // AUTOJOINCHANNELDIALOG_H
