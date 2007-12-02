#ifndef SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H
#define SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H

#include <string>

class wxImageList;

int GetFlagIndex( const std::string& flag );
void AddFlagImages( wxImageList& imgs );

enum {
  FLAG_NONE = -1
};

#endif // SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H
