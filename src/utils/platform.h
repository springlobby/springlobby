/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_PLATFORM_H
#define SPRINGLOBBY_HEADERGUARD_PLATFORM_H

#include <wx/string.h>
#include <wx/arrstr.h>

/**
    let origin be /path/to/some/dir and destination /some/other/path
    this will copy dir (and everything below that recursively to /some/other/path/dir
    \return true if successful
*/
bool MoveDirWithFilebackupRename(wxString from, wxString to, bool backup = true, bool silent = false);

//! set new cwd in ctor, reset to old in dtor
class CwdGuard
{
public:
	CwdGuard(const wxString& new_cwd);
	~CwdGuard();

private:
	wxString m_old_cwd;
};

int RunProcess(const wxString& cmd, const wxArrayString& params, const bool async = false, const bool root = false);
int BrowseFolder(const wxString& path);
int WaitForExit(int pid);
void ErrorMsgBox(const wxString& err, bool silent = false);

//!

#endif // SPRINGLOBBY_HEADERGUARD_PLATFORM_H
