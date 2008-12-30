#ifndef SPRINGLOBBY_WIDGET_H_INCLUDED
#define SPRINGLOBBY_WIDGET_H_INCLUDED

#include <wx/string.h>
#include <vector>

struct WidgetFile
{
    wxString url;
    wxString local_path;
    wxString md5;
    long id;
};

struct WidgetImage
{
    wxString url;
    long id;
};

struct ExtendedInfo
{
    ExtendedInfo():parsed(false) {}
    bool parsed;

    typedef std::vector< WidgetFile > Files;
    Files files;

    typedef std::vector< WidgetImage > Images;
    Images images;
};

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
    ExtendedInfo extendedinfo;
};

#endif // SPRINGLOBBY_WIDGET_H_INCLUDED
