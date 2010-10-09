#ifndef TASSERVERTOKENTABLE_H_INCLUDED
#define TASSERVERTOKENTABLE_H_INCLUDED

#include <wx/string.h>
#include <map>

static std::map<wxString,wxString> m_command_alias;
static std::map<wxString,wxString> m_send_command_alias;

void TASServer::FillAliasMap()
{
		m_command_alias[_T("#")] = _T("PING");
		m_command_alias[_T("$")] = _T("PONG");
		m_command_alias[_T("%")] = _T("TASSERVER");
		m_command_alias[_T("&")] = _T("REQUESTUPDATEFILE");
		m_command_alias[_T("'")] = _T("REGISTER");
		m_command_alias[_T("(")] = _T("REGISTRATIONDENIED");
		m_command_alias[_T(")")] = _T("REGISTRATIONACCEPTED");
		m_command_alias[_T("*")] = _T("RENAMEACCOUNT");
		m_command_alias[_T("+")] = _T("CHANGEPASSWORD");
		m_command_alias[_T(",")] = _T("LOGIN");
		m_command_alias[_T("-")] = _T("ACCEPTED");
		m_command_alias[_T(".")] = _T("DENIED");
		m_command_alias[_T("/")] = _T("LOGININFOEND");
		m_command_alias[_T("0")] = _T("AGREEMENT");
		m_command_alias[_T("1")] = _T("AGREEMENTEND");
		m_command_alias[_T("2")] = _T("CONFIRMAGREEMENT");
		m_command_alias[_T("3")] = _T("MOTD");
		m_command_alias[_T("4")] = _T("OFFERFILE");
		m_command_alias[_T("5")] = _T("UDPSOURCEPORT");
		m_command_alias[_T("6")] = _T("CLIENTIPPORT");
		m_command_alias[_T("7")] = _T("HOSTPORT");
		m_command_alias[_T("8")] = _T("SERVERMSG");
		m_command_alias[_T("9")] = _T("SERVERMSGBOX");
		m_command_alias[_T(":")] = _T("ADDUSER");
		m_command_alias[_T(";")] = _T("REMOVEUSER");
		m_command_alias[_T("<")] = _T("JOIN");
		m_command_alias[_T("=")] = _T("JOINFAILED");
		m_command_alias[_T(">")] = _T("CHANNELS");
		m_command_alias[_T("?")] = _T("CHANNEL");
		m_command_alias[_T("@")] = _T("ENDOFCHANNELS");
		m_command_alias[_T("A")] = _T("MUTELIST");
		m_command_alias[_T("B")] = _T("MUTELISTBEGIN");
		m_command_alias[_T("C")] = _T("MUTELISTEND");
		m_command_alias[_T("D")] = _T("CHANNELTOPIC");
		m_command_alias[_T("E")] = _T("CLIENTS");
		m_command_alias[_T("F")] = _T("JOINED");
		m_command_alias[_T("G")] = _T("LEAVE");
		m_command_alias[_T("H")] = _T("LEFT");
		m_command_alias[_T("I")] = _T("FORCELEAVECHANNEL");
		m_command_alias[_T("J")] = _T("CHANNELMESSAGE");
		m_command_alias[_T("K")] = _T("SAY");
		m_command_alias[_T("L")] = _T("SAID");
		m_command_alias[_T("M")] = _T("SAYEX");
		m_command_alias[_T("N")] = _T("SAIDEX");
		m_command_alias[_T("O")] = _T("SAYPRIVATE");
		m_command_alias[_T("P")] = _T("SAIDPRIVATE");
		m_command_alias[_T("Q")] = _T("OPENBATTLE");
		m_command_alias[_T("R")] = _T("BATTLEOPENED");
		m_command_alias[_T("S")] = _T("BATTLECLOSED");
		m_command_alias[_T("T")] = _T("JOINBATTLE");
		m_command_alias[_T("U")] = _T("JOINEDBATTLE");
		m_command_alias[_T("V")] = _T("LEFTBATTLE");
		m_command_alias[_T("W")] = _T("LEAVEBATTLE");
		m_command_alias[_T("X")] = _T("JOINBATTLEFAILED");
		m_command_alias[_T("Y")] = _T("OPENBATTLEFAILED");
		m_command_alias[_T("Z")] = _T("UPDATEBATTLEINFO");
		m_command_alias[_T("[")] = _T("SAYBATTLE");
		m_command_alias[_T("\\")] = _T("SAIDBATTLE");
		m_command_alias[_T("]")] = _T("SAYBATTLEEX");
		m_command_alias[_T("^")] = _T("SAIDBATTLEEX");
		m_command_alias[_T("_")] = _T("MYSTATUS");
		m_command_alias[_T("`")] = _T("CLIENTSTATUS");
		m_command_alias[_T("a")] = _T("MYBATTLESTATUS");
		m_command_alias[_T("b")] = _T("CLIENTBATTLESTATUS");
		m_command_alias[_T("c")] = _T("REQUESTBATTLESTATUS");
		m_command_alias[_T("d")] = _T("HANDICAP");
		m_command_alias[_T("e")] = _T("KICKFROMBATTLE");
		m_command_alias[_T("f")] = _T("FORCEQUITBATTLE");
		m_command_alias[_T("g")] = _T("FORCETEAMNO");
		m_command_alias[_T("h")] = _T("FORCEALLYNO");
		m_command_alias[_T("i")] = _T("FORCETEAMCOLOR");
		m_command_alias[_T("j")] = _T("FORCESPECTATORMODE");
		m_command_alias[_T("k")] = _T("DISABLEUNITS");
		m_command_alias[_T("l")] = _T("ENABLEUNITS");
		m_command_alias[_T("m")] = _T("ENABLEALLUNITS");
		m_command_alias[_T("n")] = _T("RING");
		m_command_alias[_T("o")] = _T("REDIRECT");
		m_command_alias[_T("p")] = _T("BROADCAST");
		m_command_alias[_T("q")] = _T("ADDBOT");
		m_command_alias[_T("r")] = _T("REMOVEBOT");
		m_command_alias[_T("s")] = _T("UPDATEBOT");
		m_command_alias[_T("t")] = _T("ADDSTARTRECT");
		m_command_alias[_T("u")] = _T("REMOVESTARTRECT");
		m_command_alias[_T("v")] = _T("MAPGRADES");
		m_command_alias[_T("w")] = _T("MAPGRADESFAILED");
		m_command_alias[_T("x")] = _T("SCRIPTSTART");
		m_command_alias[_T("y")] = _T("SCRIPT");
		m_command_alias[_T("z")] = _T("SCRIPTEND");
		m_command_alias[_T("{")] = _T("SETSCRIPTTAGS");
		m_command_alias[_T("|")] = _T("TESTLOGIN");
		m_command_alias[_T("}")] = _T("TESTLOGINACCEPT");
		m_command_alias[_T("~")] = _T("TESTLOGINDENY");
		m_command_alias[_T("")] = _T("ACQUIREUSERID");
		m_command_alias[_T("")] = _T("USERID");
		for ( std::map<wxString,wxString>::const_iterator i = m_command_alias.begin(); i != m_command_alias.end(); ++i) m_send_command_alias[i->second] = i->first; //swap content for second map
}

// returns original if not found
wxString DecodeTokenMessage( const wxString& cmd )
{
		std::map<wxString,wxString>::const_iterator it = m_command_alias.find( cmd );
		if ( it != m_command_alias.end() ) return it->second;
		else return cmd;
}

// returns original if not found
wxString EncodeTokenMessage( const wxString& cmd )
{
		std::map<wxString,wxString>::const_iterator it = m_send_command_alias.find( cmd );
		if ( it != m_send_command_alias.end() ) return it->second;
		else return cmd;
}

#endif // TASSERVERTOKENTABLE_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

