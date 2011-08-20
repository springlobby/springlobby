/* Copyright (C) 2007-2011 The SpringLobby Team. All rights reserved. */


#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/string.h>
#include <wx/regex.h>
#include <wx/intl.h>
#include <wx/protocol/http.h>
#include <wx/socket.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include <wx/platinfo.h>
#include <wx/stopwatch.h>

#include <stdexcept>
#include <algorithm>

#include <map>

#include "base64.h"
#include "utils/md5.h"
#include "tasserver.h"
#include "springunitsync.h"
#include "user.h"
#include "utils/debug.h"
#include "utils/tasutil.h"
#include "utils/conversion.h"
#include "utils/platform.h"
#include "updater/updatehelper.h"
#include "battle.h"
#include "serverevents.h"
#include "socket.h"
#include "channel/channel.h"
#include "tasservertokentable.h"
#include "pingthread.h"

// for SL_MAIN_ICON
#include "utils/customdialogs.h"

#include "settings.h"


const int udp_reply_timeout=10;


//! @brief Struct used internally by the TASServer class to get client status information.
struct TASClientstatus
{
unsigned int in_game   :
    1;
unsigned int away      :
    1;
unsigned int rank      :
    3;
unsigned int moderator :
    1;
unsigned int bot       :
    1;
};


//! @brief Union used internally by the TASServer class to get client status information.
union UTASClientStatus
{
    unsigned char byte;
    TASClientstatus tasdata;
};


//! @brief Struct used internally by the TASServer class to get battle status information.
//!TODO is that last member necessary? throws a warning baout bein used uninited
struct TASBattleStatus
{
unsigned int :
    1;
unsigned int ready :
    1;
unsigned int team :
    4;
unsigned int ally :
    4;
unsigned int player :
    1;
unsigned int handicap:
    7;
unsigned int :
    4;
unsigned int sync :
    2;
unsigned int side :
    4;
unsigned int :
    4;
};

//! @brief Union used internally by the TASServer class to get battle status information.
union UTASBattleStatus
{
    int data;
    TASBattleStatus tasdata;
};

//! @brief struct used internallby by tasserver to convert offer file bitfields
struct OfferFileData
{
	bool autoopen :
		1;
	bool closelobbyondownload :
		1;
	bool disconnectonrefuse :
		1;
};

//! @brief Union used internally by the TASServer class to get battle status information.
union UTASOfferFileData
{
    int data;
    OfferFileData tasdata;
};


struct TASColor
{
unsigned int red :
    8;
unsigned int green :
    8;
unsigned int blue :
    8;
unsigned int zero:
    8;
};


union UTASColor
{
    int data;
    TASColor color;
};


/*

myteamcolor:  Should be 32-bit signed integer in decimal form (e.g. 255 and not FF) where each color channel should occupy 1 byte (e.g. in hexdecimal: $00BBGGRR, B = blue, G = green, R = red). Example: 255 stands for $000000FF.

*/

UserStatus ConvTasclientstatus( TASClientstatus );
UserBattleStatus ConvTasbattlestatus( TASBattleStatus );
TASBattleStatus ConvTasbattlestatus( UserBattleStatus );
IBattle::StartType IntToStartType( int start );
NatType IntToNatType( int nat );
IBattle::GameType IntToGameType( int gt );

TASServer::TASServer(int serverEventsMode):
m_ping_thread(0),
m_ser_ver(0),
m_connected(false),
m_online(false),
m_debug_dont_catch( false ),
m_id_transmission( true ),
m_redirecting( false ),
m_buffer(_T("")),
m_last_udp_ping(0),
m_last_net_packet(0),
m_udp_private_port(0),
m_battle_id(-1),
m_server_lanmode(false),
m_account_id_count(0),
m_do_finalize_join_battle(false),
m_finalize_join_battle_id(-1),
m_token_transmission( false )
{
	  m_se = IServerEvents::getInstance( *this, IServerEvents::ServerEventsMode(serverEventsMode) );
	  FillAliasMap();
	  m_relay_host_manager_list.Clear();
}

TASServer::~TASServer()
{
    Disconnect();
    delete m_se;
}


