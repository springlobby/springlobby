/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef PASTEDIALOG_H
#define PASTEDIALOG_H

#include <wx/dialog.h>

/** \brief let the user decide to pastebin a message
 */
class PasteDialog : public wxDialog
{
public:
	PasteDialog(wxWindow* parent, const wxString& message);
	virtual ~PasteDialog();

	static const int pasteButtonReturnCode = wxID_HIGHEST + 1;

private:
	enum {
		ID_PASTE_BUTTON = wxID_HIGHEST
	};
	void OnPasteButton(wxCommandEvent& evt);
	void OnOptionsNo(wxCommandEvent& event);
	wxBoxSizer* m_main_sizer;
	DECLARE_EVENT_TABLE()
};

#endif // PASTEDIALOG_H
