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
// Class: ChatList
// Created on: Sat May  5 21:41:21 2007
//

#ifndef _CHATLIST_H_
#define _CHATLIST_H_

#include <map>
#include <string>
#include "chatpanel.h"

using namespace std;

struct ltstr
{
  bool operator()(const string s1, const string s2) const
  {
    return s1.compare(s2) < 0;
  }
};

typedef map<string, ChatPanel*, ltstr>::iterator chatwin_iter_t;
typedef map<string, ChatPanel*, ltstr> chatwin_map_t;

class ChatList
{
  public:
    ChatList();
    virtual ~ChatList();
  
    // ChatList interface
  
    void SetServerPanel( ChatPanel* servchat ) { m_serv = servchat; }
    ChatPanel* GetServerPanel() { assert(m_serv != NULL); return m_serv; }
  
    ChatPanel* GetChannelPanel( string chan_name );
    void SetChannelPanel( string chan_name, ChatPanel* window );
    bool ChannelExists( string chan_name );
    void RemoveChannelPanel( string chan_name );
    int NumChanels();
  
    ChatPanel* GetPrivChatPanel( string nick_name );
    void SetPrivChatPanel( string nick_name, ChatPanel* window );
    bool PrivChatExists( string nick_name );

    int NumPrivChat();

  protected:
    // ChatList variables
  
    ChatPanel* m_serv;
    
    chatwin_map_t m_channels;
    chatwin_map_t m_priv_chats;
  
};


#endif  //_CHATLIST_H_