bool TASServer::ExecuteSayCommand( const wxString& cmd )
{
    wxArrayString arrayparams = wxStringTokenize( cmd, _T(" ") );
    if ( arrayparams.GetCount() == 0 ) return false;
    wxString subcmd = arrayparams[0];
    wxString params = cmd.AfterFirst( ' ' );
    if ( subcmd == _T("/ingame") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("GETINGAMETIME"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/kick") )
    {
				if ( arrayparams.GetCount() < 2 ) return false;
        SendCmd( _T("KICKUSER"), params );
        return true;
    }
    else if ( subcmd == _T("/ban") )
    {
				if ( arrayparams.GetCount() < 2 ) return false;
        SendCmd( _T("BAN"), params );
        return true;
    }
    else if ( subcmd == _T("/unban") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("UNBAN"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/banlist") )
    {
        SendCmd( _T("BANLIST") );
        return true;
    }
    else if ( subcmd == _T("/topic") )
    {
				params.Replace( _T("\n"), _T("\\n") );
        SendCmd( _T("CHANNELTOPIC"), params );
        return true;
    }
    else if ( subcmd == _T("/chanmsg") )
    {
				if ( arrayparams.GetCount() < 2 ) return false;
        SendCmd( _T("CHANNELMESSAGE"), params );
        return true;
    }
    else if ( subcmd == _T("/ring") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("RING"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/ip") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("GETIP"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/mute") )
    {
				if ( arrayparams.GetCount() < 4 ) return false;
				if ( arrayparams.GetCount() > 5 ) return false;
        SendCmd( _T("MUTE"), params );
        return true;
    }
    else if ( subcmd == _T("/unmute") )
    {
				if ( arrayparams.GetCount() != 3 ) return false;
        SendCmd( _T("UNMUTE"), arrayparams[1] + _T(" ") + arrayparams[2] );
        return true;
    }
    else if ( subcmd == _T("/mutelist") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("MUTELIST"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/lastlogin") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("GETLASTLOGINTIME"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/findip") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("FINDIP"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/lastip") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("GETLASTIP"), arrayparams[1] );
        return true;
    }
    else if ( subcmd == _T("/rename") )
    {
				if ( arrayparams.GetCount() != 2 ) return false;
        SendCmd( _T("RENAMEACCOUNT"), arrayparams[1] );
        sett().SetServerAccountNick( sett().GetDefaultServer(), arrayparams[1] ); // this code assumes that default server hasn't changed since login ( like it should atm )
        return true;
    }
    else if ( subcmd == _T("/testmd5") )
    {
        ExecuteCommand( _T("SERVERMSG"), GetPasswordHash(params) );
        return true;
    }
    else if ( subcmd == _T("/hook") )
    {
        SendCmd( _T("HOOK"), params );
        return true;
    }
    else if ( subcmd == _T("/quit") )
    {
        Disconnect();
        return true;
    }
    else if ( subcmd == _T("/changepassword2") )
    {
		if ( arrayparams.GetCount() < 1 ) return false;
		wxString oldpassword = sett().GetServerAccountPass( GetServerName() );
		wxString newpassword = GetPasswordHash( params );
		if  ( oldpassword.IsEmpty() || !sett().GetServerAccountSavePass(GetServerName()) )
		{
			m_se->OnServerMessage(_("There is no saved password for this account, please use /changepassword"));
			return true;
		}
        SendCmd( _T("CHANGEPASSWORD"), oldpassword + _T(" ") + newpassword );
        return true;
    }
    else if ( subcmd == _T("/changepassword") )
    {
		if ( arrayparams.GetCount() != 2 ) return false;
		wxString oldpassword = GetPasswordHash(arrayparams[1]);
		wxString newpassword = GetPasswordHash( arrayparams[2] );
        SendCmd( _T("CHANGEPASSWORD"), oldpassword + _T(" ") + newpassword );
        return true;
    }
    else if ( subcmd == _T("/ping") )
    {
				Ping();
        return true;
    }
    return false;
}


void TASServer::Connect( const wxString& servername ,const wxString& addr, const int port )
{
	m_server_name = servername;
    m_addr=addr;
	m_buffer = _T("");
	m_sock->Connect( addr, port );
	m_sock->SetSendRateLimit( 800 ); // 1250 is the server limit but 800 just to make sure :)
	m_connected = false;
    m_online = false;
    m_redirecting = false;
    m_agreement = _T("");
	m_crc.ResetCRC();
	m_last_net_packet = time( 0 );
	wxString handle = m_sock->GetHandle();
	if ( !handle.IsEmpty() ) m_crc.UpdateData( STD_STRING( wxString( handle + m_addr ) ) );
}

void TASServer::Disconnect()
{
    if (!m_connected)
    {
        return;
    }
    SendCmd( _T("EXIT") ); // EXIT command for new protocol compatibility
	m_sock->Disconnect();
}

bool TASServer::IsConnected()
{
	return (m_sock->State() == SS_Open);
}


bool TASServer::Register( const wxString& addr, const int port, const wxString& nick, const wxString& password, wxString& reason )
{
    wxLogDebugFunc( _T("") );
	FakeNetClass temp;
	Socket tempsocket( temp, true, true );
    tempsocket.Connect( addr, port );
    if ( tempsocket.State() != SS_Open ) return false;

    wxString data = tempsocket.Receive().BeforeLast(_T('\n'));
    if ( data.Find( _T("\r") ) != wxNOT_FOUND ) data = data.BeforeLast(_T('\r'));
    if ( GetWordParam( data ) != _T("TASServer") ) return false;

    tempsocket.Send( _T("REGISTER ") + nick + _T(" ") + GetPasswordHash( password ) + _T("\n") );

    data = tempsocket.Receive().BeforeLast(_T('\n'));
    tempsocket.Disconnect();
    if ( data.Find( _T("\r") ) != wxNOT_FOUND ) data = data.BeforeLast(_T('\r'));
    if ( data.IsEmpty() )
    {
        reason = _("Connection timed out");
        return false;
    }
    wxString cmd = GetWordParam( data );
    if ( cmd == _T("REGISTRATIONACCEPTED"))
    {
        return true;
    }
    else if ( cmd == _T("REGISTRATIONDENIED") )
    {
        reason = data;
        return false;
    }
    reason = _("Unknown answer from server");
    return false;
}


bool TASServer::IsPasswordHash( const wxString& pass ) const
{
    return pass.length() == 24 && pass[22] == '=' && pass[23] == '=';
}


wxString TASServer::GetPasswordHash( const wxString& pass ) const
{
    if ( IsPasswordHash(pass) ) return pass;

    md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;

    std::string str = STD_STRING(pass);
    char* cstr = new char [str.size()+1];
    strcpy (cstr, str.c_str());

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *) cstr, strlen( cstr ));
	md5_finish(&state, digest);
    for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);

    wxString coded = wxBase64::Encode( digest, 16 );
    return coded;
}


User& TASServer::GetMe() {
    return GetUser( m_user );
}
const User& TASServer::GetMe() const {
    return GetUser( m_user );
}


void TASServer::Login()
{
    wxLogDebugFunc( _T("") );
    wxString pass = GetPasswordHash( m_pass );
    wxString protocol = _T("\t") + TowxString( m_crc.GetCRC() );
    wxString localaddr;
	localaddr = m_sock->GetLocalAddress();
    if ( localaddr.IsEmpty() ) localaddr = _T("*");
	m_id_transmission = false;
    SendCmd ( _T("LOGIN"), m_user + _T(" ") + pass + _T(" ") +
			  GetHostCPUSpeed() + _T(" ") + localaddr + _T(" SpringLobby ") + GetSpringLobbyVersion(false) + protocol  + _T("\ta sp"));
	m_id_transmission = true;
}

void TASServer::Logout()
{
    wxLogDebugFunc( _T("") );
    Disconnect();
}

bool TASServer::IsOnline() const
{
    if ( !m_connected ) return false;
    return m_online;
}


void TASServer::RequestChannels()
{
    SendCmd( _T("CHANNELS") );
}


void TASServer::AcceptAgreement()
{
    SendCmd( _T("CONFIRMAGREEMENT") );
}


void TASServer::Update( int mselapsed )
{

	m_sock->OnTimer( mselapsed );

    if ( !m_connected )   // We are not formally connected yet, but might be.
    {
        if ( IsConnected() )
        {
            m_last_udp_ping = time( 0 );
            m_connected = true;
        }
        return;
    }
    else   // We are connected already.
    {
        if ( !IsConnected() ) return;

        time_t now = time( 0 );

//disabled until better timing is miplemented
//        if ( ( m_last_net_packet > 0 ) && ( ( now - m_last_net_packet ) > PING_TIMEOUT ) )
//        {
//					 m_se->OnServerMessage( _("Timeout assumed, disconnecting") );
//        	 Disconnect();
//        }

        // joining battle with nat traversal:
        // if we havent finalized joining yet, and udp_reply_timeout seconds has passed since
        // we did UdpPing(our name) , join battle anyway, but with warning message that nat failed.
        // (if we'd receive reply from server, we'd finalize already)
        //
        if (m_do_finalize_join_battle&&(m_last_udp_ping+udp_reply_timeout<now))
        {
            customMessageBoxNoModal(SL_MAIN_ICON,_("Failed to punch through NAT, playing this battle might not work for you or for other players."),_("Error"), wxICON_ERROR);
            //wxMessageBox()
            FinalizeJoinBattle();
            //wxMessageBox(_("Failed to punch through NAT"), _("Error"), wxICON_INFORMATION, NULL/* m_ui.mw()*/ );
        };

        if ( ( m_last_udp_ping + m_keepalive ) < now )
        {
            // Is it time for a nat traversal PING?
            m_last_udp_ping = now;
            // Nat travelsal "ping"
            if ( m_battle_id != -1 )
            {
                Battle *battle=GetCurrentBattle();
                if (battle)
                {
                    if ( ( battle->GetNatType() == NAT_Hole_punching || ( battle->GetNatType() == NAT_Fixed_source_ports ) ) && !battle->GetInGame() )
                    {
                        if ( battle->IsFounderMe() )
                        {
                            UdpPingTheServer(m_user);
                            UdpPingAllClients();
                        }
                        else
                        {
                            UdpPingTheServer(m_user);
                        }
                    }
                    else
                    {
                        // old logging for debug
                        //if(battle->GetNatType()!=NAT_Hole_punching)wxLogMessage( _T("pinging: current battle not using NAT_Hole_punching") );
                        //if(battle->GetInGame())wxLogMessage( _T("pinging: current battle is in game") );
                    }
                }
            }
        }
    }

}


void TASServer::ExecuteCommand( const wxString& in )
{
    wxLogMessage( _T("%s"), in.c_str()  );
    wxString cmd;
    wxString params = in;
    long replyid = 0;

    if ( in.empty() ) return;
    try
    {
        ASSERT_LOGIC( params.AfterFirst( '\n' ).IsEmpty(), _T("losing data") );
    }
    catch (...)
    {
        return;
    }
    if ( params[0] == '#' )
    {
		wxString id = params.BeforeFirst( ' ' ).AfterFirst( '#' );
        params = params.AfterFirst( ' ' );
        id.ToLong( &replyid );
    }
    cmd = params.BeforeFirst( ' ' );
		params = params.AfterFirst( ' ' );

		// decode message if tokenized
		wxString copy = cmd;
		cmd = DecodeTokenMessage( cmd );
		if ( copy != cmd ) m_token_transmission = true;
		cmd.MakeUpper();

    if ( m_debug_dont_catch )
    {
        ExecuteCommand( cmd, params, replyid );
    }
    else
    {
        try
        {
            ExecuteCommand( cmd, params, replyid );
        }
        catch ( ... ) // catch everything so the app doesn't crash, may makes odd beahviours but it's better than crashing randomly for normal users
        {
        }
    }
}


void TASServer::ExecuteCommand( const wxString& cmd, const wxString& inparams, int replyid )
{
    wxString params = inparams;
    int pos, cpu, id, nat, port, maxplayers, rank, specs, units, top, left, right, bottom, ally, type;
    bool haspass,lanmode = false;
    wxString hash;
    wxString nick, contry, host, map, title, mod, channel, error, msg, owner, ai, supported_spring_version, topic;
    //NatType ntype;
    UserStatus cstatus;
    UTASClientStatus tasstatus;
    UTASBattleStatus tasbstatus;
    UserBattleStatus bstatus;
    UTASColor color;

    if ( cmd == _T("TASSERVER"))
    {
        mod = GetWordParam( params );
        mod.ToDouble( &m_ser_ver );
        supported_spring_version = GetWordParam( params );
        m_nat_helper_port = (unsigned long)GetIntParam( params );
        lanmode = GetBoolParam( params );
        m_server_lanmode = lanmode;
        m_se->OnConnected( m_server_name, mod, (m_ser_ver > 0), supported_spring_version, lanmode );
    }
    else if ( cmd == _T("ACCEPTED") )
    {
		if ( m_online ) return; // in case is the server sends WTF
        m_online = true;
        m_user = params;
		m_ping_thread = new PingThread( *this, 10000 );
		m_ping_thread->Init();
        m_se->OnLogin( );
    }
    else if ( cmd == _T("MOTD") )
    {
        m_se->OnMotd( params );
    }
    else if ( cmd == _T("ADDUSER") )
    {
        nick = GetWordParam( params );
        contry = GetWordParam( params );
        cpu = GetIntParam( params );
        if ( params.IsEmpty() )
        {
        	// if server didn't send any account id to us, fill with an always increasing number
        	id = m_account_id_count;
        	m_account_id_count++;
        }
        else
        {
					id = GetIntParam( params );
        }
        m_se->OnNewUser( nick, contry, cpu, TowxString(id) );
        if ( nick == m_relay_host_bot )
        {
           RelayCmd( _T("OPENBATTLE"), m_delayed_open_command ); // relay bot is deployed, send host command
           m_delayed_open_command = _T("");
        }
    }
    else if ( cmd == _T("CLIENTSTATUS") )
    {
        nick = GetWordParam( params );
        tasstatus.byte = GetIntParam( params );
        cstatus = ConvTasclientstatus( tasstatus.tasdata );
        m_se->OnUserStatus( nick, cstatus );
    }
    else if ( cmd == _T("BATTLEOPENED") )
    {
        id = GetIntParam( params );
        type = GetIntParam( params );
        nat = GetIntParam( params );
        nick = GetWordParam( params );
        host = GetWordParam( params );
        port = GetIntParam( params );
        maxplayers = GetIntParam( params );
        haspass = GetBoolParam( params );
        rank = GetIntParam( params );
        hash = MakeHashUnsigned( GetWordParam( params ) );
        map = GetSentenceParam( params );
        title = GetSentenceParam( params );
        mod = GetSentenceParam( params );
        m_se->OnBattleOpened( id, (BattleType)type, IntToNatType( nat ), nick, host, port, maxplayers,
                              haspass, rank, hash, map, title, mod );
        if ( nick == m_relay_host_bot )
        {
		   GetBattle( id ).SetProxy( m_relay_host_bot );
           JoinBattle( id, sett().GetLastHostPassword() ); // autojoin relayed host battles
        }
    }
    else if ( cmd == _T("JOINEDBATTLE") )
    {
        id = GetIntParam( params );
        nick = GetWordParam( params );
		wxString userScriptPassword = GetWordParam( params );
		m_se->OnUserJoinedBattle( id, nick, userScriptPassword );
    }
    else if ( cmd == _T("UPDATEBATTLEINFO") )
    {
        id = GetIntParam( params );
        specs = GetIntParam( params );
        haspass = GetBoolParam( params );
        hash = MakeHashUnsigned( GetWordParam( params ) );
        map = GetSentenceParam( params );
        m_se->OnBattleInfoUpdated( id, specs, haspass, hash, map );
    }
    else if ( cmd == _T("LOGININFOEND") )
    {
				if ( sett().GetReportStats() )
				{
					wxString version = GetSpringLobbyVersion(false);
					wxString aux = GetSpringLobbyAuxVersion();
					aux.Replace( _T(" "), _T("") );
					aux = _T(" ") + aux;
					wxString os = wxPlatformInfo::Get().GetOperatingSystemIdName();
					os.Replace( _T(" "), _T("") );
					wxString wxversion = wxVERSION_STRING;
					wxversion.Replace( _T(" "), _T("") );
					wxString reportstring = _T("stats.report ") + version + _T(" ") + wxversion + _T(" ") + os + aux;
					if ( UserExists( _T("SL_bot") ) ) SayPrivate( _T("SL_bot"), reportstring );
				}
				if ( UserExists( _T("RelayHostManagerList") ) ) SayPrivate( _T("RelayHostManagerList"), _T("!lm") );
        m_se->OnLoginInfoComplete();
    }
    else if ( cmd == _T("REMOVEUSER") )
    {
        nick = GetWordParam( params );
        if ( nick == m_user ) return; // to prevent peet doing nasty stuff to you, watch your back!
        m_se->OnUserQuit( nick );
    }
    else if ( cmd == _T("BATTLECLOSED") )
    {
        id = GetIntParam( params );
        if ( m_battle_id == id ) m_relay_host_bot = _T("");
        m_se->OnBattleClosed( id );
    }
    else if ( cmd == _T("LEFTBATTLE") )
    {
        id = GetIntParam( params );
        nick = GetWordParam( params );
        m_se->OnUserLeftBattle( id, nick );
    }
    else if ( cmd == _T("PONG") )
    {
        HandlePong( replyid );
    }
    else if ( cmd == _T("JOIN") )
    {
        channel = GetWordParam( params );
        m_se->OnJoinChannelResult( true, channel, _T("") );
    }
    else if ( cmd == _T("JOIN") )
    {
        channel = GetWordParam( params );
        error = GetSentenceParam( params );
        m_se->OnJoinChannelResult( false, channel, error );
    }
    else if ( cmd == _T("SAID") )
    {
        channel = GetWordParam( params );
        nick = GetWordParam( params );
        m_se->OnChannelSaid( channel, nick, params );
    }
    else if ( cmd == _T("JOINED") )
    {
        channel = GetWordParam( params );
        nick = GetWordParam( params );
        m_se->OnUserJoinChannel( channel, nick );
    }
    else if ( cmd == _T("LEFT") )
    {
        channel = GetWordParam( params );
        nick = GetWordParam( params );
        msg = GetSentenceParam( params );
        m_se->OnChannelPart( channel, nick, msg );
    }
    else if ( cmd == _T("CHANNELTOPIC") )
    {
        channel = GetWordParam( params );
        nick = GetWordParam( params );
        pos = GetIntParam( params );
        params.Replace( _T("\\n"), _T("\n") );
        m_se->OnChannelTopic( channel, nick, params, pos/1000 );
    }
    else if ( cmd == _T("SAIDEX") )
    {
        channel = GetWordParam( params );
        nick = GetWordParam( params );
        m_se->OnChannelAction( channel, nick, params );
    }
    else if ( cmd == _T("CLIENTS") )
    {
        channel = GetWordParam( params );
        while ( (nick = GetWordParam( params )) != _T("") )
        {
            m_se->OnChannelJoin( channel, nick );
        }
    }
    else if ( cmd == _T("SAYPRIVATE") )
    {
        nick = GetWordParam( params );
        if ( ( ( nick == m_relay_host_bot ) || ( nick == m_relay_host_manager ) ) && params.StartsWith( _T("!") ) ) return; // drop the message
        if ( ( nick == _T("RelayHostManagerList") ) && ( params == _T("!lm") ) ) return;// drop the message
        if ( nick == _T("SL_bot") )
        {
        	if ( params.StartsWith( _T("stats.report") ) ) return;
        }
        m_se->OnPrivateMessage( nick, params, true );
    }
	else if ( cmd == _T("SAYPRIVATEEX") )
	{
		nick = GetWordParam( params );
		m_se->OnPrivateMessageEx( nick, params, true );
	}
    else if ( cmd == _T("SAIDPRIVATE") )
    {
        nick = GetWordParam( params );
		if ( nick == m_relay_host_bot )
		{
			if ( params.StartsWith(_T("JOINEDBATTLE")) )
			{
				GetWordParam( params ); // skip first word, it's the message itself
				id = GetIntParam( params );
				wxString usernick = GetWordParam( params );
				wxString userScriptPassword = GetWordParam( params );
				try
				{
					User& usr = GetUser(usernick);
					usr.BattleStatus().scriptPassword = userScriptPassword;
					Battle* battle = GetCurrentBattle();
					if (battle)
					{
						if ( battle->CheckBan( usr ) ) return;
					}
					SetRelayIngamePassword( usr );
				} catch (...) {}
				return;
			}
		}
        if ( nick == m_relay_host_manager )
        {
          if ( params.StartsWith( _T("\001") ) ) // error code
          {
            m_se->OnServerMessageBox( params.AfterFirst( _T(' ') ) );
          }
          else
          {
            m_relay_host_bot = params;
          }
          m_relay_host_manager = _T("");
          return;
        }
        if ( nick == _T("RelayHostManagerList") )
				{
					if  ( params.StartsWith(_T("list ")) )
					{
						 wxString list = params.AfterFirst( _T(' ') );
						 m_relay_host_manager_list = wxStringTokenize( list, _T("\t") );
						 return;
					}
				}
        m_se->OnPrivateMessage( nick, params, false );
    }
	else if ( cmd == _T("SAIDPRIVATEEX") )
	{
		nick = GetWordParam( params );
		m_se->OnPrivateMessageEx( nick, params, false );
	}
    else if ( cmd == _T("JOINBATTLE") )
    {
        id = GetIntParam( params );
        hash = MakeHashUnsigned( GetWordParam( params ) );
        m_battle_id = id;
		m_se->OnJoinedBattle( id, hash );
        m_se->OnBattleInfoUpdated( m_battle_id );
		try
		{
		 if (GetBattle(id).IsProxy()) RelayCmd(_T("SUPPORTSCRIPTPASSWORD")); // send flag to relayhost marking we support script passwords
		} catch(...) {}
    }
    else if ( cmd == _T("CLIENTBATTLESTATUS") )
    {
        nick = GetWordParam( params );
        tasbstatus.data = GetIntParam( params );
        bstatus = ConvTasbattlestatus( tasbstatus.tasdata );
        color.data = GetIntParam( params );
        bstatus.colour = wxColour( color.color.red, color.color.green, color.color.blue );
        m_se->OnClientBattleStatus( m_battle_id, nick, bstatus );
    }
    else if ( cmd == _T("ADDSTARTRECT") )
    {
        //ADDSTARTRECT allyno left top right bottom
        ally = GetIntParam( params );
        left = GetIntParam( params );
        top = GetIntParam( params );
        right = GetIntParam( params );
        bottom = GetIntParam( params );;
        m_se->OnBattleStartRectAdd( m_battle_id, ally, left, top, right, bottom );
    }
    else if ( cmd == _T("REMOVESTARTRECT") )
    {
        //REMOVESTARTRECT allyno
        ally = GetIntParam( params );
        m_se->OnBattleStartRectRemove( m_battle_id, ally );
    }
    else if ( cmd == _T("ENABLEALLUNITS") )
    {
        //"ENABLEALLUNITS" params: "".
        m_se->OnBattleEnableAllUnits( m_battle_id );
    }
    else if ( cmd == _T("ENABLEUNITS") )
    {
        //ENABLEUNITS unitname1 unitname2
        while ( (nick = GetWordParam( params )) !=_T( "") )
        {
            m_se->OnBattleEnableUnit( m_battle_id, nick );
        }
    }
    else if ( cmd == _T("DISABLEUNITS") )
    {
        //"DISABLEUNITS" params: "arm_advanced_radar_tower arm_advanced_sonar_station arm_advanced_torpedo_launcher arm_dragons_teeth arm_energy_storage arm_eraser arm_fark arm_fart_mine arm_fibber arm_geothermal_powerplant arm_guardian"
        while ( (nick = GetWordParam( params )) != _T("") )
        {
            m_se->OnBattleDisableUnit( m_battle_id, nick );
        }
    }
    else if ( cmd == _T("CHANNEL") )
    {
        channel = GetWordParam( params );
        units = GetIntParam( params );
        topic = GetSentenceParam( params );
        m_se->OnChannelList( channel, units, topic );
    }
    else if ( cmd == _T("ENDOFCHANNELS") )
    {
        //Cmd: ENDOFCHANNELS params:
    }
    else if ( cmd == _T("REQUESTBATTLESTATUS") )
    {
        m_se->OnRequestBattleStatus( m_battle_id );
    }
    else if ( cmd == _T("SAIDBATTLE") )
    {
        nick = GetWordParam( params );
        m_se->OnSaidBattle( m_battle_id, nick, params );
    }
    else if ( cmd == _T("SAIDBATTLEEX") )
    {
        nick = GetWordParam( params );
        m_se->OnBattleAction( m_battle_id, nick, params );
    }
    else if ( cmd == _T("AGREEMENT") )
    {
        msg = GetSentenceParam( params );
        m_agreement += msg + _T("\n");
    }
    else if ( cmd == _T("AGREEMENTEND") )
    {
        m_se->OnAcceptAgreement( m_agreement );
        m_agreement = _T("");
    }
    else if ( cmd == _T("OPENBATTLE") )
    {
        m_battle_id = GetIntParam( params );
        m_se->OnHostedBattle( m_battle_id );
    }
    else if ( cmd == _T("ADDBOT") )
    {
        // ADDBOT BATTLE_ID name owner battlestatus teamcolor {AIDLL}
        id = GetIntParam( params );
        nick = GetWordParam( params );
        owner = GetWordParam( params );
        tasbstatus.data = GetIntParam( params );
        bstatus = ConvTasbattlestatus( tasbstatus.tasdata );
        color.data = GetIntParam( params );
        bstatus.colour = wxColour( color.color.red, color.color.green, color.color.blue );
        ai = GetSentenceParam( params );
        if ( ai.IsEmpty() ) {
            wxLogWarning( wxString::Format( _T("Recieved illegal ADDBOT (empty dll field) from %s for battle %d"), nick.c_str(), id ) );
            ai = _T("INVALID|INVALID");
        }
        if( usync().VersionSupports( SpringUnitSync::USYNC_GetSkirmishAI ) )
        {
			if (ai.Find(_T('|')) != -1)
			{
				bstatus.aiversion = ai.AfterLast( _T('|') );
				ai = ai.BeforeLast( _T('|') );
			}
			bstatus.aishortname = ai;
        }
        else
        {
        	 bstatus.aishortname = ai;
        }
        bstatus.owner =owner;
        m_se->OnBattleAddBot( id, nick, bstatus );
    }
    else if ( cmd == _T("UPDATEBOT") )
    {
        id = GetIntParam( params );
        nick = GetWordParam( params );
        tasbstatus.data = GetIntParam( params );
        bstatus = ConvTasbattlestatus( tasbstatus.tasdata );
        color.data = GetIntParam( params );
        bstatus.colour = wxColour( color.color.red, color.color.green, color.color.blue );
        m_se->OnBattleUpdateBot( id, nick, bstatus );
        //UPDATEBOT BATTLE_ID name battlestatus teamcolor
    }
    else if ( cmd == _T("REMOVEBOT") )
    {
        id = GetIntParam( params );
        nick = GetWordParam( params );
        m_se->OnBattleRemoveBot( id, nick );
        //REMOVEBOT BATTLE_ID name
    }
    else if ( cmd == _T("RING") )
    {
        nick = GetWordParam( params );
        m_se->OnRing( nick );
        //RING username
    }
    else if ( cmd == _T("SERVERMSG") )
    {
				m_se->OnServerMessage( params );
        //SERVERMSG {message}
    }
    else if ( cmd == _T("JOINBATTLEFAILED") )
    {
        msg = GetSentenceParam( params );
        m_se->OnServerMessage( _T("Failed to join battle. ") + msg );
        //JOINBATTLEFAILED {reason}
    }
    else if ( cmd == _T("OPENBATTLEFAILED") )
    {
        msg = GetSentenceParam( params );
        m_se->OnServerMessage( _T("Failed to host new battle on server. ") + msg );
        //OPENBATTLEFAILED {reason}
    }
    else if ( cmd == _T("JOINFAILED") )
    {
        channel = GetWordParam( params );
        msg = GetSentenceParam( params );
        m_se->OnServerMessage( _T("Failed to join channel #") + channel + _T(". ") + msg );
        //JOINFAILED channame {reason}
    }
    else if ( cmd == _T("CHANNELMESSAGE") )
    {
        channel = GetWordParam( params );
        m_se->OnChannelMessage( channel, params );
        //CHANNELMESSAGE channame {message}
    }
    else if ( cmd == _T("ACQUIREUSERID") )
    {
        SendCmd( _T("USERID"), TowxString( m_crc.GetCRC() ) );
    }
    else if ( cmd == _T("FORCELEAVECHANNEL") )
    {
        channel = GetWordParam( params );
        nick = GetWordParam( params );
        msg = GetSentenceParam( params );
        m_se->OnChannelPart( channel, GetMe().GetNick(), _T("Kicked by <") + nick + _T("> ") + msg );
        //FORCELEAVECHANNEL channame username [{reason}]
    }
    else if ( cmd == _T("DENIED") )
    {
        if ( m_online ) return;
        m_last_denied = msg = GetSentenceParam( params );
        m_se->OnServerMessage( msg );
        Disconnect();
        //Command: "DENIED" params: "Already logged in".
    }
    else if ( cmd == _T("HOSTPORT") )
    {
        unsigned int tmp_port = (unsigned int)GetIntParam( params );
        m_se->OnHostExternalUdpPort( tmp_port );
        //HOSTPORT port
    }
    else if ( cmd == _T("UDPSOURCEPORT") )
    {
        unsigned int tmp_port = (unsigned int)GetIntParam( params );
        m_se->OnMyExternalUdpSourcePort( tmp_port );
        if (m_do_finalize_join_battle)FinalizeJoinBattle();
        //UDPSOURCEPORT port
    }
    else if (cmd == _T("CLIENTIPPORT"))
    {
        // clientipport username ip port
        nick=GetWordParam( params );
        wxString ip=GetWordParam(params);
        unsigned int u_port=(unsigned int)GetIntParam( params );
        m_se->OnClientIPPort(nick, ip, u_port);
    }
    else if ( cmd == _T("SETSCRIPTTAGS") )
    {
        wxString command;
        while ( (command = GetSentenceParam( params )) != _T("") )
        {
            wxString key = command.BeforeFirst( '=' ).Lower();
            wxString value = command.AfterFirst( '=' );
            m_se->OnSetBattleInfo( m_battle_id, key, value );
        }
        m_se->OnBattleInfoUpdated( m_battle_id );
        // !! Command: "SETSCRIPTTAGS" params: "game/startpostype=0	game/maxunits=1000	game/limitdgun=0	game/startmetal=1000	game/gamemode=0	game/ghostedbuildings=-1	game/startenergy=1000	game/diminishingmms=0"
    }
    else if ( cmd == _T("SCRIPTSTART") )
    {
        m_se->OnScriptStart( m_battle_id );
        // !! Command: "SCRIPTSTART" params: ""
    }
    else if ( cmd == _T("SCRIPTEND") )
    {
        m_se->OnScriptEnd( m_battle_id );
        // !! Command: "SCRIPTEND" params: ""
    }
    else if ( cmd == _T("SCRIPT") )
    {
        m_se->OnScriptLine(  m_battle_id, params );
        // !! Command: "SCRIPT" params: "[game]"
    }
    else if ( cmd == _T("FORCEQUITBATTLE"))
    {
        m_relay_host_bot = _T("");
        m_se->OnKickedFromBattle();
    }
    else if ( cmd == _T("BROADCAST"))
    {
		m_se->OnServerBroadcast( params );
    }
    else if ( cmd == _T("SERVERMSGBOX"))
    {
        m_se->OnServerMessageBox( params );
    }
    else if ( cmd == _T("REDIRECT") )
    {
        if ( m_online ) return;
        wxString address = GetWordParam( params );
        unsigned int u_port = GetIntParam( params );
        if ( address.IsEmpty() ) return;
        if ( u_port  == 0 ) u_port  = DEFSETT_DEFAULT_SERVER_PORT;
        m_redirecting = true;
        m_se->OnRedirect( address, u_port , m_user, m_pass );
    }
    else if ( cmd == _T("MUTELISTBEGIN") )
    {
        m_current_chan_name_mutelist = GetWordParam( params );
        m_se->OnMutelistBegin( m_current_chan_name_mutelist );

    }
    else if ( cmd == _T("MUTELIST") )
    {
        wxString mutee = GetWordParam( params );
		wxString description = GetSentenceParam( params );
        m_se->OnMutelistItem( m_current_chan_name_mutelist, mutee, description );
    }
    else if ( cmd == _T("MUTELISTEND") )
    {
        m_se->OnMutelistEnd( m_current_chan_name_mutelist );
        m_current_chan_name_mutelist = _T("");
    }
    // OFFERFILE options {filename} {url} {description}
    else if ( cmd == _T("OFFERFILE") )
    {
				UTASOfferFileData parsingdata;
				parsingdata.data = GetIntParam( params );
				wxString FileName = GetSentenceParam( params );
				wxString url = GetSentenceParam( params );
				wxString description = GetSentenceParam( params );
				m_se->OnFileDownload( parsingdata.tasdata.autoopen, parsingdata.tasdata.closelobbyondownload, parsingdata.tasdata.disconnectonrefuse, FileName, url, description );
    }
    else
    {
        wxLogMessage( _T("??? Cmd: %s params: %s"), cmd.c_str(), params.c_str() );
        m_se->OnUnknownCommand( cmd, params );
    }
}


void TASServer::RelayCmd(  const wxString& command, const wxString& param )
{
    if ( m_relay_host_bot.IsEmpty() )
    {
      wxLogWarning( _T("Trying to send relayed commands but no relay bot is set!") );
      return;
    }

    wxString msg = _T("!"); // prefix comma,nds with !
    if ( param.IsEmpty() ) msg << command.Lower();
    else msg << command.Lower() << _T(" ") << param;
    SayPrivate( m_relay_host_bot, msg );
}


void TASServer::SendCmd( const wxString& command, const wxString& param )
{
		wxString cmd, msg;
		if ( m_id_transmission )
		{
			 GetLastID()++;
			 msg = msg + _T("#") + TowxString( GetLastID() ) + _T(" ");
		}
		if ( m_token_transmission )
		{
			cmd = EncodeTokenMessage( command );
		}
		else cmd = command;
		if ( param.IsEmpty() ) msg = msg + cmd + _T("\n");
		else msg = msg + cmd + _T(" ") + param + _T("\n");
		bool send_success = m_sock->Send( msg );
		if ( command != _T("PING") )
		{
			if ( send_success )
				wxLogMessage( _T("sent: %s"), msg.c_str() );
			else
				wxLogMessage( _T("sending: %s failed"), msg.c_str() );
		}
}

void TASServer::SetRelayIngamePassword( const User& user )
{
	Battle* battle = GetCurrentBattle();
	if (battle)
	{
		if ( !battle->GetInGame() ) return;
	}
	RelayCmd( _T("SETINGAMEPASSWORD"), user.GetNick() + _T(" ") + user.BattleStatus().scriptPassword );
}

void TASServer::Ping()
{
	SendCmd( _T("PING") );
	TASPingListItem pli;
	pli.id = GetLastID();
	pli.t = wxGetLocalTimeMillis();
	GetPingList().push_back ( pli );
}

void TASServer::HandlePong( int replyid )
{
	PingList& pinglistcopy = GetPingList();
	PingList::iterator it;

    bool found = false;
	for ( it = pinglistcopy.begin(); it != pinglistcopy.end(); it++ )
    {
        if (it->id == replyid )
        {
            found = true;
            break;
        }
    }

    if ( found )
    {
        m_se->OnPong( (wxGetLocalTimeMillis() - it->t) );
		pinglistcopy.erase( it );
    }
}


void TASServer::JoinChannel( const wxString& channel, const wxString& key )
{
    //JOIN channame [key]
    wxLogDebugFunc( channel );

    m_channel_pw[channel] = key;
	m_id_transmission = false; // workaround a retarded server bug
    SendCmd ( _T("JOIN"), channel + _T(" ") + key );
	m_id_transmission = true;
}


void TASServer::PartChannel( const wxString& channel )
{
    //LEAVE channame
    wxLogDebugFunc( channel );

    SendCmd( _T("LEAVE"), channel );

}


void TASServer::DoActionChannel( const wxString& channel, const wxString& msg )
{
    //SAYEX channame {message}
    wxLogDebugFunc( _T("") );

    SendCmd( _T("SAYEX"), channel + _T(" ") + msg );
}


void TASServer::SayChannel( const wxString& channel, const wxString& msg )
{
    //SAY channame {message}
    wxLogDebugFunc( _T("") );

    SendCmd( _T("SAY"), channel + _T(" ") + msg );
}


void TASServer::SayPrivate( const wxString& nick, const wxString& msg )
{
    //SAYPRIVATE username {message}
    wxLogDebugFunc( _T("") );

    SendCmd( _T("SAYPRIVATE"), nick + _T(" ") + msg );
}


void TASServer::DoActionPrivate( const wxString& nick, const wxString& msg )
{
    wxLogDebugFunc( _T("") );

    SendCmd( _T("SAYPRIVATEEX"), nick + _T(" ") + msg );
}


void TASServer::SayBattle( int /*unused*/, const wxString& msg )
{
    wxLogDebugFunc( _T("") );

    SendCmd( _T("SAYBATTLE"), msg );
}


void TASServer::DoActionBattle( int /*unused*/, const wxString& msg )
{
    wxLogDebugFunc( _T("") );

    SendCmd( _T("SAYBATTLEEX"), msg );
}


void TASServer::Ring( const wxString& nick )
{
    wxLogDebugFunc( _T("") );
		try
		{
				ASSERT_EXCEPTION( m_battle_id != -1, _T("invalid m_battle_id value") );
				ASSERT_EXCEPTION( BattleExists(m_battle_id), _T("battle doesn't exists") );

				Battle& battle = GetBattle( m_battle_id );
				ASSERT_EXCEPTION( battle.IsFounderMe(), _T("I'm not founder") );

				if ( battle.IsProxy() )
				{
					RelayCmd( _T("RING"), nick );
				}
				else
				{
					SendCmd( _T("RING"), nick );
				}

		}
		catch (...)
		{
			SendCmd( _T("RING"), nick );
		}
}




void TASServer::ModeratorSetChannelTopic( const wxString& channel, const wxString& topic )
{
		wxString msgcopy = topic;
	  msgcopy.Replace( _T("\n"), _T("\\n") );
    SendCmd( _T("CHANNELTOPIC"), channel + _T(" ") + msgcopy );
}


void TASServer::ModeratorSetChannelKey( const wxString& channel, const wxString& key)
{
    SendCmd( _T("SETCHANNELKEY"), channel + _T(" ") + key );
}


void TASServer::ModeratorMute( const wxString& channel, const wxString& nick, int duration, bool byip )
{
    SendCmd( _T("MUTE"), channel + _T(" ") + nick + _T(" ") + wxString::Format( _T("%d"), duration) + (byip?_T(" ip"):_T("") )  );
}


void TASServer::ModeratorUnmute( const wxString& channel, const wxString& nick )
{
    SendCmd( _T("UNMUTE"),  channel + _T(" ") + nick );
}


void TASServer::ModeratorKick( const wxString& channel, const wxString& reason )
{
    SendCmd( _T("KICKUSER"), channel + _T(" ") + reason );
}


void TASServer::ModeratorBan( const wxString& /*unused*/, bool /*unused*/ )
{
    // FIXME TASServer::ModeratorBan not yet implemented
}


void TASServer::ModeratorUnban( const wxString& /*unused*/ )
{
    // FIXME TASServer::ModeratorUnban not yet implemented
}


void TASServer::ModeratorGetIP( const wxString& nick )
{
    SendCmd( _T("GETIP"), nick );
}


void TASServer::ModeratorGetLastLogin( const wxString& nick )
{
    SendCmd( _T("GETLASTLOGINTIME"), nick );
}


void TASServer::ModeratorGetLastIP( const wxString& nick )
{
    SendCmd( _T("GETLASTIP"), nick );
}


void TASServer::ModeratorFindByIP( const wxString& ipadress )
{
    SendCmd( _T("FINDIP"), ipadress );
}


void TASServer::AdminGetAccountAccess( const wxString& /*unused*/ )
{
    // FIXME TASServer::AdminGetAccountAccess not yet implemented
}


void TASServer::AdminChangeAccountAccess( const wxString& /*unused*/, const wxString& /*unused*/ )
{
    // FIXME TASServer::AdminChangeAccountAccess not yet implemented
}


void TASServer::AdminSetBotMode( const wxString& nick, bool isbot )
{
    SendCmd( _T("SETBOTMODE"), nick + _T(" ") + (isbot?_T("1"):_T("0")) );
}





void TASServer::HostBattle( BattleOptions bo, const wxString& password )
{
    wxLogDebugFunc( _T("") );

    // to see ip addresses of users as they join (in the log), pretend you're hosting with NAT.
    int nat_type=bo.nattype;
    /*
    if(nat_type==0 && sett().GetShowIPAddresses()){
    nat_type=1;
    }*/
    wxLogMessage(_T("hosting with nat type %d"),nat_type);

    wxString cmd = wxString::Format( _T("0 %d "), nat_type );
    cmd += (password.IsEmpty())?_T("*"):password;
    cmd += wxString::Format( _T(" %d %d "), bo.port, bo.maxplayers );
    cmd += MakeHashSigned( bo.modhash );
    cmd += wxString::Format( _T(" %d "), bo.rankneeded );
    cmd += MakeHashSigned( bo.maphash ) + _T(" ");
    cmd += bo.mapname + _T("\t");
    cmd += bo.description + _T("\t");
    cmd += bo.modname;

    m_delayed_open_command = _T("");
	if ( !bo.userelayhost )
    {
       SendCmd( _T("OPENBATTLE"), cmd );
    }
    else
    {
       if ( bo.relayhost.IsEmpty() )
       {
			wxArrayString relaylist = GetRelayHostList();
           unsigned int numbots = relaylist.GetCount();
           if ( numbots > 0 )
           {
              srand ( time(NULL) );
              unsigned int choice = rand() % numbots;
							m_relay_host_manager = relaylist[choice];
							m_delayed_open_command = cmd;
							SayPrivate( m_relay_host_manager, _T("!spawn") );
           }
       }
       else
       {
           m_relay_host_manager = bo.relayhost;
           m_delayed_open_command = cmd;
           SayPrivate(bo.relayhost,_T("!spawn"));
       }
    }

    if (bo.nattype>0)UdpPingTheServer(m_user);

    // OPENBATTLE type natType password port maphash {map} {title} {modname}
}


void TASServer::JoinBattle( const int& battleid, const wxString& password )
{
    //JOINBATTLE BATTLE_ID [parameter]
    wxLogDebugFunc( _T("") );

    m_finalize_join_battle_pw=password;
    m_finalize_join_battle_id=battleid;

    if (BattleExists(battleid))
    {
        Battle *battle=&GetBattle(battleid);

        if (battle)
        {
            if ( ( battle->GetNatType() == NAT_Hole_punching ) || ( battle->GetNatType() == NAT_Fixed_source_ports ) )
            {
                m_udp_private_port=sett().GetClientPort();

                m_last_udp_ping = time(0);
                // its important to set time now, to prevent Update()
                // from calling FinalizeJoinBattle() on timeout.
                // m_do_finalize_join_battle must be set to true after setting time, not before.
                m_do_finalize_join_battle=true;
                for (int n=0;n<5;++n) // do 5 udp pings with tiny interval
                {
                    UdpPingTheServer( m_user );
                    // sleep(0);// sleep until end of timeslice.
                }
                m_last_udp_ping = time(0);// set time again
            }
            else
            {
                // if not using nat, finalize now.
                m_do_finalize_join_battle=true;
                FinalizeJoinBattle();
            }
        }
        else
        {
            wxLogMessage( _T("battle doesnt exist (null)") );
        }
    }
    else
    {
        wxLogMessage( _T("battle doesnt exist") );
    }
    //SendCmd( _T("JOINBATTLE"), wxString::Format( _T("%d"), battleid ) + _T(" ") + password );
}


void TASServer::FinalizeJoinBattle()
{
    if (m_do_finalize_join_battle)
    {
		srand ( time(NULL) );
		wxString randomScriptPassword = wxString::Format(_T("%04x%04x"), rand()&0xFFFF, rand()&0xFFFF);
		SendCmd( _T("JOINBATTLE"), wxString::Format( _T("%d %s %s"), m_finalize_join_battle_id, m_finalize_join_battle_pw.c_str(), randomScriptPassword.c_str()  ) );
        m_do_finalize_join_battle=false;
    }
}


void TASServer::LeaveBattle( const int& /*unused*/ )
{
    //LEAVEBATTLE
    wxLogDebugFunc( _T("") );
    m_relay_host_bot = _T("");
    SendCmd( _T("LEAVEBATTLE") );
}


void TASServer::SendHostInfo( HostInfo update )
{
    wxLogDebugFunc( _T("") );

    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("invalid m_battle_id value") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
    }
    catch (...)
    {
        return;
    }

    Battle& battle = GetBattle( m_battle_id );
    try
    {
        ASSERT_LOGIC( battle.IsFounderMe(), _T("I'm not founder") );
    }
    catch (...)
    {
        return;
    }

    //BattleOptions bo = battle.opts();

    if ( ( update & ( IBattle::HI_Map | IBattle::HI_Locked | IBattle::HI_Spectators ) ) > 0 )
    {
        // UPDATEBATTLEINFO SpectatorCount locked maphash {mapname}
        wxString cmd = wxString::Format( _T("%d %d "), battle.GetSpectators(), battle.IsLocked() );
        cmd += MakeHashSigned( battle.LoadMap().hash ) + _T(" ");
        cmd += battle.LoadMap().name;

        if ( !battle.IsProxy() ) SendCmd( _T("UPDATEBATTLEINFO"), cmd );
        else RelayCmd( _T("UPDATEBATTLEINFO"), cmd );
    }
    if ( ( update & IBattle::HI_Send_All_opts ) > 0 )
    {
        wxString cmd;

        OptionsWrapper::wxStringTripleVec optlistMap = battle.CustomBattleOptions().getOptions( OptionsWrapper::MapOption );
		for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistMap.begin(); it != optlistMap.end(); ++it)
        {
			wxString newcmd = _T("game/mapoptions/") + it->first + _T("=") + it->second.second + _T("\t");
			if ( cmd.size() + newcmd.size() > 900 ) // should be 1024 add margin for relayhost name and command itself
			{
				if ( !battle.IsProxy() ) SendCmd( _T("SETSCRIPTTAGS"), cmd );
				else RelayCmd( _T("SETSCRIPTTAGS"), cmd );
				cmd = _T("");
			}
			cmd << newcmd;
        }
        OptionsWrapper::wxStringTripleVec optlistMod = battle.CustomBattleOptions().getOptions( OptionsWrapper::ModOption );
		for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistMod.begin(); it != optlistMod.end(); ++it)
        {
			wxString newcmd = _T("game/modoptions/") + it->first + _T("=") + it->second.second + _T("\t");
			if ( cmd.size() + newcmd.size() > 900 )// should be 1024 add margin for relayhost name and command itself
			{
				if ( !battle.IsProxy() ) SendCmd( _T("SETSCRIPTTAGS"), cmd );
				else RelayCmd( _T("SETSCRIPTTAGS"), cmd );
				cmd = _T("");
			}
			cmd << newcmd;
        }
        OptionsWrapper::wxStringTripleVec optlistEng = battle.CustomBattleOptions().getOptions( OptionsWrapper::EngineOption );
		for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistEng.begin(); it != optlistEng.end(); ++it)
        {
			wxString newcmd = _T("game/") + it->first + _T("=") + it->second.second + _T("\t");
			if ( cmd.size() + newcmd.size() > 900 )// should be 1024 add margin for relayhost name and command itself
			{
				if ( !battle.IsProxy() ) SendCmd( _T("SETSCRIPTTAGS"), cmd );
				else RelayCmd( _T("SETSCRIPTTAGS"), cmd );
				cmd = _T("");
			}
			cmd << newcmd;
        }
		if ( !battle.IsProxy() ) SendCmd( _T("SETSCRIPTTAGS"), cmd );
		else RelayCmd( _T("SETSCRIPTTAGS"), cmd );
    }

    if ( (update & IBattle::HI_StartRects) > 0 )   // Startrects should be updated.
    {
        unsigned int numrects = battle.GetLastRectIdx();
        for ( unsigned int i = 0; i <= numrects; i++ )   // Loop through all, and remove updated or deleted.
        {
            wxString cmd;
            BattleStartRect sr = battle.GetStartRect( i );
            if ( !sr.exist ) continue;
            if ( sr.todelete )
            {
                if ( !battle.IsProxy() ) SendCmd( _T("REMOVESTARTRECT"), wxString::Format( _T("%d"), i ) );
                else RelayCmd( _T("REMOVESTARTRECT"), wxString::Format( _T("%d"), i ) );
                battle.StartRectRemoved( i );
            }
            else if ( sr.toadd )
            {
                if ( !battle.IsProxy() ) SendCmd( _T("ADDSTARTRECT"), wxString::Format( _T("%d %d %d %d %d"), sr.ally, sr.left, sr.top, sr.right, sr.bottom ) );
                else RelayCmd( _T("ADDSTARTRECT"), wxString::Format( _T("%d %d %d %d %d"), sr.ally, sr.left, sr.top, sr.right, sr.bottom ) );
                battle.StartRectAdded( i );
            }
            else if ( sr.toresize )
            {
                if ( !battle.IsProxy() )
                {
                  SendCmd( _T("REMOVESTARTRECT"), wxString::Format( _T("%d"), i ) );
                  SendCmd( _T("ADDSTARTRECT"), wxString::Format( _T("%d %d %d %d %d"), sr.ally, sr.left, sr.top, sr.right, sr.bottom ) );
                }
                else
                {
                  RelayCmd( _T("REMOVESTARTRECT"), wxString::Format( _T("%d"), i ) );
                  RelayCmd( _T("ADDSTARTRECT"), wxString::Format( _T("%d %d %d %d %d"), sr.ally, sr.left, sr.top, sr.right, sr.bottom ) );
                }
                battle.StartRectResized( i );
            }
        }

    }
    if ( (update & IBattle::HI_Restrictions) > 0 )
    {
        std::map<wxString, int> units = battle.RestrictedUnits();
        if ( !battle.IsProxy() ) SendCmd( _T("ENABLEALLUNITS") );
        else RelayCmd( _T("ENABLEALLUNITS") );
        if ( units.size() > 0 )
        {
            wxString msg;
            wxString scriptmsg;
			for ( std::map<wxString, int>::const_iterator itor = units.begin(); itor != units.end(); itor++ )
            {
            	 msg << itor->first + _T(" ");
            	 scriptmsg << _T("game/restrict/") + itor->first + _T("=") + TowxString(itor->second) + _T('\t'); // this is a serious protocol abuse, but on the other hand, the protocol fucking suck and it's unmaintained so it will do for now
            }
						if ( !battle.IsProxy() )
						{
							 SendCmd( _T("DISABLEUNITS"), msg );
							 SendCmd( _T("SETSCRIPTTAGS"), scriptmsg );
						}
						else
						{
							 RelayCmd( _T("DISABLEUNITS"), msg );
						   RelayCmd( _T("SETSCRIPTTAGS"), scriptmsg );
						}
        }
    }
}


