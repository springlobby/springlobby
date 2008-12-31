#ifndef SPRINGLOBBY_IMAGEVIEWER_H_INCLUDED
#define SPRINGLOBBY_IMAGEVIEWER_H_INCLUDED

#include <wx/dialog.h>


class ImageViewer : public wxDialog
{
    public:
        ImageViewer(const wxArrayString& filenames, wxWindow* parent, wxWindowID id, const wxString& title,
            long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE );
        virtual ~ImageViewer();

    protected:
        const wxArrayString& m_filenames;
        unsigned int m_current_file_index;
        const unsigned int m_num_files;
};

#endif // SPRINGLOBBY_IMAGEVIEWER_H_INCLUDED
