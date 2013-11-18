#include "imageviewer.h"

#include <stdexcept>
//#include <wx/intl.h>
//#include <wx/imaglist.h>
#include <wx/bitmap.h>
#include <wx/sizer.h>
//#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/image.h>
#include <wx/scrolwin.h>
#include <wx/dcclient.h>
#include <wx/button.h>
#include <wx/filedlg.h>

#include "../utils/customdialogs.h"
#include "../uiutils.h"

BEGIN_EVENT_TABLE( ImagePanel, wxPanel )
    EVT_PAINT(ImagePanel::OnPaint)
    EVT_SIZE( ImagePanel::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE( ImageViewerPanel, wxPanel )
    EVT_BUTTON( ImageViewerPanel::ID_NEXT, ImageViewerPanel::OnNext )
    EVT_BUTTON( ImageViewerPanel::ID_DELETE, ImageViewerPanel::OnDelete )
    EVT_BUTTON( ImageViewerPanel::ID_PREV, ImageViewerPanel::OnPrev )
    EVT_BUTTON( ImageViewerPanel::ID_SAVE_AS, ImageViewerPanel::OnSaveAs )
END_EVENT_TABLE()

ImagePanel::ImagePanel( const wxString& file, wxWindow* parent, wxWindowID id )
    : wxPanel( parent, id ),
    m_file( file )
{}

ImagePanel::ImagePanel( wxWindow* parent, wxWindowID id )
    : wxPanel( parent, id )
{}

ImagePanel::~ImagePanel()
{}

void ImagePanel::SetBitmap( const wxString& file )
{
    m_file = file;
    wxPaintEvent p;
    OnPaint( p );
}

void ImagePanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc( this );
    dc.Clear();

    wxImage im;
    if ( !im.LoadFile( m_file ) )
        return;

    wxSize c_sz = GetClientSize();
    wxSize im_sz ( im.GetWidth(), im.GetHeight() );
    im_sz = MakeFit( im_sz, c_sz );

    dc.DrawBitmap( wxBitmap(im.Rescale( im_sz.GetWidth(), im_sz.GetHeight() ) ), 0, 0, true /* use mask */ );
}
void ImagePanel::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxPaintEvent p;
    OnPaint( p );
}

ImageViewerPanel::ImageViewerPanel(const wxArrayString& filenames, bool enable_delete, wxWindow* parent,
        wxWindowID id, long /*style*/ )
    : wxPanel ( parent, id, wxDefaultPosition, wxDefaultSize ),
    m_filenames( filenames ),
    m_current_file_index( 0 ),
    m_num_files( filenames.Count() ),
    m_enable_delete( enable_delete ),
    m_delete( 0 )
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_button_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new ImagePanel( this, -1 );
    m_main_sizer->Add( m_panel, 1, wxEXPAND | wxALL, 0 );

    m_prev = new wxButton( this, ID_PREV, _("previous") );
    m_button_sizer->Add( m_prev, 0, wxALL, 5 );

    m_next = new wxButton( this, ID_NEXT, _("next") );
    m_button_sizer->Add( m_next, 0, wxALL, 5 );

    if ( m_enable_delete ) {
        m_delete = new wxButton( this, ID_DELETE, _("delete") );
        m_button_sizer->Add( m_delete, 0, wxALL, 5 );
    }

    m_save_as = new wxButton( this, ID_SAVE_AS, _("save as") );
    m_button_sizer->Add( m_save_as, 0, wxALL, 5 );

    m_main_sizer->Add( m_button_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );
    SetSizer( m_main_sizer );

    SetButtonStates();
    SetImage();

    Layout();
    m_panel->Refresh();
}

ImageViewerPanel::~ImageViewerPanel()
{
}

void ImageViewerPanel::SetButtonStates()
{
    m_next->Enable( m_current_file_index < m_num_files -1 );
    m_prev->Enable( m_current_file_index > 0 );
    if ( m_delete )
        m_delete->Enable( m_num_files > 0 );
    m_save_as->Enable( m_num_files > 0 );
}

void ImageViewerPanel::SetImage()
{
    SetButtonStates();
    m_panel->SetBitmap( m_filenames[m_current_file_index] );
//    SetTitle( m_filenames[m_current_file_index] );
}

void ImageViewerPanel::OnNext( wxCommandEvent& /*evt*/ )
{
    m_current_file_index++;
    SetImage();
}

void ImageViewerPanel::OnDelete( wxCommandEvent& /*evt*/ )
{
    wxString file = m_filenames[m_current_file_index];
    if ( wxRemoveFile( file ) ) {
        m_filenames.RemoveAt( m_current_file_index );
        m_num_files--;
        if ( m_current_file_index > m_num_files -1 )
            m_current_file_index--;
        SetImage();
    }
    else {
        customMessageBoxNoModal( SL_MAIN_ICON, _("couldn't remove file"), _("Error") );
    }
}

void ImageViewerPanel::OnPrev( wxCommandEvent& /*evt*/ )
{
    m_current_file_index--;
    SetImage();
}

void ImageViewerPanel::OnSaveAs( wxCommandEvent& /*evt*/ )
{
    wxString ext = m_filenames[m_current_file_index].AfterLast( '.' );
    wxString mask = _T("*.") + ext;
    wxFileDialog fd( this, _("Choose a filename"), wxEmptyString, wxEmptyString, mask, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
    if ( fd.ShowModal() == wxID_OK ) {
        wxString new_file = fd.GetPath();
        if ( !new_file.EndsWith( ext ) )
            new_file += _T(".") + ext;

        if ( wxCopyFile( m_filenames[m_current_file_index], new_file ) ) {
            customMessageBoxNoModal( SL_MAIN_ICON, _("File successfully saved"), _("Success") );
        }
        else {
            customMessageBoxNoModal( SL_MAIN_ICON, _("Couldn't save file"), _("Error") );
        }
    }
}

ImageViewerDialog::ImageViewerDialog(const wxArrayString& filenames, bool enable_delete, wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style)
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    long p_style = 0;
    m_imageviewer_panel = new ImageViewerPanel( filenames, enable_delete, this, -1,p_style );
    m_main_sizer->Add( m_imageviewer_panel, 1, wxEXPAND|wxALIGN_CENTER, 0 );
    SetSizer( m_main_sizer );

}
