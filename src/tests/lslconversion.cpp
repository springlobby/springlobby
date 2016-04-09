/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define BOOST_TEST_MODULE lobbyid
#include <boost/test/unit_test.hpp>

#include "testingstuff/silent_logger.h"

#include <stdio.h>
#include <wx/colour.h>
#include <lslutils/misc.h>
#include <lslutils/conversion.h>

#include "user.h"
#include "utils/lslconversion.h"
#include "utils/conversion.h"
#include "utils/tasutil.h"

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

void test_wxColourTolsl(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
{
	const wxColor wxcol(red, green, blue, alpha);
	LSL::lslColor lslcol = wxColourTolsl(wxcol);
	BOOST_CHECK(lslcol.Red() == red);
	BOOST_CHECK(lslcol.Green() == green);
	BOOST_CHECK(lslcol.Blue() == blue);
	BOOST_CHECK(lslcol.Alpha() == alpha);
}

void test_lslTowxColour(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
{
	const LSL::lslColor col(red, green, blue, alpha);
	wxColor wxcol = lslTowxColour(col);
	BOOST_CHECK(wxcol.Red() == red);
	BOOST_CHECK(wxcol.Green() == green);
	BOOST_CHECK(wxcol.Blue() == blue);
	BOOST_CHECK(wxcol.Alpha() == alpha);
}

void test_lslToLobbyColour(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
{
	const LSL::lslColor col(red, green, blue, alpha);

	BOOST_CHECK(col.Red() == red);
	BOOST_CHECK(col.Green() == green);
	BOOST_CHECK(col.Blue() == blue);
	BOOST_CHECK(col.Alpha() == alpha);
	const LSL::lslColor col2(col.GetLobbyColor());

	BOOST_CHECK(col2.Red() == red);
	BOOST_CHECK(col2.Green() == green);
	BOOST_CHECK(col2.Blue() == blue);
	BOOST_CHECK(col2.Alpha() == 255);
}

BOOST_AUTO_TEST_CASE(lslconversion)
{
	test_wxColourTolsl(0, 0, 0, 0);
	test_wxColourTolsl(1, 2, 3, 4);
	test_wxColourTolsl(255, 255, 255, 255);

	test_lslTowxColour(0, 0, 0, 0);
	test_lslTowxColour(1, 2, 3, 4);
	test_lslTowxColour(255, 255, 255, 255);

	test_lslToLobbyColour(0, 0, 0, 0);
	test_lslToLobbyColour(1, 2, 3, 4);
	test_lslToLobbyColour(255, 255, 255, 255);

	LSL::StringVector test;
	test.push_back("line1");
	test.push_back("line2");
	test.push_back("line3");

	wxArrayString tmp = lslTowxArrayString(test);
	LSL::StringVector tmp2 = wxArrayStringToLSL(tmp);

	BOOST_CHECK(test.size() == tmp2.size());
	BOOST_CHECK(test[0] == tmp2[0]);
	BOOST_CHECK(test[1] == tmp2[1]);
	BOOST_CHECK(test[2] == tmp2[2]);
}

BOOST_AUTO_TEST_CASE(battlestatus)
{
	UserBattleStatus bs;
	BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));
	bs.ready = false;
	BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));

	for (int i = 0; i <= 15; i++) {
		bs.team = i;
		BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));
	}

	for (int i = 0; i <= 15; i++) {
		bs.ally = i;
		BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));
	}

	bs.spectator = true;
	BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));

	for (int i = 0; i <= 100; i++) {
		bs.handicap = i;
		BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));
	}

	for (int i = 0; i <= 2; i++) {
		bs.sync = i;
		BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));
	}

	for (int i = 0; i <= 15; i++) {
		bs.side = i;
		BOOST_CHECK(bs == UserBattleStatus::FromInt(UserBattleStatus::ToInt(bs)));
	}
}

