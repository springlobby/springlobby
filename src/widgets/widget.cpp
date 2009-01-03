#include "widget.h"
#include <wx/arrstr.h>

Widget::Widget()
{
    //ctor
}

Widget::~Widget()
{
    //dtor
}

bool Widget::IsInstalled()
{

}

wxArrayString Widget::GetImageFilenames()
{
    wxArrayString ret;
    ExtendedInfo::Images::const_iterator it = extendedinfo.images.begin();
    for ( ; it != extendedinfo.images.end(); ++it ) {
        ret.Add( it->local_path );
    }
    return ret;
}
