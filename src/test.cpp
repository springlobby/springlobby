// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>
#include <assert.h>
//#include <wx/wx.h>

#include "chatlist.h"
#include "settings.h"

void test_chatlist()
{
  cout << "* Testing ChatList... ";
  ChatList* chatlist = new ChatList();
  assert( chatlist != NULL );
  
  void* chatptr = (void*)chatlist->GetChannelPanel( string("does not exist") );
  assert( chatptr == NULL ); // List should be empty.

  chatptr = (void*)chatlist->GetPrivChatPanel( string("does not exist") );
  assert( chatptr == NULL ); // List should be empty.
  
  assert( chatlist->NumChanels() == 0 );
  
  chatlist->SetChannelPanel( string("test_chat1"), (ChatPanel*)101 );
  assert( chatlist->NumChanels() == 1 );
  chatlist->SetChannelPanel( string("test_chat"), (ChatPanel*)100 );
  assert( chatlist->NumChanels() == 2 );
  chatlist->SetChannelPanel( string("test_chat2"), (ChatPanel*)102 );
  assert( chatlist->NumChanels() == 3 );
  chatptr = (void*)chatlist->GetChannelPanel( string("test_chat") );
  assert( chatptr == (void*)100 ); // Should return same as set.

  chatlist->SetChannelPanel( string("test_chat"), (ChatPanel*)500 );
  assert( chatlist->NumChanels() == 3 );
  chatptr = (void*)chatlist->GetChannelPanel( string("test_chat") );
  assert( chatptr == (void*)500 ); // Should be updated.  
  
  chatlist->SetPrivChatPanel( string("test_priv"), (ChatPanel*)200 );
  chatlist->SetPrivChatPanel( string("test_priv1"), (ChatPanel*)300 );
  chatlist->SetPrivChatPanel( string("test_priv2"), (ChatPanel*)400 );
  chatptr = (void*)chatlist->GetPrivChatPanel( string("test_priv") );

  assert( chatptr == (void*)200 ); // Should return same as set.

  chatptr = (void*)chatlist->GetChannelPanel( string("does not exist2") );
  assert( chatptr == NULL ); // Should still return NULL.

  chatptr = (void*)chatlist->GetPrivChatPanel( string("dofghfghst") );
  assert( chatptr == NULL ); // Should still return NULL.
  
  assert( chatlist->NumChanels() == 3 );
  assert( chatlist->ChannelExists( string( "test_chat" ) ) );
  
  chatlist->RemoveChannelPanel( string( "test_chat" ) );
  assert( chatlist->NumChanels() == 2 );
  assert( !chatlist->ChannelExists( string( "test_chat" ) ) );
  
  delete chatlist;
  
  cout << "Sucessful." << endl;
  
}

void test_settings()
{
  cout << "* Testing Settings... " << endl;
  
  Settings* sett = new Settings();
  assert( sett != NULL );
  string tmp_def_srvr = sett->GetDefaultServer(); // Save value before playing with it.
  
  sett->SetDefaultServer( "test" );
  assert( sett->GetDefaultServer() == string("test") ); // Should have been set.
  sett->SetDefaultServer( "test2" );
  assert( sett->GetDefaultServer() == string("test2") ); // Should have been set.

  delete sett;
  sett = new Settings();
  
  assert( sett->GetDefaultServer() == string("test2") ); // Should still have same value.
  assert( WX_STRING(sett->GetDefaultServer()) == WX_STRING(string("test2")) ); // Should still have same value.
  
  sett->SetDefaultServer( tmp_def_srvr ); // Restore old value.

  delete sett;
  
  cout << "Sucessful." << endl;
}

int main()
{
  cout << "* Starting tests." << endl;
  test_chatlist();
  test_settings();
  cout << "* All tests successfull!" << endl;
  
  return 0;
}