void TASServer::SendHostInfo( const wxString& Tag )
{
    wxLogDebugFunc( _T("") );

    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("invalid m_battle_id value") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
    }
    catch (...)
    {
        return;
    }

    Battle& battle = GetBattle( m_battle_id );

    try
    {
        ASSERT_LOGIC( battle.IsFounderMe(), _T("I'm not founder") );
    }
    catch (...)
    {
        return;
    }

    wxString cmd;

    long type;
    Tag.BeforeFirst( '_' ).ToLong( &type );
    wxString key = Tag.AfterFirst( '_' );
    if ( type == OptionsWrapper::MapOption )
    {
        cmd << _T("game/mapoptions/") << key << _T("=") << battle.CustomBattleOptions().getSingleValue( key, OptionsWrapper::MapOption );
    }
    else if ( type == OptionsWrapper::ModOption )
    {
        cmd << _T("game/modoptions/") << key << _T("=") << battle.CustomBattleOptions().getSingleValue( key, OptionsWrapper::ModOption );
    }
    else if ( type == OptionsWrapper::EngineOption )
    {
        cmd << _T("game/") << key << _T("=") << battle.CustomBattleOptions().getSingleValue( key, OptionsWrapper::EngineOption );
    }
    if ( !battle.IsProxy() ) SendCmd( _T("SETSCRIPTTAGS"), cmd );
    else RelayCmd( _T("SETSCRIPTTAGS"), cmd );
}


