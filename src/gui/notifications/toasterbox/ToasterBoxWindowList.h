/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_TOASTERWINDOWLIST_HEADERGUARD
#define SPRINGLOBBY_TOASTERWINDOWLIST_HEADERGUARD

#include <wx/list.h>
#include "ToasterBoxWindow.h"
WX_DECLARE_LIST(ToasterBoxWindow, ToasterBoxWindowList);
typedef ToasterBoxWindowList::compatibility_iterator
    ToasterBoxWindowListNode;

#endif // SPRINGLOBBY_TOASTERWINDOWLIST_HEADERGUARD
