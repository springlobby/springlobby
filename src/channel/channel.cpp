/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Channel
//

#include "channel.h"
#include "../ui.h"
#include "../server.h"
#include "../user.h"
#include "../utils/tasutil.h"
#include "../utils/debug.h"
#include "../utils/platform.h"
#include <wx/regex.h>
#include <wx/log.h>
#include "../chatpanel.h"

Channel::~Channel() {
  if(uidata.panel)uidata.panel->SetChannel(NULL);
}

void Channel::SetName( const wxString& name )
{
  m_name = name;
}


wxString Channel::GetName()
{
  return m_name;
}


User& Channel::GetMe()
{
  return m_serv.GetMe();
}


void Channel::Said( User& who, const wxString& message )
{
  m_ui.OnChannelSaid( *this , who, message );
}


void Channel::Say( const wxString& message )
{
  wxLogDebugFunc( _T("") );
  m_serv.SayChannel( m_name, message );
}


void Channel::DidAction( User& who, const wxString& action )
{
  m_ui.OnChannelDidAction( *this, who, action );
}


void Channel::DoAction( const wxString& action )
{
  wxLogDebugFunc( _T("") );
  m_serv.DoActionChannel( m_name, action );
}


void Channel::Left( User& who, const wxString& reason )
{
  RemoveUser( who.GetNick() );
  m_ui.OnUserLeftChannel( *this, who, reason );
}


void Channel::Leave()
{
  m_serv.PartChannel( m_name );
}



void Channel::Joined( User& who )
{
  AddUser( who );
  m_ui.OnUserJoinedChannel( *this, who );
}


void Channel::OnChannelJoin( User& who )
{
  AddUser( who );
  m_ui.OnChannelJoin( *this, who );
}


void Channel::SetTopic( const wxString& topic, const wxString& who )
{
  m_topic = topic;
  m_topic_nick = who;

  m_ui.OnChannelTopic( *this, who, topic );
}

wxString Channel::GetTopicSetBy()
{
  return m_topic_nick;
}


wxString Channel::GetTopic()
{
  return m_topic;
}


void Channel::AddUser( User& user )
{
  UserList::AddUser( user );
  CheckBanned(user.GetNick());
}

void Channel::CheckBanned(const wxString& name){
  if(name==_T("ChanServ"))return;
  if(m_banned_users.count(name)>0){
    m_serv.SayPrivate(_T("ChanServ"),_T("!kick #")+GetName()+_T(" ")+name);
  }
  if(m_do_ban_regex&&m_ban_regex.IsValid()){
    if(m_ban_regex.Matches(name)&&!(m_do_unban_regex&&m_unban_regex.IsValid()&&m_unban_regex.Matches(name))){
      m_serv.SayPrivate(_T("ChanServ"),_T("!kick #")+GetName()+_T(" ")+name);
      if(!m_ban_regex_msg.empty())m_serv.SayPrivate(name,m_ban_regex_msg);
    }
  }
};
bool Channel::IsBanned(const wxString& name){
  if(name==_T("ChanServ"))return false;
  if(m_banned_users.count(name)>0)return true;
  if(m_do_ban_regex&&m_ban_regex.IsValid()){
    if(m_ban_regex.Matches(name)&&!(m_do_unban_regex&&m_unban_regex.IsValid()&&m_unban_regex.Matches(name)))return true;
  }
  return false;
};


void Channel::RemoveUser( const wxString& nick )
{
  UserList::RemoveUser( nick );
}


bool Channel::ExecuteSayCommand( const wxString& in )
{
  if ( in.length() == 0 ) return true;

  if ( in[0] != '/' ) return false;

  wxString subcmd = in.BeforeFirst(' ').Lower();
  wxString params = in.AfterFirst( ' ' );

  wxString cmdline = in;
  wxString param = GetWordParam( cmdline );
  if ( param == _T("/me") ) {
    DoAction( cmdline );
    return true;
	} else if ( in == _T("/part") || in == _T("/p") ) {
		Leave();
		uidata.panel = 0;
		return true;
  } else if ( param == _T("/sayver") ) {
    DoAction( _T("is using SpringLobby v") + GetSpringLobbyVersion() );
    return true;
  } else if(subcmd==_T("/userban")){
    m_banned_users.insert(params);
    m_serv.SayPrivate(_T("ChanServ"),_T("!kick #")+GetName()+_T(" ")+params);
    return true;
  } else if(subcmd==_T("/userunban")){
    m_banned_users.erase(params);
    return true;
  } else if(subcmd==_T("/banregex")){
    m_ui.OnChannelMessage(m_name,_T("/banregex ")+params);
    m_do_ban_regex=!params.empty();
    if(m_do_ban_regex){
      #ifdef wxHAS_REGEX_ADVANCED
      m_ban_regex.Compile(params, wxRE_ADVANCED);
      #else
      m_ban_regex.Compile(params, wxRE_EXTENDED);
      #endif
      if(!m_ban_regex.IsValid())m_ui.OnChannelMessage(m_name,_T("Invalid regular expression"));
    }
    return true;
  } else if(subcmd==_T("/unbanregex")){
    m_ui.OnChannelMessage(m_name,_T("/unbanregex ")+params);
    m_do_unban_regex=!params.empty();
    if(m_do_unban_regex){
      #ifdef wxHAS_REGEX_ADVANCED
      m_unban_regex.Compile(params, wxRE_ADVANCED);
      #else
      m_unban_regex.Compile(params, wxRE_EXTENDED);
      #endif
      if(!m_unban_regex.IsValid())m_ui.OnChannelMessage(m_name,_T("Invalid regular expression"));
    }
    return true;
  }else if(subcmd==_T("/checkban")){
    if(IsBanned(params)){
      m_ui.OnChannelMessage(m_name,params+_T(" is banned"));
    }else{
      m_ui.OnChannelMessage(m_name,params+_T(" is not banned"));
    }
    return true;
  }


  else if(subcmd==_T("/banregexmsg")){
    m_ui.OnChannelMessage(m_name,_T("/banregexmsg ")+params);
    m_ban_regex_msg=params;
    return true;
  }

  return false;
}


wxString Channel::GetPassword()
{
  return m_password;
}


void Channel::SetPassword( const wxString& pw )
{
  m_password = pw;
}

