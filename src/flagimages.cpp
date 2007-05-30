
#include <string>
#include <iostream>
#include "flagimages.h"

int GetFlagIndex( const std::string& flag )
{
  int i = 0;
  while ( strcmp(flag_str[i], "") != 0 ) {
    if ( flag == flag_str[i] ) { 
      return i;
    }
    i++;
  }
  std::cout << "!! GetFlagIndex( " << flag.c_str() << " ): Not Found!" << std::endl;  
  return FLAG_NONE;
}
