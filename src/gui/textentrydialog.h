/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/string.h>
#include <wx/dialog.h>

class wxTextCtrl;

class TextEntryDialog : public wxDialog
{
public:
	TextEntryDialog(wxWindow* parent, const wxString& title, const wxString& heading, const wxString& answer, bool multiline = false);
	virtual ~TextEntryDialog();
	void SetValue(const wxString& value);
	const wxString GetValue();

private:
	wxTextCtrl* input;
};
