/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define BOOST_TEST_MODULE slconfig

#include <boost/test/unit_test.hpp>

#include "testingstuff/silent_logger.h"

#include "chatlog.h"

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/filefn.h>

struct TestInitializer
{
	TestInitializer()
	{
		InitWxLogger();
	}
	~TestInitializer()
	{
	}
};

BOOST_GLOBAL_FIXTURE(TestInitializer);

void customMessageBox(int, wxString const&, wxString const&, long, int, int)
{
}


BOOST_AUTO_TEST_CASE(slconfig)
{
	//delete wxLog::SetActiveTarget(new wxLogStderr(NULL));

	const wxString line1 = _T("this is line 1");
	const wxString line2 = _T("this is line 2");
	const wxString line3 = _T("this is line 3");

	ChatLog* logfile;
	logfile = new ChatLog();
	BOOST_CHECK(logfile->SetLogFile(_T("test")));
	BOOST_CHECK(logfile->AddMessage(line1));
	BOOST_CHECK(logfile->AddMessage(line2));
	BOOST_CHECK(logfile->AddMessage(line3));

	delete logfile;
	logfile = new ChatLog();
	BOOST_CHECK(logfile->SetLogFile(_T("test")));

	wxArrayString lines = logfile->GetLastLines();

	BOOST_CHECK_MESSAGE(lines.GetCount() > 3, lines.GetCount());
	for (auto line : lines) {
		wxLogMessage(_T("line: '%s'"), line.c_str());
	}

	BOOST_CHECK(lines[lines.GetCount() - 4] == line1);
	BOOST_CHECK(lines[lines.GetCount() - 3] == line2);
	BOOST_CHECK(lines[lines.GetCount() - 2] == line3);

	delete logfile;
}
