#ifndef SPRINGLOBBY_SORTUTIL_H_INCLUDED
#define SPRINGLOBBY_SORTUTIL_H_INCLUDED


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

#endif // SPRINGLOBBY_SORTUTIL_H_INCLUDED
