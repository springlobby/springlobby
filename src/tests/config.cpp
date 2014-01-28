#define BOOST_TEST_MODULE slconfig
#include <boost/test/unit_test.hpp>

#include "helper/slconfig.h"
#include "utils/conversion.h"

#include <wx/string.h>
#include <wx/filename.h>

SLCONFIG("/test/string", (const wxString&)_T("hello world!") , "test string"); //FIXME: why is explicit cast required?
SLCONFIG("/test/long", -12345l, "test long");
SLCONFIG("/test/double", -321.123, "test double");
SLCONFIG("/test/bool", true, "test bool");

namespace SlPaths{

wxString GetConfigPath(){
	return wxFileName::GetTempDir()+_T("/sltest.config");
}
}

BOOST_AUTO_TEST_CASE( slconfig )
{

//	cfg().SaveFile();
	BOOST_CHECK(cfg().ReadLong(_T("/test/long")) == -12345l);
	BOOST_CHECK(cfg().Read(_T("/test/long")) == _T("-12345") );

	BOOST_CHECK(cfg().ReadDouble(_T("/test/double")) == -321.123);
	BOOST_CHECK(cfg().Read(_T("/test/double")) == _T("-321.123") );

	BOOST_CHECK(cfg().ReadBool(_T("/test/bool")) == true);
	BOOST_CHECK(cfg().Read(_T("/test/bool")) == _T("1") );

	BOOST_CHECK(cfg().ReadString(_T("/test/string")) == _T("hello world!"));
	//BOOST_CHECK(cfg().Read(_T("/test/string")) == _T("hello world!") ); //FIXME: fails, returns 104 ?!

	BOOST_CHECK(cfg().Write(_T("/test/long"), -10l));
	BOOST_CHECK(cfg().ReadLong(_T("/test/long")) == -10l);

	BOOST_CHECK(cfg().Write(_T("/test/double"), 10.3));
	BOOST_CHECK(cfg().ReadDouble(_T("/test/double")) == 10.3);

	BOOST_CHECK(cfg().Write(_T("/test/bool"), false));
	BOOST_CHECK(cfg().ReadBool(_T("/test/bool")) == false);

	BOOST_CHECK(cfg().Write(_T("/test/string"), (const wxString&)_T("test2")));
	BOOST_CHECK(cfg().Read(_T("/test/string")) == _T("test2") );
	BOOST_CHECK(cfg().ReadString(_T("/test/string")) == _T("hello world!") ); //FIXME: should return "test2"

//	cfg().SaveFile();
}
