
#include <string>
#include "flagimages.h"

int GetFlagIndex( const std::string& flag )
{
  for ( int i = 0; i < sizeof(flag_str); i++ ) {
    if ( flag == flag_str[i] )
      return i;
  }
  return FLAG_NONE;
}
