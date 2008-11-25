#ifndef SPRINGLOBBY_SORTUTIL_H_INCLUDED
#define SPRINGLOBBY_SORTUTIL_H_INCLUDED

template < class T>
struct CompareBase {
    typedef T CompareType;

    template < typename Type >
    inline static int compareSimple( Type o1, Type o2 ) {
        if ( o1 < o2 )
            return -1;
        else if ( o1 > o2 )
            return 1;
        return 0;
    }
};

template < class CompareImp >
struct CompareInterface {

    typedef CompareImp ImpType;

    inline ImpType& asImp(){
        return static_cast<ImpType>(*this);
    }

};

template < template <int n > class Comparator, int C0, int C1, int C2 >
struct Compare :
    public CompareInterface < Compare <Comparator ,C0, C1, C2 > >
{
    typedef bool test;
    typedef typename Comparator<-1>::CompareType ObjType;
    static bool compare ( ObjType u1, ObjType u2, int dir1, int dir2, int dir3 ) {
        assert( u1 && u2 );

        int res = Comparator<C0>::compare( u1, u2, dir1 );
        if ( res != 0 )
            return res < 0;

        res = Comparator<C1>::compare( u1, u2, dir1 );
        if ( res != 0 )
            return res < 0;

        res = Comparator<C2>::compare( u1, u2, dir1 );
        if ( res != 0 )
            return res < 0;

        return false;

    }
};

template < template <int n > class Comparator  >
struct CompareSelector {

    typedef typename Comparator<-1>::CompareType ObjType;
    typedef bool  (*cmp)  ( ObjType , ObjType, int, int, int  )  ;

    static cmp GetFunctor( int c1, int c2, int c3 )
    {

        #define CASE(i) case i: return GetFunctor<i>(c1,c2);
        switch ( c3 ) {
            CASE(1)
            CASE(2)
            CASE(3)
            CASE(4)
            CASE(5)
            CASE(7)
            CASE(8)
            CASE(9)
            CASE(10)
            CASE(11)
            CASE(12)
            CASE(13)
            CASE(14)
            CASE(15)
        }
        #undef CASE
        return  &(Compare< Comparator, 1, 2, 3 >::compare);

    }

    template < int C3 >
    static cmp GetFunctor( int c1, int c2 )
    {
        #define CASE(i) case i: return GetFunctor<i,C3>(c1);
        switch ( c2 ) {
            CASE(1)
            CASE(2)
            CASE(3)
            CASE(4)
            CASE(5)
            CASE(7)
            CASE(8)
            CASE(9)
            CASE(10)
            CASE(11)
            CASE(12)
            CASE(13)
            CASE(14)
            CASE(15)
        }
        #undef CASE
        return  &(Compare< Comparator, 1, 2, 3 >::compare);
    }
    template < int C2, int C3 >
    static cmp GetFunctor( int c1 )
    {
        #define CASE(i) case i: return &(Compare<Comparator,i,C2,C3>::compare);
        switch ( c1 ) {
            CASE(1)
            CASE(2)
            CASE(3)
            CASE(4)
            CASE(5)
            CASE(7)
            CASE(8)
            CASE(9)
            CASE(10)
            CASE(11)
            CASE(12)
            CASE(13)
            CASE(14)
            CASE(15)
        }
        #undef CASE
        return  &(Compare< Comparator, 1, 2, 3 >::compare);
    }


};

template< class ContainerType, class ObjType >
void SLBubbleSort( ContainerType& data, bool  (*cmp)  ( ObjType , ObjType  ) )
{
    const int n = data.size();
    int j = 0;
    bool swapped = true;
    while ( swapped )
    {
        j++;
        unsigned int swaps = 0;
        for ( int i = 0; i < n - j; ++i )
        {
            if ( cmp ( data[i], data[i+1] ) )
            {
                ObjType tmp = data[i+1];
                data[i+1] = data[i];
                data[i] = tmp;
                swapped = true;
            }
        }
    }
}

template< class ContainerType, class ObjType >
void SLInsertionSort( ContainerType& data, bool  (*cmp)  ( ObjType , ObjType, int, int, int  ), int dir1, int dir2, int dir3  )
{
    const int n = data.size();
    for ( int i = 0; i < n; i++ )
    {
        ObjType v = data[i];
        int j;

        for ( j = i - 1; j >= 0; j--)
        {
            if ( cmp( data[j], v, dir1, dir2, dir3  ) )
                break;
            data[j + 1] = data[j];
        }
        data[j + 1] = v;
    }

}

#endif // SPRINGLOBBY_SORTUTIL_H_INCLUDED
