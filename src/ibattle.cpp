/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/tokenzr.h>
#include <wx/image.h>

#include "ibattle.h"
#include "utils.h"
#include "uiutils.h"
#include "settings.h"
#include "ui.h"
#include "images/fixcolours_palette.xpm"

#include <list>


IBattle::IBattle():
  m_map_loaded(false),
  m_mod_loaded(false),
  m_map_exists(false),
  m_mod_exists(false),
  m_ingame(false)
{
}


IBattle::~IBattle()
{
}




std::vector<wxColour> &IBattle::GetFixColoursPalette()
{
    static std::vector<wxColour> result;
    if (result.empty())
    {
        wxImage image(fixcolours_palette_xpm);
        unsigned char* data=image.GetData();
        size_t len=image.GetWidth()*image.GetHeight();
        for (size_t i=0;i<len;++i)
        {
            int r=data[i*3];
            int g=data[i*3+1];
            int b=data[i*3+2];
            if (r||g||b)
            {
                result.push_back(wxColour(r,g,b));
            }
        }
    }
    return result;
}

wxColour IBattle::GetFixColour(int i)
{
    std::vector<wxColour> palette = GetFixColoursPalette();
    if (((unsigned int)i)<palette.size())
    {
        return palette[i];
    }
    else
    {
        return wxColour(127,127,127);
    }
}

int IBattle::GetFreeTeamNum( bool excludeme ) const
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( ( &GetUser( i ) == &GetMe() ) && excludeme ) continue;
            //if ( GetUser( i ).BattleStatus().spectator ) continue;
            if ( GetUser( i ).BattleStatus().team == lowest )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return lowest;
}


wxColour IBattle::GetFreeColour( User *for_whom ) const
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( &GetUser( i ) == for_whom ) continue;
            //if ( GetUser( i ).BattleStatus().spectator ) continue;
            UserBattleStatus& bs = GetUser( i ).BattleStatus();
            if ( AreColoursSimilar( bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return wxColour( colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] );

}

int IBattle::ColourDifference(const wxColour &a, const wxColour &b) // returns max difference of r,g,b.
{
    return std::max(abs(a.Red()-b.Red()),std::max(abs(a.Green()-b.Green()),abs(a.Blue()-b.Blue())));

}

int IBattle::GetClosestFixColour(const wxColour &col, const std::vector<int> &excludes, int &difference)
{
    std::vector<wxColour> palette = GetFixColoursPalette();
    int mindiff=1024;
    int result=0;
    int t1=palette.size();
    int t2=excludes.size();
    wxLogMessage(_T("GetClosestFixColour %d %d"),t1,t2);
    for (size_t i=0;i<palette.size();++i)
    {
        if ((i>=excludes.size()) || (!excludes[i]))
        {
            int diff=ColourDifference(palette[i],col);
            if (diff<mindiff)
            {
                mindiff=diff;
                result=i;
            }
        }
    }
    difference=mindiff;
    wxLogMessage(_T("GetClosestFixColour result=%d diff=%d"),result,difference);
    return result;
}

bool IBattle::HaveMultipleBotsInSameTeam() const
{
    std::list<BattleBot*>::const_iterator i;
    wxLogDebugFunc(_T(""));

    std::vector<int> teams ( GetMaxPlayers(), -1 );
		for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
    {
				User& usr = GetUser( i );
        if ( !usr.BattleStatus().IsBot() ) continue;
        if ( teams[ usr.BattleStatus().team ] == 1 )return true;
        teams[ usr.BattleStatus().team ] = 1;
    }
    return false;
}

void IBattle::OnUserAdded( User& user )
{
    UserList::AddUser( user );

    user.BattleStatus().spectator = false;
    user.BattleStatus().ready = false;
    user.BattleStatus().sync = SYNC_UNKNOWN;
}

void IBattle::OnBotAdded( const wxString& nick, const wxString& owner, const UserBattleStatus& bs, const wxString& aidll )
{
		User user();
		m_internal_bot_list.push_back( user );
    UserList::AddUser( user );

    user().BattleStatus().spectator = false;
    user().BattleStatus().ready = false;
    user().BattleStatus().sync = SYNC_SYNCED;
    user().SetNick( nick );
		user().BattleStatus().owner = owner;
		user().BattleStatus().ailib = aidll;
}

