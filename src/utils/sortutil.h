/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_SORTUTIL_H_INCLUDED
#define SPRINGLOBBY_SORTUTIL_H_INCLUDED

#include <map>
//! set direction to +1 for down, -1 for up
struct SortOrderItem {
	int col;
	int direction;
};
//! map sort priority <--> ( column, direction )
typedef std::map<int, SortOrderItem> SortOrder;

// Returns 1 if the second argument is considered greater/newer than the first,
// 0 if they are identical and -1 otherwise.
int CompareVersionStrings(const std::string& lhs, const std::string& rhs);

struct CompareVersionStringsFunctor {
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{
		return 1 == CompareVersionStrings(lhs, rhs);
	}
};

extern CompareVersionStringsFunctor compareVersionStringsFunctor;


template <class T>
int GenericCompare(const T& itemA, const T& itemB)
{
	if (itemA < itemB)
		return 1;
	else if (itemA > itemB)
		return -1;
	else
		return 0;
}

#endif // SPRINGLOBBY_SORTUTIL_H_INCLUDED
