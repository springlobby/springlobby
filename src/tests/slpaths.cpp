/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#define BOOST_TEST_MODULE slconfig

#include <boost/test/unit_test.hpp>
#include <stdlib.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <string>

#include "testingstuff/silent_logger.h"
#include "utils/conversion.h"
#include "utils/platform.h"
#include "utils/slpaths.h"
#include "utils/sortutil.h"

using std::string;

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

bool CheckVersion(const std::string& v1, const std::string& v2)
{
	return SlPaths::VersionSyncCompatible(v1, v2) && SlPaths::VersionSyncCompatible(v2, v1);
}

bool CheckCompareVersionStrings(const string& lhs, const string& rhs, int expected)
{
	int result = CompareVersionStrings(lhs, rhs);
	if (result != expected) {
		std::cout << "Compare version strings: \"" << lhs << "\" ? \"" << rhs << "\": Got: "
		          << result << " expected: " << expected << std::endl;
	}
	return expected == result;
}
BOOST_AUTO_TEST_CASE(VersionCheck)
{
	std::string v1 = "104.0.1-104-g2135e2e maintenance";
	std::string v2 = "104";
	std::string v3 = "105.0";
	std::string v4 = "104.0.1-448-g243c5dd develop";
	std::string v5 = "104.0.1-448-g243c5dd";
	std::string v6 = "104.0";
	std::string v7 = "";

	BOOST_CHECK(CheckVersion(v6, v6));
	BOOST_CHECK(!CheckVersion(v6, v4));
	BOOST_CHECK(!CheckVersion(v6, v1));
	BOOST_CHECK(!CheckVersion(v6, v2));
	BOOST_CHECK(!CheckVersion(v5, v4));
	BOOST_CHECK(!CheckVersion(v2, v7));

	BOOST_CHECK(CheckCompareVersionStrings("0.264", "0.264-43-gd87151a2", 1));
	BOOST_CHECK(CheckCompareVersionStrings("0.264-43-gd87151a2", "0.264", -1));
	BOOST_CHECK(CheckCompareVersionStrings("0.264", "0.264", 0));
	BOOST_CHECK(CheckCompareVersionStrings("0.264-43-gd87151a2", "0.264-43-gd87151a2", 0));
	BOOST_CHECK(CheckCompareVersionStrings("0.264-43-gd87151a2", "0.264-42-gd87151a2", -1));
	BOOST_CHECK(CheckCompareVersionStrings("0.264-43-gd87151a2", "0.264-43-gd17151a2", -1));
	BOOST_CHECK(CheckCompareVersionStrings("", "", 0));
	BOOST_CHECK(CheckCompareVersionStrings("0.", "", -1));
	BOOST_CHECK(CheckCompareVersionStrings("spring 100.0", "spring 98.0", -1));
	BOOST_CHECK(CheckCompareVersionStrings("100", "98", -1));

	std::array<string, 5> versions = {"spring 100.0", "spring 98.0", "spring 104.0", "spring 99.0", "spring 103.0"};
	std::array<string, 5> sorted   = {"spring 98.0",  "spring 99.0", "spring 100.0", "spring 103.0", "spring 104.0"};

	std::sort(versions.begin(), versions.end(), compareVersionStringsFunctor);
	BOOST_CHECK(sorted == versions);
	for (auto a : versions) {
		std::cout << a << " ";
	}
	std::cout << '\n';
}