void TASServer::SendUserPosition( const User& user )
{
	wxLogDebugFunc( _T("") );

	try
	{
			ASSERT_LOGIC( m_battle_id != -1, _T("invalid m_battle_id value") );
			ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );

			Battle& battle = GetBattle( m_battle_id );
			ASSERT_LOGIC( battle.IsFounderMe(), _T("I'm not founder") );

			UserBattleStatus status = user.BattleStatus();
			wxString msgx = _T("game/Team") + TowxString( status.team ) + _T("/StartPosX=") + TowxString( status.pos.x );
			wxString msgy = _T("game/Team") + TowxString( status.team ) + _T("/StartPosY=") + TowxString( status.pos.y );
			wxString netmessage = msgx + _T("\t") + msgy;
			if ( battle.IsProxy() )
			{
				RelayCmd( _T("SETSCRIPTTAGS"), netmessage );
			}
			else
			{
				SendCmd( _T("SETSCRIPTTAGS"), netmessage );
			}

	}
	catch (...)
	{
			return;
	}
}

void TASServer::SendRaw( const wxString& raw )
{
    SendCmd( raw );
}


void TASServer::RequestInGameTime( const wxString& nick )
{
    SendCmd( _T("GETINGAMETIME"), nick );
}


Battle* TASServer::GetCurrentBattle()
{
    try
    {
        ASSERT_EXCEPTION( m_battle_id != -1, _T("invalid m_battle_id value") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
    }
    catch (...)
    {
        return NULL;
    }

    return &GetBattle( m_battle_id );
}


void TASServer::SendMyBattleStatus( UserBattleStatus& bs )
{
    wxLogDebugFunc( _T("") );

    UTASBattleStatus tasbs;
    tasbs.tasdata = ConvTasbattlestatus( bs );
    UTASColor tascl;
    tascl.color.red = bs.colour.Red();
    tascl.color.green = bs.colour.Green();
    tascl.color.blue = bs.colour.Blue();
    tascl.color.zero = 0;
    //MYBATTLESTATUS battlestatus myteamcolor
    SendCmd( _T("MYBATTLESTATUS"), wxString::Format( _T("%d %d"), tasbs.data, tascl.data ) );
}


void TASServer::SendMyUserStatus()
{
    wxLogDebugFunc( _T("") );

    UserStatus& us = GetMe().Status();

    UTASClientStatus taus;
    taus.tasdata.in_game = us.in_game;
    taus.tasdata.away = us.away;
    taus.tasdata.rank = us.rank;
    taus.tasdata.moderator = us.moderator;
    taus.tasdata.bot = us.bot;

    SendCmd( _T("MYSTATUS"), wxString::Format( _T("%d"), taus.byte ) );
}


void TASServer::StartHostedBattle()
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
    }
    catch (...)
    {
        return;
    }
    Battle *battle=GetCurrentBattle();
    if (battle)
    {
        if ( ( battle->GetNatType() == NAT_Hole_punching ) || ( battle->GetNatType() == NAT_Fixed_source_ports ) )
        {
            UdpPingTheServer(m_user);
            for (int i=0;i<5;++i)UdpPingAllClients();
        }
    }

    m_se->OnStartHostedBattle( m_battle_id );
}


