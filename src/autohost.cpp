/* Author: Tobi Vollebregt */

#include "autohost.h"
#include "battle.h"
#include "server.h"
#include "settings.h"
#include "user.h"
#include "utils.h"

#include <wx/tokenzr.h>


AutoHost::AutoHost( Battle& battle )
: m_battle(battle), m_enabled(false), m_lastActionTime(0)
{
}


void AutoHost::SetEnabled( const bool enabled )
{
  m_enabled = enabled;
}

bool AutoHost::GetEnabled()
{
	return m_enabled;
}

void AutoHost::OnSaidBattle( const wxString& nick, const wxString& msg )
{
  // do nothing if autohost functionality is disabled

  if (!m_enabled)
    return;

  // protect against command spam

  time_t currentTime = time(NULL);

  if ((currentTime - m_lastActionTime) < 5)
    return;

  // check for autohost commands
  wxString command = msg.BeforeFirst(_T(' '));
	wxString params = msg.AfterFirst(_T(' '));
  if (command == _T("!start")) {
    StartBattle();
  }
  else if ( command == _T("!balance") ) {
    unsigned int num = s2l( params );
    m_battle.Autobalance(IBattle::balance_random, false, false, num);
		m_battle.DoAction(_T("is auto-balancing alliances ..."));
  }
  else if ( command == _T("!cbalance") ) {
    unsigned int num = s2l( params );
    m_battle.Autobalance(IBattle::balance_random, true, false, num);
    m_battle.DoAction(_T("is auto-balancing alliances ..."));
  }
  else if ( command == _T("!help")) {
    m_battle.DoAction( _T( "The following commands are available ( <> = mandatory value, {} = optional value ):" ) );
    m_battle.DoAction( _T( "!addbox <allynumber> <topx> <topy> <bottomx> <bottomy>: adds a <allynumber> start restriction to the given coordinates, coordinates range from 0 to 200." ) );
    m_battle.DoAction( _T( "!balance {number}: tries to put players into allyteams by how many start boxes there are, uses {number} allyteams if present.") );
    m_battle.DoAction( _T( "!cbalance {number}: see !balance but tries to put clanmates together first." ) );
    m_battle.DoAction( _T( "!fixcolors: changes players duplicate colours so they are unique." ) );
    m_battle.DoAction( _T( "!fixids {number}: tries to put players into control teams by number, if number is omitted it assignes as different controlteam per player." ) );
    m_battle.DoAction( _T( "!cfixids {number}: see !fixids but tries to put clanmates together first." ) );
    m_battle.DoAction( _T( "!help: this guide." ) );
    m_battle.DoAction( _T( "!listprofiles: lists the available battle profiles." ) );
    m_battle.DoAction( _T( "!loadprofile profilename: loads an available battle profile." ) );
    m_battle.DoAction( _T( "!lock: prevents more people to join." ) );
    m_battle.DoAction( _T( "!map <name>: switches to <name>." ) );
    m_battle.DoAction( _T( "!removebox <allynumber>: deletes <allynumber> start restriction's box." ) );
		m_battle.DoAction( _T( "!ring {name}: rings players that are not ready or {name} if specified." ) );
		m_battle.DoAction( _T( "!set <optionname> <value>: sets battle option <optionname> to <value>" ) );
    m_battle.DoAction( _T( "!spectunsynced: sets all players with unsynced status to be spectators." ) );
    m_battle.DoAction( _T( "!start: starts the battle." ) );
    m_battle.DoAction( _T( "!unlock: opens the battle again." ) );
  }
  else if ( command == _T("!ring") ) {
  	if ( !params.IsEmpty() )
  	{
				wxString user = GetBestMatch( m_userlist, params );
				try
				{
					User& u = m_battle.GetUser( user );
					m_battle.RingPlayer( u );
					m_battle.DoAction( _T( "is ringing " ) + user );
				}
				catch(...)
				{
					m_battle.DoAction( _T( "cannot ring " ) + user );
				}
  	}
    else
    {
			m_battle.RingNotReadyPlayers();
			m_battle.DoAction( _T( "is ringing players not ready ..." ) );
    }
  }
  else if ( command == _T("!listprofiles") ) {
    wxArrayString profilelist = m_battle.GetPresetList();
    unsigned int count = profilelist.GetCount();
    if (count == 0) {
      m_battle.DoAction( _T( "There are no presets available.") );
    }
    else {
      m_battle.DoAction( _T( "The following presets are available:" ) );
      for ( unsigned int i = 0; i < count; i++ ) {
        m_battle.DoAction( profilelist[i] );
      }
    }
  }
  else if ( command == _T("!loadprofile") ) {
    wxString profilename = GetBestMatch( m_battle.GetPresetList(), params );
    if ( !m_battle.LoadOptionsPreset( profilename ) )
      m_battle.DoAction( _T( "Profile not found, use !listprofiles for a list of available profiles." ) );
    else m_battle.DoAction( _T("has loaded profile: ") + profilename );
  }
  else if ( command == _T("!fixcolors") ) {
    m_battle.FixColours();
    m_battle.DoAction( _T( "is fixing colors." ) );
  }
  else if ( command == _T("!lock") ) {
    m_battle.SetIsLocked( true );
    m_battle.DoAction( _T( "has locked the battle." ) );
    m_battle.SendHostInfo( IBattle::HI_Locked );
  }
  else if ( command == _T("!unlock") ) {
    m_battle.SetIsLocked( false );
    m_battle.DoAction( _T( "has unlocked the battle." ) );
    m_battle.SendHostInfo( IBattle::HI_Locked );
  }
  else if ( command == _T("!fixids") ) {
    unsigned int num = s2l( params );
    m_battle.FixTeamIDs( IBattle::balance_divide, false, false, num );
    m_battle.DoAction(_T("is auto-balancing control teams ..."));
  }
  else if ( command == _T("!cfixids") ) {
    unsigned int num = s2l( params );
    m_battle.FixTeamIDs( IBattle::balance_divide, true, true, num );
    m_battle.DoAction(_T("is auto-balancing control teams ..."));
  }
  else if ( command == _T("!spectunsynced") ) {
    m_battle.ForceUnsyncedToSpectate();
    m_battle.DoAction( _T( "is forcing unsynced players to be spectators." ) );
  }
  else if ( command == _T("!map") ) {
  	if ( params.IsEmpty() ) m_battle.DoAction( _T( "cannot switch to void mapname" ) );
  	else
  	{
			wxString mapname = GetBestMatch( usync().GetMapList(), params );
			try
			{
				UnitSyncMap map = usync().GetMap( mapname );
				m_battle.SetLocalMap( map );
				m_battle.DoAction( _T( "is switching to map " ) + mapname );
				m_battle.SendHostInfo( IBattle::HI_Map );
			} catch (...)
			{
				m_battle.DoAction( _T( "cannot switch to map " ) + mapname );
			}
  	}
  }
  else if ( command == _T("!set") ) {
  	wxString key = params.BeforeFirst(_T(' '));
  	wxString value = params.AfterFirst(_T(' '));
  	bool exists = m_battle.CustomBattleOptions().keyExists( key );
  	if ( exists )
  	{
			bool result = m_battle.CustomBattleOptions().setSingleOption( key, value );
			if ( result )
			{
				 OptionsWrapper::GameOption section = m_battle.CustomBattleOptions().GetSection( key );
				 m_battle.SendHostInfo( wxString::Format(_T("%d_%s"), section, key.c_str() ) );
				 m_battle.DoAction( _T("has set option ") + key + _T(" to value ") + value );
			}
			else m_battle.DoAction( _T( "cannot set option " ) + key + _T(" to value ") + value + _T(", reason: invalid value.") );
  	}
  	else
  	{
  		m_battle.DoAction( _T( "cannot find option entry " ) + key );
  	}
  }
  else if ( command == _T("!addbox") ) {
		long allynumber;
		long topleftx;
		long toplefty;
		long bottomrightx;
		long bottomrighty;
		wxArrayString values = wxStringTokenize( params, _T(" ") );
		if ( values.GetCount() != 5 ) m_battle.DoAction( _T("has recieved an invalid number of params for !addbox") );
		else
		{
			bool valueok = values[0].ToLong(&allynumber);
			valueok = valueok && values[1].ToLong(&topleftx);
			valueok = valueok && values[2].ToLong(&toplefty);
			valueok = valueok && values[3].ToLong(&bottomrightx);
			valueok = valueok && values[4].ToLong(&bottomrighty);
			valueok = valueok && ( allynumber > 0 );
			valueok = valueok && ( topleftx >= 0 ) && ( topleftx <= 200 );
			valueok = valueok && ( toplefty >= 0 ) && ( toplefty <= 200 );
			valueok = valueok && ( bottomrightx >= 0 ) && ( bottomrightx <= 200 );
			valueok = valueok && ( bottomrighty >= 0 ) && ( bottomrighty <= 200 );
			if ( valueok )
			{
				allynumber = allynumber - 1;
				BattleStartRect rect = m_battle.GetStartRect( allynumber );
				if ( rect.IsOk() )
				{
					m_battle.DoAction( _T("cannot add a startbox for allyteam ") + TowxString(allynumber) + _T(" because one is already present.") );
				}
				else
				{
					m_battle.AddStartRect( allynumber, topleftx, toplefty, bottomrightx, bottomrighty );
					m_battle.SendHostInfo( IBattle::HI_StartRects );
					m_battle.DoAction( _T("has added start box for allyteam ") + TowxString(allynumber) );
				}
			}
			else
			{
				m_battle.DoAction( _T("has recieved an invalid param for !addbox") );
			}
		}
  }
  else if ( command == _T("!removebox") ) {
  	long boxnumber;
  	bool numberok = params.ToLong( &boxnumber );
  	if ( numberok )
  	{
  		boxnumber = boxnumber - 1;
  		BattleStartRect rect = m_battle.GetStartRect( boxnumber );
  		if ( rect.IsOk() )
  		{
  			m_battle.RemoveStartRect( boxnumber );
  			m_battle.SendHostInfo( IBattle::HI_StartRects );
  			m_battle.DoAction( _T("has removed the start box for allyteam ") + TowxString(boxnumber) );
  		}
  		else
  		{
  			m_battle.DoAction( _T("cannot find start box ") + params );
  		}
  	}
  	else
  	{
  		m_battle.DoAction( _T("has recieved an invalid param to !removebox command") );
  	}
  }
  else return;
  m_lastActionTime = currentTime;
}


