/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_IMAGEVIEWER_H_INCLUDED
#define SPRINGLOBBY_IMAGEVIEWER_H_INCLUDED

#include <wx/dialog.h>
#include <wx/panel.h>

class wxPaintEvent;
class SL_JPEGHandler;//this would be a custom jpeg handler with better resizing implemented
class wxPaintDC;

//! panel displaying bitmap loaded from file
class ImagePanel : public wxPanel
{
    public:
        ImagePanel( const wxString& file, wxWindow* parent, wxWindowID id );
        ImagePanel( wxWindow* parent, wxWindowID id );
        virtual ~ImagePanel();
        void SetBitmap( const wxString& file );

    protected:
        wxString m_file;
//        SL_JPEGHandler* m_jpeg_handler;

        void OnPaint(wxPaintEvent& WXUNUSED(event));
        void OnSize(wxSizeEvent& WXUNUSED(event));

        DECLARE_EVENT_TABLE()
};

class wxBoxSizer;
class wxButton;

//! panel containing a ImagePanel plus navigation buttons to view a list of filenames
class ImageViewerPanel : public wxPanel
{
    public:
        ImageViewerPanel(const wxArrayString& filenames, bool enable_delete, wxWindow* parent, wxWindowID id, long style );
        virtual ~ImageViewerPanel();

    protected:
        wxArrayString m_filenames;
        unsigned int m_current_file_index;
        unsigned int m_num_files;
        bool m_enable_delete;

        wxBoxSizer* m_main_sizer;
        wxBoxSizer* m_button_sizer;
        wxButton* m_next;
        wxButton* m_prev;
        wxButton* m_delete;
        wxButton* m_save_as;
        ImagePanel* m_panel;

        void OnNext( wxCommandEvent& evt );
        void OnDelete( wxCommandEvent& evt );
        void OnPrev( wxCommandEvent& evt );
        void OnSaveAs( wxCommandEvent& evt );

        void SetButtonStates();
        void SetImage();

        enum {
            ID_DELETE,
            ID_NEXT,
            ID_PREV,
            ID_SAVE_AS
        };

        DECLARE_EVENT_TABLE()
};

//! dialog used as container for ImageViewerPanel, eg for screenshots filenames retrieved via Unitsysnc
class ImageViewerDialog : public wxDialog
{
    public:
        ImageViewerDialog(const wxArrayString& filenames, bool enable_delete, wxWindow* parent, wxWindowID id, const wxString& title = wxEmptyString,
            long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE );
        virtual ~ImageViewerDialog(){}

    protected:
        ImageViewerPanel* m_imageviewer_panel;
        wxBoxSizer* m_main_sizer;
};

#endif // SPRINGLOBBY_IMAGEVIEWER_H_INCLUDED
