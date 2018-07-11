/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_SORTUTIL_H_INCLUDED
#define SPRINGLOBBY_SORTUTIL_H_INCLUDED

#include <map>


//! set direction to +1 for down, -1 for up
struct SortOrderItem
{
	int col;
	int direction;
};
//! map sort priority <--> ( column, direction )
typedef std::map<int, SortOrderItem> SortOrder;


//! the sort algo used in almost all ListCtrls
template <class ContainerType, class Comparator>
void SLInsertionSort(ContainerType& data, const Comparator& cmp)
{
	const int n = data.size();
	for (int i = 0; i < n; i++) {
		typename Comparator::ObjType v = data[i];
		int j;

		for (j = i - 1; j >= 0; j--) {
			if (cmp(data[j], v))
				break;
			data[j + 1] = data[j];
		}
		data[j + 1] = v;
	}
}

// Returns 1 if the second argument is considered greater/newer than the first,
// 0 if they are identical and -1 otherwise.
int CompareVersionStrings(const std::string& lhs, const std::string& rhs);

struct CompareVersionStringsFunctor {
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{ return 1 == CompareVersionStrings(lhs, rhs); }
};

extern CompareVersionStringsFunctor compareVersionStringsFunctor;
#endif // SPRINGLOBBY_SORTUTIL_H_INCLUDED
