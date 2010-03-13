#ifndef SPRINGLOBBY_SORTUTIL_H_INCLUDED
#define SPRINGLOBBY_SORTUTIL_H_INCLUDED

//! reference sort algo implementation, not in production use
template< class ContainerType, class Comparator >
void SLBubbleSort( ContainerType& data, const Comparator& cmp )
{
    const int n = data.size();
    int j = 0;
    bool swapped = true;
    while ( swapped )
    {
        j++;
        swapped = false;
        for ( int i = 0; i < n - j; ++i )
        {
            if ( cmp ( data[i], data[i+1] ) )
            {
                typename Comparator::ObjType tmp = data[i+1];
                data[i+1] = data[i];
                data[i] = tmp;
                swapped = true;
            }
        }
    }
}

 //! set direction to +1 for down, -1 for up
struct SortOrderItem {
    int col;
    int direction;
};
//! map sort priority <--> ( column, direction )
typedef std::map<int,SortOrderItem> SortOrder;


//! the sort algo used in almost all ListCtrls
template< class ContainerType, class Comparator >
void SLInsertionSort( ContainerType& data, const Comparator& cmp )
{
    const int n = data.size();
    for ( int i = 0; i < n; i++ )
    {
        typename Comparator::ObjType v = data[i];
        int j;

        for ( j = i - 1; j >= 0; j--)
        {
            if ( cmp( data[j], v ) )
                break;
            data[j + 1] = data[j];
        }
        data[j + 1] = v;
    }
}

namespace SlQuickSort {

template< class ContainerType, class Comparator >
size_t partition(ContainerType& a, size_t p, size_t r, const Comparator& cmp) {
  typename Comparator::ObjType x = a[r];
  size_t j = p - 1;
  for (size_t i = p; i < r; i++) {

    if ( !cmp(x , a[i]) ) {
      j = j + 1;
      typename Comparator::ObjType temp = a[j];
      a[j] = a[i];
      a[i] = temp;
    }
  }
  a[r] = a[j + 1];
  a[j + 1] = x;

  return (j + 1);
}

template< class ContainerType, class Comparator >
void quickSort(ContainerType& a, size_t p, size_t r, const Comparator& cmp ) {
    if (p < r) {
        size_t q = partition(a, p, r, cmp);
        quickSort(a, p, q - 1, cmp);
        quickSort(a, q + 1, r, cmp);
    }
}

template< class ContainerType, class Comparator >
void Sort(ContainerType& a, const Comparator& cmp ) {
    quickSort( a, 0, a.size() - 1, cmp );
}

} //end namespace Sl_Quicksort

#endif // SPRINGLOBBY_SORTUTIL_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

