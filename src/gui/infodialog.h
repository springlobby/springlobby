/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <wx/dialog.h>

class wxBoxSizer;

class InfoDialog : public wxDialog
{
public:
	InfoDialog(wxWindow* parent);
	virtual ~InfoDialog();

private:
	wxBoxSizer* m_main_sizer;
};

#endif // INFODIALOG_H
