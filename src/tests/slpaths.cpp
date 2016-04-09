/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define BOOST_TEST_MODULE slconfig
#include <boost/test/unit_test.hpp>

#include "testingstuff/silent_logger.h"

#include "utils/slpaths.h"
#include "utils/platform.h"
#include "utils/conversion.h"

#include <stdlib.h>

#include <wx/string.h>
#include <wx/filename.h>

#ifdef WIN32
#define DELIM "\\"
#else
#define DELIM "/"
#endif

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

BOOST_AUTO_TEST_CASE(slpaths)
{
	const std::string testdir = "testdir";
	const std::string subdirs = testdir + DELIM + "dir1" + DELIM + "dir2";
	const std::string subdirs2 = testdir + DELIM + "dir3" + DELIM + "dir4";

	BOOST_CHECK(SlPaths::mkDir(subdirs));
	BOOST_CHECK(SlPaths::mkDir(subdirs2));
	BOOST_CHECK_MESSAGE(wxFileName::DirExists(TowxString(subdirs)), "dir doesn't exist: " + subdirs);
	BOOST_CHECK(SlPaths::RmDir(testdir));
	BOOST_CHECK(SlPaths::SantinizeFilename(":<>test:end") == std::string("___test_end"));
}
