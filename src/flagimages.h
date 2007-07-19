//
// flagimages.h
//

#ifndef _FLAGIMAGES_H_
#define _FLAGIMAGES_H_

#include <string>

class wxImageList;

int GetFlagIndex( const std::string& flag );
void AddFlagImages( wxImageList& imgs );

const char* flag_str[];
const char** flag_xpm[];

enum {
  FLAG_NONE = -1
};

#endif // _FLAGIMAGES_H_

