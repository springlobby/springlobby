/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CRASHREPORTERDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_CRASHREPORTERDIALOG_H


#include "gui/windowattributespickle.h"
#include "generated/CrashReporterDialogBase.h"


class CrashReporterDialog : public CrashReporterDialogBase, public WindowHintsPickle
{
public:
	explicit CrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message);
	static int RunCrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message);

private:
	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
};

namespace SL
{
//! this shows an instance and handles everything wrt data feeding and input handling
}


#endif // SPRINGLOBBY_HEADERGUARD_CRASHREPORTERDIALOG_H
