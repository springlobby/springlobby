/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CRASHREPORTERDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_CRASHREPORTERDIALOG_H


#include "gui/windowattributespickle.h"
#include "generated/CrashReporterDialogBase.h"


class CrashReporterDialog : public CrashReporterDialogBase, public WindowHintsPickle
{
public:
	static const int CANCEL = 0;
	static const int RERUN_NORMAL = 1;
	static const int RERUN_SAFE = 2;

	explicit CrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message, const wxString& filePath);
	static int RunCrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message, const wxString& filePath);

private:
	void OnCancel(wxCommandEvent& event);
	void OnClickBugReport(wxCommandEvent& event);
	void OnNorm(wxCommandEvent& event);
	void OnSafe(wxCommandEvent& event);
};

namespace SL
{
//! this shows an instance and handles everything wrt data feeding and input handling
}


#endif // SPRINGLOBBY_HEADERGUARD_CRASHREPORTERDIALOG_H
