#ifndef SPRINGLOBBY_WIDGET_H_INCLUDED
#define SPRINGLOBBY_WIDGET_H_INCLUDED

#include <wx/string.h>

struct Widget
{
    public:
        Widget();
        virtual ~Widget();
    int id;
    wxString name;
    wxString description;
    unsigned int num_downloads;
    wxString mods;
    wxString author;
    unsigned int num_images;
    unsigned int rev_major;
    unsigned int rev_minor;
    wxString date;
    wxString changelog;

};

#endif // SPRINGLOBBY_WIDGET_H_INCLUDED
