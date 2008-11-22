#ifndef SPRINGLOBBY_SORTUTIL_H_INCLUDED
#define SPRINGLOBBY_SORTUTIL_H_INCLUDED

template < class T>
struct CompareBase {
    typedef T CompareType;
};

template < class CompareImp >
struct CompareInterface {

    typedef CompareImp ImpType;

    template < class ObjType >
    bool operator() ( ObjType o1, ObjType o2 ) {
        return asImp()( o1,o2 );
    }

    ImpType& asImp(){
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
        if ( !Comparator<C0,B0>::compare( u1, u2 ) ) {
            if ( !Comparator<C1,B1>::compare( u1, u2 ) ) {
                if ( !Comparator<C2,B2>::compare( u1, u2 ) ) {
                    return false;
                }
                return true;
            }
            return true;
        }
        return true;
    }

    bool operator () ( ObjType u1, ObjType u2 ) const {
        if ( !Comparator<C0,B0>::compare( u1, u2 ) ) {
            if ( !Comparator<C1,B1>::compare( u1, u2 ) ) {
                if ( !Comparator<C2,B2>::compare( u1, u2 ) ) {
                    return false;
                }
                return true;
            }
            return true;
        }
        return true;
    }
};

template < template <int n, bool b > class Comparator  >
struct CompareSelector {

    typedef typename Comparator<-1,false>::CompareType ObjType;
    typedef bool  (*cmp)  ( ObjType , ObjType  )  ;

    static cmp GetFunctor( int c1, bool b1,int c2, bool b2,int c3, bool b3 )
    {

//        return  &(Compare< Comparator, 1, false, 0, true, 0, true  >::compare);
        return  &(Compare< Comparator, 3, false, 1, true, 1, true  >::compare);
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
