/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include "flagimages.h"
#include "flagimagedata.h"
#include "utils.h"

#include <wx/imaglist.h>

int GetFlagIndex( const wxString& flag )
{
  for (int i = 0; flag_str[i]; ++i) {
    if ( flag == WX_STRINGC(flag_str[i]) ) {
      return i;
    }
  }
  wxLogMessage( _T("%s flag not found!"), flag.c_str() );
  return FLAG_NONE;
}

void AddFlagImages( wxImageList& imgs )
{
  for (int i = 0; flag_xpm[i]; ++i) {
    imgs.Add( wxBitmap( const_cast<const char**>(flag_xpm[i])) );
  }
}