BOOST_AUTO_TEST_CASE(colorconv)
{
	const std::string whitestr("1 1 1");
	const LSL::lslColor whitecol(255, 255, 255);

	const std::string blackstr("0 0 0");
	const LSL::lslColor blackcol(0, 0, 0);

	const std::string greystr("0.501961 0.501961 0.501961");
	const LSL::lslColor greycol(128, 128, 128);

	BOOST_CHECK(greycol.Red() == 128);


	BOOST_CHECK(whitecol == LSL::lslColor::FromFloatString(whitestr));
	BOOST_CHECK_MESSAGE(whitestr == LSL::lslColor::ToFloatString(whitecol), LSL::lslColor::ToFloatString(whitecol));

	BOOST_CHECK(blackcol == LSL::lslColor::FromFloatString(blackstr));
	BOOST_CHECK_MESSAGE(blackstr == LSL::lslColor::ToFloatString(blackcol), LSL::lslColor::ToFloatString(blackcol));

	BOOST_CHECK(greycol == LSL::lslColor::FromFloatString(greystr));
	BOOST_CHECK_MESSAGE(greystr == LSL::lslColor::ToFloatString(greycol), LSL::lslColor::ToFloatString(greycol));


	for (int c = 0; c < 3; c++) {
		float r = 0.f, g = 0.f, b = 0.f;
		for (int i = 0; i < 256; i++) {
			if (c == 0) {
				r = i;
			} else if (c == 1) {
				g = i;
			} else {
				b = i;
			}
			const LSL::lslColor col(r, g, b);
			const std::string res = LSL::lslColor::ToFloatString(col);
			const LSL::lslColor rescol = LSL::lslColor::FromFloatString(res);
			BOOST_CHECK(rescol == col);
		}
	}
}

BOOST_AUTO_TEST_CASE(userstatus)
{
	UserStatus us;
	BOOST_CHECK(us == UserStatus::FromInt(UserStatus::ToInt(us)));

	us.in_game = true;
	BOOST_CHECK(us == UserStatus::FromInt(UserStatus::ToInt(us)));

	us.away = true;
	BOOST_CHECK(us == UserStatus::FromInt(UserStatus::ToInt(us)));

	us.rank = UserStatus::RANK_1;
	BOOST_CHECK(us == UserStatus::FromInt(UserStatus::ToInt(us)));

	us.rank = UserStatus::RANK_8;
	BOOST_CHECK(us == UserStatus::FromInt(UserStatus::ToInt(us)));

	us.moderator = true;
	BOOST_CHECK(us == UserStatus::FromInt(UserStatus::ToInt(us)));

	us.bot = true;
	BOOST_CHECK(us == UserStatus::FromInt(UserStatus::ToInt(us)));
}

BOOST_AUTO_TEST_CASE(hashes)
{
	const unsigned int overflow = 2232970410; //this
	const int negative = -2147483648;

	BOOST_CHECK("-2061996886" == LSL::Util::MakeHashSigned(LSL::Util::ToIntString(overflow)));
	BOOST_CHECK("-2147483648" == LSL::Util::MakeHashSigned(LSL::Util::ToIntString(negative)));

	BOOST_CHECK("2232970410" == LSL::Util::MakeHashUnsigned(LSL::Util::ToIntString(overflow)));
	BOOST_CHECK("2147483648" == LSL::Util::MakeHashUnsigned(LSL::Util::ToIntString(negative)));

	BOOST_CHECK("2232970410" == LSL::Util::MakeHashUnsigned("-2061996886"));
	BOOST_CHECK("2147483648" == LSL::Util::MakeHashUnsigned("-2147483648"));

	BOOST_CHECK(LSL::Util::MakeHashUnsigned("3807049253") == LSL::Util::MakeHashUnsigned("-487918043"));
	BOOST_CHECK(LSL::Util::MakeHashUnsigned("-2567621") == LSL::Util::MakeHashUnsigned("4292399675"));

	BOOST_CHECK(LSL::Util::MakeHashSigned("3807049253") == LSL::Util::MakeHashSigned("-487918043"));
	BOOST_CHECK(LSL::Util::MakeHashSigned("-2567621") == LSL::Util::MakeHashSigned("4292399675"));
}

BOOST_AUTO_TEST_CASE(convertfloat)
{
	const float a = 1.0f;
	const float b = -1.5f;
	const float c = 1.999f;

	BOOST_CHECK(a == LSL::Util::FromFloatString("1.0"));
	BOOST_CHECK(b == LSL::Util::FromFloatString("-1.5"));
	BOOST_CHECK(c == LSL::Util::FromFloatString("1.999"));

	BOOST_CHECK(a == LSL::Util::FromFloatString(LSL::Util::ToFloatString(a)));
	BOOST_CHECK(b == LSL::Util::FromFloatString(LSL::Util::ToFloatString(b)));
	BOOST_CHECK(c == LSL::Util::FromFloatString(LSL::Util::ToFloatString(c)));
}

