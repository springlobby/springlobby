#define BOOST_TEST_MODULE slconfig
#include <boost/test/unit_test.hpp>

#include "helper/slconfig.h"

#include <wx/string.h>
#include <wx/filename.h>

SLCONFIG("/test/string", "test" , "test string");
SLCONFIG("/test/long", 1l, "test long");
SLCONFIG("/test/double", 2.0, "test double");
SLCONFIG("/test/bool", true, "test bool");

namespace SlPaths{

wxString GetConfigPath(){
	return wxFileName::GetTempDir()+_T("/sltest.config");
}
}

BOOST_AUTO_TEST_CASE( slconfig )
{
	BOOST_CHECK(true);

	BOOST_CHECK(cfg().ReadString(_T("/test/string")) == _T("test"));
	BOOST_CHECK(cfg().ReadString(_T("/test/long")) == 1l);
	BOOST_CHECK(cfg().ReadString(_T("/test/double")) == 2.0);
	BOOST_CHECK(cfg().ReadString(_T("/test/bool")) == true);
}
