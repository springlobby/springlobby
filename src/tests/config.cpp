/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define BOOST_TEST_MODULE slconfig
#include <boost/test/unit_test.hpp>

#include "testingstuff/silent_logger.h"

#include "utils/slconfig.h"
#include "utils/conversion.h"

#include <wx/app.h>
#include <wx/string.h>
#include <wx/filename.h>

SLCONFIG("/test/string", "hello world!", "test string");
SLCONFIG("/test/long", -12345l, "test long");
SLCONFIG("/test/double", -321.123, "test double");
SLCONFIG("/test/bool", true, "test bool");
SLCONFIG("/test/notbool", false, "test bool");

struct TestInitializer{
	TestInitializer()
	{
		InitWxLogger();

		_wxApp = new wxApp();
	}
	~TestInitializer()
	{
		delete _wxApp;
	}

	wxApp* _wxApp;
};

DECLARE_APP(wxApp)
IMPLEMENT_APP(wxApp)

BOOST_GLOBAL_FIXTURE(TestInitializer);

namespace SlPaths
{
std::string GetConfigPath()
{
	return STD_STRING(wxFileName::GetTempDir()) + "/sltest.config";
}
}

BOOST_AUTO_TEST_CASE(slconfig)
{

	//	cfg().SaveFile();
	BOOST_CHECK(cfg().ReadLong(_T("/test/long")) == -12345l);
	BOOST_CHECK(cfg().Read(_T("/test/long")) == _T("-12345"));

	BOOST_CHECK(cfg().ReadDouble(_T("/test/double")) == -321.123);
	BOOST_CHECK(cfg().Read(_T("/test/double")) == _T("-321.123"));

	BOOST_CHECK(cfg().ReadBool(_T("/test/bool")) == true);
	BOOST_CHECK(cfg().ReadBool(_T("/test/notbool")) == false);
	BOOST_CHECK(cfg().Read(_T("/test/bool")) == _T("1"));

	BOOST_CHECK(cfg().ReadString(_T("/test/string")) == _T("hello world!"));
	//BOOST_CHECK(cfg().Read(_T("/test/string")) == _T("hello world!") ); //FIXME: fails, returns 104 ?!

	BOOST_CHECK(cfg().Write(_T("/test/long"), -10l));
	BOOST_CHECK(cfg().ReadLong(_T("/test/long")) == -10l);

	BOOST_CHECK(cfg().Write(_T("/test/double"), 10.3));
	BOOST_CHECK(cfg().ReadDouble(_T("/test/double")) == 10.3);

	BOOST_CHECK(cfg().Write(_T("/test/bool"), false));
	BOOST_CHECK(cfg().ReadBool(_T("/test/bool")) == false);

	BOOST_CHECK(cfg().Write(_T("/test/string"), (const wxString&)_T("test2")));
	BOOST_CHECK(cfg().Read(_T("/test/string")) == _T("test2"));
	BOOST_CHECK(cfg().ReadString(_T("/test/string")) == _T("test2"));

	//	cfg().SaveFile();
}
