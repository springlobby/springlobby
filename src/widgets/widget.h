#ifndef SPRINGLOBBY_WIDGET_H_INCLUDED
#define SPRINGLOBBY_WIDGET_H_INCLUDED

#include <wx/string.h>

struct Widget
{
    public:
        Widget();
        virtual ~Widget();
    long w_id;
    long n_id;
    wxString name;
    wxString description;
    wxString short_description;
    long num_downloads;
    wxString mods;
    wxString author;
    long num_images;
    long rev_major;
    long rev_minor;
    wxString date;
    wxString changelog;

};

#endif // SPRINGLOBBY_WIDGET_H_INCLUDED