void TASServer::ForceSide( int battleid, User& user, int side )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }

		UserBattleStatus status = user.BattleStatus();

    if ( &user == &GetMe() )
    {
        status.side = side;
        SendMyBattleStatus( status );
        return;
    }

    if ( status.IsBot() )
    {
    	status.side = side;
    	UpdateBot( battleid, user, status );
    }
}


void TASServer::ForceTeam( int battleid, User& user, int team )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }
    UserBattleStatus status = user.BattleStatus();
		if ( status.IsBot() )
		{
				status.team = team;
				UpdateBot( battleid, user, status );
				return;
		}
		if ( &user == &GetMe() )
		{
				status.team = team;
				SendMyBattleStatus( status );
				return;
		}
    if ( !GetBattle(battleid).IsFounderMe() )
    {
				DoActionBattle( battleid, _T("suggests that ") + user.GetNick() + _T(" changes to team #") + wxString::Format( _T("%d"), team + 1 ) + _T(".") );
        return;
    }

    //FORCETEAMNO username teamno
    if( !GetBattle(battleid).IsProxy() ) SendCmd( _T("FORCETEAMNO"), user.GetNick() + wxString::Format(_T(" %d"), team ) );
    else RelayCmd( _T("FORCETEAMNO"), user.GetNick() + wxString::Format(_T(" %d"), team ) );
}


