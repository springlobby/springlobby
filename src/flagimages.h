#ifndef SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H
#define SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H

#include <wx/string.h>

class wxImageList;

int GetFlagIndex( const wxString& flag );
int AddFlagImages( wxImageList& imgs );

enum {
  FLAG_NONE = -1
};

#endif // SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H
