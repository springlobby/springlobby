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

//    template <>
//    inline static int compareSimple( const wxString& o1, const wxString& o2 ) {
//        if ( o1 < o2 )
//            return -1;
//        else if ( o1 > o2 )
//            return 1;
//        return 0;
//    }
};

template < class CompareImp >
struct CompareInterface {

    typedef CompareImp ImpType;

    template < class ObjType >
    bool operator() ( ObjType o1, ObjType o2 ) {
        return asImp()( o1,o2 );
    }

    inline ImpType& asImp(){
        return static_cast<ImpType>(*this);
    }

};

template < template <int n, bool b > class Comparator ,int C0, bool B0,int C1, bool B1,int C2, bool B2 >
struct Compare :
    public CompareInterface < Compare <Comparator ,C0, B0,C1, B1,C2, B2 > >
{
    typedef bool test;
    typedef typename Comparator<-1,false>::CompareType ObjType;
    static bool compare ( ObjType u1, ObjType u2 ) {
        assert( u1 && u2 );
        Comparator<C0,B0> c1;
        int res = c1.compare( u1, u2 );
        if ( res != 0 )
            return res < 0;

        Comparator<C1,B1> c2;
        res = c2.compare( u1, u2 );
        if ( res != 0 )
            return res < 0;

        Comparator<C2,B2> c3;
        res = c3.compare( u1, u2 );
        if ( res != 0 )
            return res < 0;

        return false;

    }

    bool operator () ( ObjType u1, ObjType u2 ) const {
        assert( u1 && u2 );
        Comparator<C0,B0> c1;
        int res = c1.compare( u1, u2 );
        if ( res != 0 )
            return res < 0;

        Comparator<C1,B1> c2;
        res = c2.compare( u1, u2 );
        if ( res != 0 )
            return res < 0;

        Comparator<C2,B2> c3;
        res = c3.compare( u1, u2 );
        if ( res != 0 )
            return res < 0;

        return false;
    }
};

template < template <int n, bool b > class Comparator  >
struct CompareSelector {

    typedef typename Comparator<-1,false>::CompareType ObjType;
    typedef bool  (*cmp)  ( ObjType , ObjType  )  ;

    static cmp GetFunctor( int c1, bool b1,int c2, bool b2,int c3, bool b3 )
    {

//        return  &(Compare< Comparator, 1, false, 0, true, 0, true  >::compare);
        return  &(Compare< Comparator, 1, true, 2, true, 3, true  >::compare);
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
void SLInsertionSort( ContainerType& data, bool  (*cmp)  ( ObjType , ObjType  ) )
{
    const int n = data.size();
    for ( int i = 0; i < n; i++ )
    {
        ObjType v = data[i];
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

template< class ContainerType, class Comparator >
void SLInsertionSort( ContainerType& data, Comparator& cmp )
{
    typedef typename Comparator::ObjType ObjType;

    const int n = data.size();
    for ( int i = 0; i < n; i++ )
    {
        ObjType v = data[i];
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