void TASServer::ForceAlly( int battleid, User& user, int ally )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }
		UserBattleStatus status = user.BattleStatus();
		if ( status.IsBot() )
		{
			 status.ally = ally;
			 UpdateBot( battleid, user, status );
			 return;
		}

		if ( &user == &GetMe() )
		{
				status.ally = ally;
				SendMyBattleStatus( status );
				return;
		}

    if ( !GetBattle(battleid).IsFounderMe() )
    {
			DoActionBattle( battleid, _T("suggests that ") + user.GetNick() + _T(" changes to ally #") + wxString::Format( _T("%d"), ally + 1 ) + _T(".") );
			return;
    }

    //FORCEALLYNO username teamno
    if( !GetBattle(battleid).IsProxy() ) SendCmd( _T("FORCEALLYNO"), user.GetNick() + wxString::Format( _T(" %d"), ally ) );
    else RelayCmd( _T("FORCEALLYNO"), user.GetNick() + wxString::Format( _T(" %d"), ally ) );
}


void TASServer::ForceColour( int battleid, User& user, const wxColour& col )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }
		UserBattleStatus status = user.BattleStatus();
		if ( status.IsBot() )
		{
			 status.colour = col;
			 UpdateBot( battleid, user, status );
			 return;
		}
		if ( &user == &GetMe() )
		{
				status.colour = col;
				SendMyBattleStatus( status );
				return;
		}
    if ( !GetBattle(battleid).IsFounderMe() )
    {
			DoActionBattle( battleid, _T("sugests that ") + user.GetNick() + _T(" changes colour.") );
			return;
    }

    UTASColor tascl;
    tascl.color.red = col.Red();
    tascl.color.green = col.Green();
    tascl.color.blue = col.Blue();
    tascl.color.zero = 0;
    //FORCETEAMCOLOR username color
    if( !GetBattle(battleid).IsProxy() ) SendCmd( _T("FORCETEAMCOLOR"), user.GetNick() + _T(" ") + wxString::Format( _T("%d"), tascl.data ) );
    else RelayCmd( _T("FORCETEAMCOLOR"), user.GetNick() + _T(" ") + wxString::Format( _T("%d"), tascl.data ) );
}