BOOST_AUTO_TEST_CASE(tokenize)
{
	std::vector<std::string> v;
	v = LSL::Util::StringTokenize("", " ");
	BOOST_CHECK(v.empty());

	v = LSL::Util::StringTokenize("0 1 2", " ");
	BOOST_CHECK(v.size() == 3);
	BOOST_CHECK(v[0] == "0");
	BOOST_CHECK(v[1] == "1");
	BOOST_CHECK(v[2] == "2");

	v = LSL::Util::StringTokenize(" 0 1 2 ", " ");
	BOOST_CHECK(v.size() == 3);
	BOOST_CHECK(v[0] == "0");
	BOOST_CHECK(v[1] == "1");
	BOOST_CHECK(v[2] == "2");

	v = LSL::Util::StringTokenize(" 0:1 2 ", " :");
	BOOST_CHECK(v.size() == 3);
	BOOST_CHECK(v[0] == "0");
	BOOST_CHECK(v[1] == "1");
	BOOST_CHECK(v[2] == "2");

	v = LSL::Util::StringTokenize(" ", " :");
	BOOST_CHECK(v.size() == 0);

	v = LSL::Util::StringTokenize(" 0", " :");
	BOOST_CHECK(v.size() == 1);
	BOOST_CHECK(v[0] == "0");

	v = LSL::Util::StringTokenize(" 0 ", " :");
	BOOST_CHECK(v.size() == 1);
	BOOST_CHECK(v[0] == "0");

	v = LSL::Util::StringTokenize("0 ", " :");
	BOOST_CHECK(v.size() == 1);
	BOOST_CHECK(v[0] == "0");
}


BOOST_AUTO_TEST_CASE(tasutils)
{
	std::string input = "BATTLECLOSED 18172";
	BOOST_CHECK(GetWordParam(input) == "BATTLECLOSED");
	BOOST_CHECK(GetIntParam(input) == 18172);
	BOOST_CHECK(input.empty());

	input = "A B C";

	BOOST_CHECK(GetParamByChar(input, ' ') == "A");
	BOOST_CHECK(GetParamByChar(input, ' ') == "B");
	BOOST_CHECK(GetParamByChar(input, ' ') == "C");
	BOOST_CHECK(GetParamByChar(input, ' ').empty());
	BOOST_CHECK(input.empty());

	input = "A\tB\tC";
	BOOST_CHECK(GetSentenceParam(input) == "A");
	BOOST_CHECK(GetSentenceParam(input) == "B");
	BOOST_CHECK(GetSentenceParam(input) == "C");
	BOOST_CHECK(GetSentenceParam(input).empty());
	BOOST_CHECK(input.empty());

	input = "1 0 1";
	BOOST_CHECK(GetBoolParam(input));
	BOOST_CHECK(!GetBoolParam(input));
	BOOST_CHECK(GetBoolParam(input));
	BOOST_CHECK(!GetBoolParam(input)); //input is already empty
	BOOST_CHECK(input.empty());

	BOOST_CHECK(LSL::Util::ToLower("AbCdEfghIJ") == "abcdefghij");
	BOOST_CHECK(LSL::Util::ToLower("A") == "a");
	BOOST_CHECK(LSL::Util::ToLower("") == "");

	BOOST_CHECK(LSL::Util::FromLongString("1238129312390") == 1238129312390l);
	BOOST_CHECK(LSL::Util::FromLongString("-1238129312390") == -1238129312390l);
	BOOST_CHECK(LSL::Util::FromLongString("") == 0l);

	BOOST_CHECK(LSL::Util::FromIntString("2147483647") == INT_MAX);
	//Check for overflow
	BOOST_CHECK(LSL::Util::FromIntString("2147483648") == INT_MIN);
	BOOST_CHECK(LSL::Util::FromIntString("-2147483647") == INT_MIN + 1);
	BOOST_CHECK(LSL::Util::FromIntString("") == 0);
}
