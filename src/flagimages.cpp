#include "flagimages.h"
#include "utils.h"

int GetFlagIndex( const std::string& flag )
{
  int i = 0;
  while ( strcmp(flag_str[i], "") != 0 ) {
    if ( flag == flag_str[i] ) { 
      return i;
    }
    i++;
  }
  debug_error( flag + " flag not found!" );  
  return FLAG_NONE;
}

void AddFlagImages( wxImageList& imgs )
{
  int i = 0;
  while ( flag_xpm[i] != NULL ) {
    imgs.Add( wxBitmap( const_cast<const char**>(flag_xpm[i])) );
    i++;
  }
}