unsigned int IBattle::GetNumBots() const
{
		return m_internal_bot_list.size();
}

unsigned int IBattle::GetNumPlayers() const
{
		return GetNumUsers() - GetNumBots();
}

void IBattle::OnUserBattleStatusUpdated( User &user, UserBattleStatus status )
{

    bool previousspectatorstatus = user.BattleStatus().spectator;
    int previousteam = user.BattleStatus().team;
    int previousally = user.BattleStatus().ally;

    user.UpdateBattleStatus( status );


    if ( IsFounderMe() )
    {
			if ( status.spectator != previousspectatorstatus )
			{
					if ( status.spectator )
					{
							m_opts.spectators++;
					}
					else
					{
							m_opts.spectators--;
					}
					SendHostInfo( HI_Spectators );
			}
			if ( m_opts.lockexternalbalancechanges )
			{
				if ( previousteam != user.BattleStatus().team ) ForceTeam( user, previousteam );
				if ( previousally != user.BattleStatus().ally ) ForceAlly( user, previousally );
			}
    }
}

void IBattle::OnUserRemoved( User& user )
{
    if ( IsFounderMe() && user.BattleStatus().spectator )
    {
      m_opts.spectators--;
      SendHostInfo( HI_Spectators );
    }
    user.SetBattle( 0 );
    if ( user.IsBot() )
    {
    	for( UserVecCIter i = m_internal_bot_list.begin(); i != m_internal_bot_list.end(); i++ )
    	{
    		if ( i->second == user )
    		{
    			m_internal_bot_list.erase( i );
    			break;
    		}
    	}
    }
    UserList::RemoveUser( user.GetNick() );
}

bool IBattle::IsEveryoneReady()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
				User& usr = GetUser(i);
				if ( usr.IsBot() ) continue;
        UserBattleStatus& bs = usr.BattleStatus();
        if ( !bs.ready && !bs.spectator ) return false;
    }
    return true;
}



void IBattle::AddStartRect( unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom )
{
    BattleStartRect sr;

    sr.ally = allyno;
    sr.left = left;
    sr.top = top;
    sr.right = right;
    sr.bottom = bottom;
    sr.toadd = true;
    sr.todelete = false;
    sr.toresize = false;
    sr.exist = true;

    m_rects[allyno] = sr;
}



void IBattle::RemoveStartRect( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.todelete = true;
    m_rects[allyno] = sr;
}


void IBattle::ResizeStartRect( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toresize = true;
    m_rects[allyno] = sr;
}


void IBattle::StartRectRemoved( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    if ( m_rects[allyno].todelete ) m_rects.erase(allyno);
}


void IBattle::StartRectResized( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toresize = false;
    m_rects[allyno] = sr;
}


void IBattle::StartRectAdded( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toadd = false;
    m_rects[allyno] = sr;
}


BattleStartRect IBattle::GetStartRect( unsigned int allyno )
{
    return m_rects[allyno];
}

unsigned int IBattle::GetNumRects()
{
    return m_rects.size();
}

void IBattle::ClearStartRects()
{
    m_rects.clear();
}

void IBattle::ForceSide( User& user, int side )
{
	if ( usr.IsBot() ) usr.BattleStatus().side = side;
}

void IBattle::ForceTeam( User& user, int team )
{
  if ( IsFounderMe() || user.BattleStatus().IsBot() )
  {
    user.BattleStatus().team = team;
    ui().OnUserBattleStatus( *this, user );
  }
}


void IBattle::ForceAlly( User& user, int ally )
{

  if ( IsFounderMe() || user.BattleStatus().IsBot() )
  {
    user.BattleStatus().team = team;// update locally first so locking status changes won't revert host's
    ui().OnUserBattleStatus( *this, user );
  }

}


void IBattle::ForceColour( User& user, const wxColour& col )
{
  if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().colour = col;
			 ui().OnUserBattleStatus( *this, user );
		}

}


void IBattle::ForceSpectator( User& user, bool spectator )
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().colour = col;
			 ui().OnUserBattleStatus( *this, user );
		}
}


void IBattle::BattleKickPlayer( User& user )
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 ui().OnUserLeftBattle( *this, user );
			 OnUserRemoved( user );
		}
}