void TASServer::ForceSpectator( int battleid, User& user, bool spectator )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }
		UserBattleStatus status = user.BattleStatus();
		if ( status.IsBot() )
		{
			 status.spectator = spectator;
			 UpdateBot( battleid, user, status );
			 return;
		}
		if ( &user == &GetMe() )
		{
				status.spectator = spectator;
				SendMyBattleStatus( status );
				return;
		}
    if ( !GetBattle(battleid).IsFounderMe() )
    {
			if ( spectator ) DoActionBattle( battleid, _T("suggests that ") + user.GetNick() + _T(" becomes a spectator.") );
			else DoActionBattle( battleid, _T("suggests that ") + user.GetNick() + _T(" plays.") );
			return;
    }

    //FORCESPECTATORMODE username
    if( !GetBattle(battleid).IsProxy() ) SendCmd( _T("FORCESPECTATORMODE"), user.GetNick() );
    else RelayCmd( _T("FORCESPECTATORMODE"), user.GetNick() );
}


void TASServer::BattleKickPlayer( int battleid, User& user )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }
		UserBattleStatus status = user.BattleStatus();
    if ( status.IsBot() )
    {
    	RemoveBot( battleid, user );
    	return;
    }
		if ( &user == &GetMe() )
		{
				LeaveBattle( battleid );
				return;
		}
    if ( !GetBattle(battleid).IsFounderMe() )
    {
			DoActionBattle( battleid, _T("thinks ") + user.GetNick() + _T(" should leave.") );
			return;
    }

    //KICKFROMBATTLE username
	if( !GetBattle(battleid).IsProxy() )
	{
		user.BattleStatus().scriptPassword = wxString::Format(_T("%04x%04x"), rand()&0xFFFF, rand()&0xFFFF); // reset his password to something random, so he can't rejoin
		SetRelayIngamePassword( user );
		SendCmd( _T("KICKFROMBATTLE"), user.GetNick() );
	}
    else RelayCmd( _T("KICKFROMBATTLE"), user.GetNick() );
}

void TASServer::SetHandicap( int battleid, User& user, int handicap)
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }
		UserBattleStatus status = user.BattleStatus();
		if ( status.IsBot() )
		{
				status.handicap = handicap;
				UpdateBot( battleid, user, status );
				return;
		}

    if ( !GetBattle(battleid).IsFounderMe() )
    {
        DoActionBattle( battleid, _T("thinks ") + user.GetNick() + _T(" should get a ") + wxString::Format( _T("%d"), handicap) + _T("% resource bonus") );
        return;
    }

    //HANDICAP username value
    if( !GetBattle(battleid).IsProxy() ) SendCmd( _T("HANDICAP"), user.GetNick() + wxString::Format( _T(" %d"), handicap ) );
    else RelayCmd( _T("HANDICAP"), user.GetNick() + wxString::Format( _T(" %d"), handicap ) );
}


void TASServer::AddBot( int battleid, const wxString& nick, UserBattleStatus& status )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }

    UTASBattleStatus tasbs;
    tasbs.tasdata = ConvTasbattlestatus( status );
    UTASColor tascl;
    tascl.color.red = status.colour.Red();
    tascl.color.green = status.colour.Green();
    tascl.color.blue = status.colour.Blue();
    tascl.color.zero = 0;
    //ADDBOT name battlestatus teamcolor {AIDLL}
    wxString msg;
    wxString ailib;
    ailib += status.aishortname;
    if ( usync().VersionSupports( SpringUnitSync::USYNC_GetSkirmishAI ) ) ailib += _T("|") + status.aiversion;
    SendCmd( _T("ADDBOT"), nick + wxString::Format( _T(" %d %d "), tasbs.data, tascl.data ) + ailib );
}


void TASServer::RemoveBot( int battleid, User& bot )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }

    Battle& battle = GetBattle( battleid );
    ASSERT_LOGIC( &bot != 0, _T("Bot does not exist.") );

    if ( !( battle.IsFounderMe() || ( bot.BattleStatus().owner == GetMe().GetNick() ) ) )
    {
        DoActionBattle( battleid, _T("thinks the bot ") + bot.GetNick() + _T(" should be removed.") );
        return;
    }

    //REMOVEBOT name
    if( !GetBattle(battleid).IsProxy() ) SendCmd( _T("REMOVEBOT"), bot.GetNick() );
    else RelayCmd( _T("REMOVEBOT"), bot.GetNick() );
}


void TASServer::UpdateBot( int battleid, User& bot, UserBattleStatus& status )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( m_battle_id != -1, _T("Invalid m_battle_id") );
        ASSERT_LOGIC( BattleExists(m_battle_id), _T("battle doesn't exists") );
        ASSERT_LOGIC( battleid == m_battle_id, _T("Not current battle") );
    }
    catch (...)
    {
        return;
    }

    UTASBattleStatus tasbs;
    tasbs.tasdata = ConvTasbattlestatus( status );
    UTASColor tascl;
    tascl.color.red = status.colour.Red();
    tascl.color.green = status.colour.Green();
    tascl.color.blue = status.colour.Blue();
    tascl.color.zero = 0;
    //UPDATEBOT name battlestatus teamcolor
    if( !GetBattle(battleid).IsProxy() ) SendCmd( _T("UPDATEBOT"), bot.GetNick() + wxString::Format( _T(" %d %d"), tasbs.data, tascl.data ) );
    else RelayCmd( _T("UPDATEBOT"), bot.GetNick() + wxString::Format( _T(" %d %d"), tasbs.data, tascl.data ) );
}

void TASServer::SendScriptToProxy( const wxString& script )
{
  wxArrayString strings = wxStringTokenize( script, _T("\n") );
  int relaylenghtprefix = 10 + 1 + m_relay_host_bot.Len() + 2; // SAYPRIVATE + space + botname + space + exclamation mark lenght
  int lenght = script.size();
  lenght += relaylenghtprefix + 11 + 1; // CLEANSCRIPT command size
  lenght += strings.GetCount() * ( relaylenghtprefix + 16 + 1 ); // num lines * APPENDSCRIPTLINE + space command size ( \n is already counted in script.size)
  lenght += relaylenghtprefix + 9 + 1; // STARTGAME command size
  int time = lenght / m_sock->GetSendRateLimit(); // calculate time in seconds to upload script
  DoActionBattle( m_battle_id, wxString::Format(_T("is preparing to start the game, game will start in approximately %d seconds"),time));
  RelayCmd( _T("CLEANSCRIPT") );
  wxStringTokenizer tkzr( script, _T("\n") );
  while ( tkzr.HasMoreTokens() )
  {
      wxString line = tkzr.GetNextToken();
      RelayCmd( _T("APPENDSCRIPTLINE"), line );
  }
  RelayCmd( _T("STARTGAME") );
}

