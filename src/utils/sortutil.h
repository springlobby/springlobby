/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_SORTUTIL_H_INCLUDED
#define SPRINGLOBBY_SORTUTIL_H_INCLUDED

#include <map>

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
typename ContainerType::size_t partition(ContainerType& a, typename ContainerType::size_t p,
                                         typename ContainerType::size_t r, const Comparator& cmp) {
  typename Comparator::ObjType x = a[r];
  typename ContainerType::size_t j = p - 1;
  for (typename ContainerType::size_t i = p; i < r; i++) {

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
void quickSort(ContainerType& a, typename ContainerType::size_t p,
               typename ContainerType::size_t r, const Comparator& cmp ) {
    if (p < r) {
        typename ContainerType::size_t q = partition(a, p, r, cmp);
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