int IBattle::GetFreeAlly()
{
  int lowest = 0;
  bool changed = true;
  while ( changed ) {
    changed = false;
    for ( unsigned int i = 0; i < GetNumUsers(); i++ )
    {
      User& user = GetUser( i );
      if ( user.BattleStatus().ally == lowest )
      {
        lowest++;
        changed = true;
      }
    }
  }
  return lowest;
}

void IBattle::GetFreePosition( int& x, int& y )
{
  UnitSyncMap map = LoadMap();
  for ( int i = 0; i < map.info.posCount; i++ )
	{
    bool taken = false;
    for ( unsigned int bi = 0; bi < GetNumUsers(); bi++ )
    {
      User& user = GetBot( bi );
      if ( ( map.info.positions[i].x == user.BattleStatus().posx ) && ( map.info.positions[i].y == user.BattleStatus().posy ) )
      {
        taken = true;
        break;
      }
    }
    if ( !taken )
    {
      x = CLAMP(map.info.positions[i].x, 0, map.info.width);
      y = CLAMP(map.info.positions[i].y, 0, map.info.height);
      return;
    }
  }
  x = map.info.width / 2;
  y = map.info.height / 2;
}

void IBattle::SetHandicap( User& user, int handicap)
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() ) user.BattleStatus().colour = col;
    m_serv.SetHandicap ( m_opts.battleid, user.GetNick(), handicap );
}

void IBattle::SetHostMap(const wxString& mapname, const wxString& hash)
{
  if ( mapname != m_host_map.name || hash != m_host_map.hash ) {
    m_map_loaded = false;
    m_host_map.name = mapname;
    m_host_map.hash = hash;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
  }
}


void IBattle::SetLocalMap(const UnitSyncMap& map)
{
  if ( map.name != m_local_map.name || map.hash != m_local_map.hash ) {
    m_local_map = map;
    m_map_loaded = true;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
  }
}


const UnitSyncMap& IBattle::LoadMap()
{

  if ( !m_map_loaded ) {
    try {
      ASSERT_EXCEPTION( m_map_exists, _T("Map does not exist.") );
      m_local_map = usync().GetMapEx( m_host_map.name );
      m_map_loaded = true;

    } catch (...) {}
  }
  return m_local_map;
}


wxString IBattle::GetHostMapName() const
{
  return m_host_map.name;
}


wxString IBattle::GetHostMapHash() const
{
  return m_host_map.hash;
}


