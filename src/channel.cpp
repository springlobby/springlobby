/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Channel
//

#include "channel.h"
#include "ui.h"
#include "server.h"
#include "user.h"
#include "utils.h"

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
  if(banned_users.count(name)>0){
    m_serv.SayPrivate(_T("ChanServ"),_T("!kick #")+GetName()+_T(" ")+name);
  }
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
  } else if ( param == _T("/sayver") ) {
    DoAction( _T("is using SpringLobby v") + GetSpringLobbyVersion() );
    return true;
  } else if(subcmd==_T("/userban")){
    banned_users.insert(params);
    m_serv.SayPrivate(_T("ChanServ"),_T("!kick #")+GetName()+_T(" ")+params);
    return true;
  } else if(subcmd==_T("/userunban")){
    banned_users.erase(params);
    return true;
  }

  return false;
}

