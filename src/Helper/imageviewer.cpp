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

#include "jpeghandler.h"

BEGIN_EVENT_TABLE( ImagePanel, wxPanel )
    EVT_PAINT(ImagePanel::OnPaint)
    EVT_SIZE( ImagePanel::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE( ImageViewer, wxDialog )
    //EVT_PAINT(ImageViewer::OnPaint)
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

ImagePanel::~ImagePanel()
{
    wxImage::AddHandler( new wxJPEGHandler );
}

void ImagePanel::SetBitmap( const wxString& file )
{
    m_file = file;
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
    m_num_files( filenames.Count() )
{
    m_panel = new ImagePanel( m_filenames[0], this, -1 );
}

ImageViewer::~ImageViewer()
{
}