void IBattle::SetHostMod( const wxString& modname, const wxString& hash )
{
  if ( m_host_mod.name != modname || m_host_mod.hash != hash ){
    m_mod_loaded = false;
    m_host_mod.name = modname;
    m_host_mod.hash = hash;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


void IBattle::SetLocalMod( const UnitSyncMod& mod )
{
  if ( mod.name != m_local_mod.name || mod.hash != m_local_mod.hash ) {
    m_local_mod = mod;
    m_mod_loaded = true;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


const UnitSyncMod& IBattle::LoadMod()
{
  if ( !m_mod_loaded ) {
    try {
      ASSERT_EXCEPTION( m_mod_exists, _T("Mod does not exist.") );
      m_local_mod = usync().GetMod( m_host_mod.name );
      m_mod_loaded = true;
    } catch (...) {}
  }
  return m_local_mod;
}


wxString IBattle::GetHostModName() const
{
  return m_host_mod.name;
}


wxString IBattle::GetHostModHash() const
{
  return m_host_mod.hash;
}


bool IBattle::MapExists()
{
  return m_map_exists;
  //return usync().MapExists( m_map_name, m_map.hash );
}


bool IBattle::ModExists()
{
  return m_mod_exists;
  //return usync().ModExists( m_mod_name );
}



void IBattle::DisableUnit( const wxString& unitname )
{
  if ( m_units.Index( unitname ) == wxNOT_FOUND ) m_units.Add( unitname );
}


void IBattle::EnableUnit( const wxString& unitname )
{
  int pos = m_units.Index( unitname );
  if ( pos == wxNOT_FOUND ) return;
  m_units.RemoveAt( pos );
}


void IBattle::EnableAllUnits()
{
  m_units.Empty();
}


wxArrayString IBattle::DisabledUnits()
{
  return m_units;
}


void IBattle::OnUnitSyncReloaded()
{
  if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash);
  else m_mod_exists = usync().ModExists( m_host_mod.name );
  if ( !m_host_map.hash.IsEmpty() )  m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
  else  m_map_exists = usync().MapExists( m_host_map.name );
}


unsigned int IBattle::AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll )
{
  return (unsigned int)(-1);/// note: that looks pretty crappy and needs to be investigated.
}


static wxString FixPresetName( const wxString& name )
{
  // look name up case-insensitively
  const wxArrayString& presetList = sett().GetPresetList();
  int index = presetList.Index( name, false /*case insensitive*/ );
  if ( index == -1 ) return _T("");

  // set preset to the actual name, with correct case
  return presetList[index];
}


bool IBattle::LoadOptionsPreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if (preset == _T("")) return false; ///preset not found
  m_preset = preset;

  for ( unsigned int i = 0; i < OptionsWrapper::LastOption; i++)
  {
    std::map<wxString,wxString> options = sett().GetHostingPreset( m_preset, i );
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      for ( std::map<wxString,wxString>::iterator itor = options.begin(); itor != options.end(); itor++ )
      {
        CustomBattleOptions().setSingleOption( itor->first, itor->second, (OptionsWrapper::GameOption)i );
      }
    }
    else
    {
      if ( !options[_T("mapname")].IsEmpty() )
      {
        UnitSyncMap map = usync().GetMapEx( options[_T("mapname")] );
        SetLocalMap( map );

        SendHostInfo( HI_Map );
      }
      unsigned int localrectcount = GetNumRects();
      for( unsigned int localrect = 0 ; localrect < localrectcount; ++localrect) if ( GetStartRect( localrect ).exist ) RemoveStartRect( localrect );
      SendHostInfo( HI_StartRects );

      unsigned int rectcount = s2l( options[_T("numrects")] );
      for ( unsigned int loadrect = 0; loadrect < rectcount; loadrect++)
      {
        int ally = s2l(options[_T("rect_") + TowxString(loadrect) + _T("_ally")]);
        if ( ally == 0 ) continue;
        AddStartRect( ally - 1, s2l(options[_T("rect_") + TowxString(loadrect) + _T("_left")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_top")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_right")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_bottom")]) );
      }
      SendHostInfo( HI_StartRects );

      m_units = wxStringTokenize( options[_T("restrictions")], _T('\t') );
      SendHostInfo( HI_Restrictions );
      Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );

    }
  }
  SendHostInfo( HI_Send_All_opts );
  ui().ReloadPresetList();
  return true;
}


void IBattle::SaveOptionsPreset( const wxString& name )
{
  m_preset = FixPresetName(name);
  if (m_preset == _T("")) m_preset = name; ///new preset

  for ( int i = 0; i < (int)OptionsWrapper::LastOption; i++)
  {
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, CustomBattleOptions().getOptionsMap( (OptionsWrapper::GameOption)i ) );
    }
    else
    {
      std::map<wxString,wxString> opts;
      opts[_T("mapname")] = GetHostMapName();
      unsigned int validrectcount = 0;
      if ( s2l (CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ) ) == ST_Choose )
      {
        unsigned int boxcount = GetNumRects();
        for ( unsigned int boxnum = 0; boxnum < boxcount; boxnum++ )
        {
          BattleStartRect rect = GetStartRect( boxnum );
          if ( rect.IsOk() )
          {
            opts[_T("rect_") + TowxString(validrectcount) + _T("_ally")] = TowxString( rect.ally + 1 );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_left")] = TowxString( rect.left );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_top")] = TowxString( rect.top );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_bottom")] = TowxString( rect.bottom );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_right")] = TowxString( rect.right );
            validrectcount++;
          }
        }
      }
      opts[_T("numrects")] = TowxString( validrectcount );

      unsigned int restrcount = m_units.GetCount();
      wxString restrictionsstring;
      for ( unsigned int restrnum = 0; restrnum < restrcount; restrnum++ )
      {
        restrictionsstring << m_units[restrnum] << _T('\t');
      }
      opts[_T("restrictions")] = restrictionsstring;

      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, opts );
    }
  }
  sett().SaveSettings();
  ui().ReloadPresetList();
}


wxString IBattle::GetCurrentPreset()
{
  return m_preset;
}


void IBattle::DeletePreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if ( m_preset == preset ) m_preset = _T("");
  sett().DeletePreset( preset );
  ui().ReloadPresetList();
}


wxArrayString IBattle::GetPresetList()
{
  return sett().GetPresetList();
}
