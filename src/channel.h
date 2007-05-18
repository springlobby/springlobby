/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: Channel
#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "userlist.h"

class Channel;

typedef void(*channel_msg_callback)(Channel&,User&,const string&);
typedef void(*channel_cmd_callback)(Channel&);
typedef void(*channel_whocmd_callback)(Channel&,User&);

class Channel : public UserList
{
  public:
    Channel();
    virtual ~Channel();
  
    // Channel interface
  
    // Properties
    void SetName( const string& name );
    string GetName();
  
    void SetUserData( void* userdata );
    void* GetUserData();

    // Channel Functions
    void Say( const string& message );
    void DoAction( const string& action );
    void Leave();

    void Said( User& who, const string& message );
 
    void DidAction( User& who, const string& action );
  
    void Left( User& who, const string& reason );
    void Joined( User& who );
  
    void SetTopic( const string& topic, User& who );
    string GetTopic();
    string GetTopicSetBy();
  
  protected:
    
    // Channel variables
  
    string m_topic;
    string m_topic_nick;
    string m_name;
    void* m_userdata;
  
    void AddUser( User* user );
    void RemoveUser( const string nick );
  
};


#endif  //_CHANNEL_H_

