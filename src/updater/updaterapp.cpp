/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: UpdaterApp
//

#include <wx/intl.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include "updaterapp.h"
#include "utils/platform.h"

IMPLEMENT_APP(UpdaterApp)

UpdaterApp::UpdaterApp()
    : m_pid(0)
{
	SetAppName(_T("springlobby_updater"));
}

static wxString TrimQuotes(const wxString& str)
{
	wxString res = str;
	res.Replace(_T("\""), wxEmptyString, true);
	return res;
}

//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool UpdaterApp::OnInit()
{
	//this triggers the Cli Parser amongst other stuff
	if (!wxApp::OnInit())
		return false;

	if (m_paramcount == 5) {
		WaitForExit(m_pid);
		wxArrayString params;
		const wxString src = TrimQuotes(m_source_dir);
		const wxString dst = TrimQuotes(m_destination_dir);

		if (!wxFileName::IsDirWritable(dst) || !StartUpdate(src, dst, true)) { //update failed, try as admin
			params.push_back(m_source_dir);
			params.push_back(m_destination_dir);
			RunProcess(m_updater_exe, params, false, true);
			params.clear();
		}
		//start springlobby
		return RunProcess(m_springlobby_exe, params, true);
	}
	if (m_paramcount != 2) {
		return false;
	}
	return StartUpdate(TrimQuotes(m_source_dir), TrimQuotes(m_destination_dir), false);
}

int UpdaterApp::OnRun()
{
	return 0; //instantly exit updater
}

void UpdaterApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxCmdLineEntryDesc cmdLineDesc[] = {
	    {wxCMD_LINE_SWITCH, _("h"), _("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
	    {wxCMD_LINE_PARAM, NULL, NULL, _("<parent pid> <springlobby.exe> <updater.exe> <source dir> <destination dir>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
	    {wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0} //while this throws warnings, it is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html
	};

	parser.SetDesc(cmdLineDesc);
	parser.SetSwitchChars(_T("-"));
}

//! @brief parses the command line
bool UpdaterApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (parser.Found(_T("help")))
		return false; // not a syntax error, but program should stop if user asked for command line usage
	m_paramcount = parser.GetParamCount();
	if (m_paramcount == 2) {
		m_source_dir = parser.GetParam(0);
		m_destination_dir = parser.GetParam(1);
		return true;
	}
	if (m_paramcount == 5) {
		if (!parser.GetParam(0).ToLong(&m_pid)) {
			ErrorMsgBox(_T("Invalid pid %s") + parser.GetParam(0));
			return false;
		}
		m_springlobby_exe = parser.GetParam(1);
		m_updater_exe = parser.GetParam(2);
		m_source_dir = parser.GetParam(3);
		m_destination_dir = parser.GetParam(4);
		return true;
	}
	return false;
}

static wxString TrimSep(const wxString& path)
{
	const wxString sep = wxFileName::GetPathSeparator();
	if (path.EndsWith(sep)) {
		return path.SubString(0, path.length() - 2);
	}
	return path;
}

bool UpdaterApp::StartUpdate(const wxString& source, const wxString& destination, bool silent)
{
	const bool ret = MoveDirWithFilebackupRename(TrimSep(source), TrimSep(destination), silent);
	if (!ret) {
		ErrorMsgBox(_T("Copy failed: \n") + source + _T("\n") + destination, silent);
	}
	return ret;
}
