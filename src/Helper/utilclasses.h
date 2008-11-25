#ifndef SPRINGLOBBY_UTILCLASSES_H_INCLUDED
#define SPRINGLOBBY_UTILCLASSES_H_INCLUDED

#include <vector>

struct SortOrderItem {
    int col;
    int direction;
};
typedef std::map<int,SortOrderItem> SortOrder;

#endif // SPRINGLOBBY_UTILCLASSES_H_INCLUDED