void TASServer::SendScriptToClients( const wxString& script )
{
  SendCmd( _T("SCRIPTSTART") );
  wxStringTokenizer tkzr( script, _T("\n") );
  while ( tkzr.HasMoreTokens() )
  {
      wxString line = tkzr.GetNextToken();
      SendCmd( _T("SCRIPT"), line );
  }
  SendCmd( _T("SCRIPTEND") );
}

void TASServer::OnConnected( Socket* /*unused*/ )
{
    wxLogDebugFunc( _T("") );
    //TASServer* serv = (TASServer*)sock->GetUserdata();
    m_last_udp_ping = time( 0 );
    m_connected = true;
    m_online = false;
	m_token_transmission = false;
	m_relay_host_manager_list.Clear();
	m_last_denied = _T("");
	GetLastID() = 0;
	GetPingList().clear();
}


void TASServer::OnDisconnected( Socket* /*unused*/ )
{
    wxLogDebugFunc( TowxString(m_connected) );
    bool connectionwaspresent = m_online || !m_last_denied.IsEmpty() || m_redirecting;
    m_last_denied = _T("");
    m_connected = false;
    m_online = false;
    m_redirecting = false;
	m_token_transmission = false;
	m_buffer = _T("");
	m_relay_host_manager_list.Clear();
	GetLastID() = 0;
	GetPingList().clear();
	if ( m_ping_thread )
	{
		m_ping_thread->Wait();
		delete m_ping_thread;
		m_ping_thread = 0;
	}
	m_users.Nullify();
    m_se->OnDisconnected( connectionwaspresent );
    Server::OnDisconnected();
}


void TASServer::OnDataReceived( Socket* sock )
{
		if ( sock == 0 ) return;
		m_last_net_packet = time( 0 );
    wxString data = sock->Receive();
		m_buffer << data;
		m_buffer.Replace( _T("\r\n"), _T("\n") );
		int returnpos = m_buffer.Find( _T("\n") );
		while ( returnpos != -1 )
		{
			wxString cmd = m_buffer.Left( returnpos );
			m_buffer = m_buffer.Mid( returnpos + 1 );
			ExecuteCommand( cmd );
			returnpos = m_buffer.Find( _T("\n") );
		}
}


//! @brief Send udp ping.
//! @note used for nat travelsal.

unsigned int TASServer::UdpPing(unsigned int src_port, const wxString &target, unsigned int target_port, const wxString &message)// full parameters version, used to ping all clients when hosting.
{
    int result=0;
    wxLogMessage(_T("UdpPing src_port=%d , target='%s' , target_port=%d , message='%s'"),src_port,target.c_str(),target_port, message.c_str());
    wxIPV4address local_addr;
    local_addr.AnyAddress(); // <--- THATS ESSENTIAL!
    local_addr.Service(src_port);

    wxDatagramSocket udp_socket(local_addr,/* wxSOCKET_WAITALL*/wxSOCKET_NONE);

    wxIPV4address wxaddr;
    wxaddr.Hostname(target);
    wxaddr.Service(target_port);

    if (udp_socket.IsOk()&&!udp_socket.Error())
    {
        std::string m=STD_STRING(message);
        udp_socket.SendTo( wxaddr, m.c_str(), m.length() );
        wxIPV4address true_local_addr;
        if (udp_socket.GetLocal(true_local_addr))
        {
            result=true_local_addr.Service();
        }
    }
    else
    {
        wxLogMessage(_T("socket's IsOk() is false, no UDP ping done."));
    }

    if (udp_socket.Error()) {
        wxLogWarning(_T("wxDatagramSocket Error=%d"),udp_socket.LastError());
    }
    return result;
}

void TASServer::UdpPingTheServer(const wxString &message)
{
    unsigned int port=UdpPing(m_udp_private_port,m_addr,m_nat_helper_port,message);
    if (port>0)
    {
        m_udp_private_port=port;
        m_se->OnMyInternalUdpSourcePort( m_udp_private_port );
    }
}


// copypasta from spring.cpp , to get users ordered same way as in tasclient.
struct UserOrder
{
    int index;// user number for GetUser
    int order;// user order (we'll sort by it)
    bool operator<(UserOrder b) const  // comparison function for sorting
    {
        return order<b.order;
    }
};


void TASServer::UdpPingAllClients()// used when hosting with nat holepunching. has some rudimentary support for fixed source ports.
{
    Battle *battle=GetCurrentBattle();
    if (!battle)return;
    if (!battle->IsFounderMe())return;
    wxLogMessage(_T("UdpPingAllClients()"));

    // I'm gonna mimic tasclient's behavior.
    // It of course doesnt matter in which order pings are sent,
    // but when doing "fixed source ports", the port must be
    // FIRST_UDP_SOURCEPORT + index of user excluding myself
    // so users must be reindexed in same way as in tasclient
    // to get same source ports for pings.


    // copypasta from spring.cpp
    std::vector<UserOrder> ordered_users;


    for ( UserList::user_map_t::size_type i = 0; i < battle->GetNumUsers(); i++ )
    {
        User &user=battle->GetUser(i);
        if (&user == &(battle->GetMe()))continue;// dont include myself (change in copypasta)

        UserOrder tmp;
        tmp.index=i;
        ordered_users.push_back(tmp);
    }
    std::sort(ordered_users.begin(),ordered_users.end());


    for (int i=0;i<int(ordered_users.size());++i)
    {
        User &user=battle->GetUser(ordered_users[i].index);

        wxString ip=user.BattleStatus().ip;
        unsigned int port=user.BattleStatus().udpport;

        unsigned int src_port=m_udp_private_port;
        if ( battle->GetNatType() == NAT_Fixed_source_ports )
        {
            port = FIRST_UDP_SOURCEPORT + i;
        }

        wxLogMessage(_T(" pinging nick=%s , ip=%s , port=%u"),user.GetNick().c_str(),ip.c_str(),port);

        if (port!=0 && !ip.empty())
        {
            UdpPing(src_port,ip,port,_T("hai!"));
        }
    }
}


//! @brief used to check if the NAT is done properly when hosting
int TASServer::TestOpenPort( unsigned int port ) const
{
    wxIPV4address local_addr;
    local_addr.AnyAddress(); // <--- THATS ESSENTIAL!
    local_addr.Service(port);

    wxSocketServer udp_socket(local_addr, wxSOCKET_NONE);

    wxHTTP connect_to_server;
    connect_to_server.SetTimeout( 10 );

    if ( !connect_to_server.Connect( _T("zjt3.com") ) ) return porttest_unreachable;
    connect_to_server.GetInputStream(wxString::Format( _T("/porttest.php?port=%u"), port));

    if (udp_socket.IsOk())
    {
        if ( !udp_socket.WaitForAccept( 10 ) ) return porttest_timeout;
    }
    else
    {
        wxLogMessage(_T("socket's IsOk() is false, no UDP packets can be checked"));
        return porttest_socketNotOk;
    }
    if (udp_socket.Error())
    {
        wxLogMessage(_T("Error=%d"),udp_socket.LastError());
        return porttest_socketError;
    }
    return porttest_pass;
}

void TASServer::RequestSpringUpdate()
{
	SendCmd( _T("REQUESTUPDATEFILE"), _T("Spring ") + usync().GetSpringVersion() );
}

wxArrayString TASServer::GetRelayHostList()
{
	if ( UserExists( _T("RelayHostManagerList") ) ) SayPrivate( _T("RelayHostManagerList"), _T("!lm") );
	wxArrayString ret;
	for ( unsigned int i = 0; i < m_relay_host_manager_list.GetCount(); i++ )
	{
		try
		{
			User& manager = GetUser( m_relay_host_manager_list[i] );
			if ( manager.Status().in_game ) continue; // skip the manager is not connected or reports it's ingame ( no slots available ), or it's away ( functionality disabled )
			if ( manager.Status().away ) continue;
			ret.Add( m_relay_host_manager_list[i] );
		}
		catch(...){}
	}
	return ret;
}

////////////////////////
// Utility functions
//////////////////////

UserStatus ConvTasclientstatus( TASClientstatus tas )
{
    UserStatus stat;
    stat.in_game = tas.in_game;
    stat.away = tas.away;
    stat.rank = (UserStatus::RankContainer)tas.rank;
    stat.moderator = tas.moderator;
    stat.bot = tas.bot;
    return stat;
}

UserBattleStatus ConvTasbattlestatus( TASBattleStatus tas )
{
    UserBattleStatus stat;
    stat.ally = tas.ally;
    stat.handicap = tas.handicap;
    stat.ready = (tas.ready==1)?true:false;
    stat.side = tas.side;
    stat.spectator = (tas.player == 0)?true:false;
    stat.sync = tas.sync;
    stat.team = tas.team;
    return stat;
}


TASBattleStatus ConvTasbattlestatus( UserBattleStatus bs)
{
    TASBattleStatus stat;
    stat.ally = bs.ally;
    stat.handicap = bs.handicap;
    stat.ready = bs.ready?1:0;
    stat.side = bs.side;
    stat.player = bs.spectator?0:1;
    stat.sync = bs.sync;
    stat.team = bs.team;
    return stat;
}


IBattle::StartType IntToStartType( int start )
{
    switch ( start )
    {
    case 0:
        return IBattle::ST_Fixed;
    case 1:
        return IBattle::ST_Random;
    case 2:
        return IBattle::ST_Choose;
    default:
        ASSERT_EXCEPTION( false, _T("invalid value") );
    };
    return IBattle::ST_Fixed;
}


NatType IntToNatType( int nat )
{
    switch ( nat )
    {
    case 0:
        return NAT_None;
    case 1:
        return NAT_Hole_punching;
    case 2:
        return NAT_Fixed_source_ports;
    default:
        ASSERT_EXCEPTION( false, _T("invalid value") );
    };
    return NAT_None;
}


IBattle::GameType IntToGameType( int gt )
{
    switch ( gt )
    {
    case 0:
        return IBattle::GT_ComContinue;
    case 1:
        return IBattle::GT_ComEnds;
    case 2:
        return IBattle::GT_Lineage;
    default:
        ASSERT_EXCEPTION( false, _T("invalid value") );
    };
    return IBattle::GT_ComContinue;
}

