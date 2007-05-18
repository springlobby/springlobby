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

    void Said( const string& who, const string& message );
    void Said( User& who, const string& message );
 
    void DidAction( const string& who, const string& action );
    void DidAction( User& who, const string& action );
  
    void Left( const string& who );
    void Left( User& who );
  
    void Joined( const string& who );
  
  
    // Callbacks
    void SetOnSaidCallback( channel_msg_callback callback );
    channel_msg_callback GetOnSaidCallback();

    void SetOnDidActionCallback( channel_msg_callback callback );
    channel_msg_callback GetOnDidActionCallback();

    void SetOnLeaveCallback( channel_cmd_callback callback );
    channel_cmd_callback GetOnLeaveCallback();

    void SetOnLeftCallback( channel_whocmd_callback callback );
    channel_whocmd_callback GetOnLeftCallback();

    void SetOnJoinedCallback( channel_whocmd_callback callback );
    channel_whocmd_callback GetOnJoinedCallback();

  protected:
    
    // Channel variables
    channel_msg_callback m_said_cb;
    channel_msg_callback m_action_cb;
    channel_cmd_callback m_leave_cb;
    channel_whocmd_callback m_left_cb;
    channel_whocmd_callback m_joined_cb;
  
    string m_name;
    void* m_userdata;
  
    void AddUser( User* user );
    void RemoveUser( const string nick );
  
};


#endif  //_CHANNEL_H_

