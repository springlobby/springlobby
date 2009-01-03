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

#include "jpeghandler.h"

BEGIN_EVENT_TABLE( ImagePanel, wxPanel )
    EVT_PAINT(ImagePanel::OnPaint)
    EVT_SIZE( ImagePanel::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE( ImageViewer, wxDialog )
    EVT_BUTTON( ImageViewer::ID_NEXT, ImageViewer::OnNext )
    EVT_BUTTON( ImageViewer::ID_DELETE, ImageViewer::OnDelete )
    EVT_BUTTON( ImageViewer::ID_PREV, ImageViewer::OnPrev )
END_EVENT_TABLE()

ImagePanel::ImagePanel( const wxString& file, wxWindow* parent, wxWindowID id )
    : wxPanel( parent, id ),
    m_file( file )
{
//    m_jpeg_handler = new SL_JPEGHandler();
//    wxString old_name = (new wxJPEGHandler)->GetName();
//    wxImage::RemoveHandler( old_name );
//    wxImage::AddHandler(m_jpeg_handler);


}

ImagePanel::ImagePanel( wxWindow* parent, wxWindowID id )
    : wxPanel( parent, id )
{}

ImagePanel::~ImagePanel()
{
    //wxImage::AddHandler( new wxJPEGHandler );
}

void ImagePanel::SetBitmap( const wxString& file )
{
    m_file = file;
    wxPaintEvent p;
    OnPaint( p );
}

void ImagePanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc( this );
    //m_jpeg_handler->m_parentSize = this->GetClientSize();
    wxImage im ( m_file );

    dc.DrawBitmap( wxBitmap(im.Rescale( GetClientSize().GetX(), GetClientSize().GetY() ) ), 0, 0, true /* use mask */ );

}
void ImagePanel::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxPaintEvent p;
    OnPaint( p );
}

ImageViewer::ImageViewer(const wxArrayString& filenames, wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style),
    m_filenames( filenames ),
    m_current_file_index( 0 ),
    m_num_files( filenames.Count() )
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_button_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new ImagePanel( this, -1 );
    m_main_sizer->Add( m_panel, 1, wxEXPAND | wxALL, 0 );

    m_next = new wxButton( this, ID_NEXT, _("next") );
    m_button_sizer->Add( m_next, 0, wxALL, 5 );
    m_prev = new wxButton( this, ID_PREV, _("previous") );
    m_button_sizer->Add( m_prev, 0, wxALL, 5 );
    m_main_sizer->Add( m_button_sizer, 0, wxALL, 0 );
    SetSizer( m_main_sizer );
    SetImage();
    Layout();
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::SetButtonStates()
{
    m_next->Enable( m_current_file_index < m_num_files -1 );
    m_prev->Enable( m_current_file_index > 0 );
}

void ImageViewer::SetImage()
{
    SetButtonStates();
    m_panel->SetBitmap( m_filenames[m_current_file_index] );
    SetTitle( m_filenames[m_current_file_index] );
}

void ImageViewer::OnNext( wxCommandEvent& evt )
{
    m_current_file_index++;
    SetImage();
}

void ImageViewer::OnDelete( wxCommandEvent& evt )
{

}

void ImageViewer::OnPrev( wxCommandEvent& evt )
{
    m_current_file_index--;
    SetImage();
}