/// Should only be called if user isn't immediately kicked (ban / rank limit)
void AutoHost::OnUserAdded( User& user )
{
	m_userlist.Add( user.GetNick() );
  // do nothing if autohost functionality is disabled
  if (!m_enabled)
    return;
  m_battle.DoAction(_T("Hi ") + user.GetNick() + _T(", this battle is in SpringLobby autohost mode. For help say !help"));
}


void AutoHost::OnUserRemoved( User& user )
{
	m_userlist.Remove( user.GetNick() );
  // do nothing if autohost functionality is disabled
  if (!m_enabled)
    return;

  if ( m_battle.GetNumUsers() == 1 && m_battle.IsLocked() )
  {
    m_battle.SetIsLocked( false );
    m_battle.DoAction( _T( "has auto-unlocked the battle." ) );
    m_battle.SendHostInfo( IBattle::HI_Locked );
  }
}


void AutoHost::StartBattle()
{
  // todo: the logic here is copied from BattleRoomTab::OnStart, may wish to refactor this sometime.
  // note: the strings here must remain untranslated because they're visible to everyone in the battle!

  m_battle.GetMe().BattleStatus().ready = true;

  if ( !m_battle.IsEveryoneReady() ) {
    m_battle.DoAction(_T("Some players are not ready yet."));
    return;
  }

  m_battle.DoAction(_T("is starting game ..."));
  m_battle.GetServer().StartHostedBattle();

	m_battle.SaveMapDefaults(); // save map preset

  // todo: copied from Ui::StartHostedBattle
  sett().SetLastHostMap( m_battle.GetHostMapName() );
  sett().SaveSettings();
}
