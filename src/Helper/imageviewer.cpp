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

BEGIN_EVENT_TABLE( ImageViewer, wxDialog )
    EVT_PAINT(ImageViewer::OnPaint)
END_EVENT_TABLE()

ImageViewer::ImageViewer(const wxArrayString& filenames, wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style),
    m_filenames( filenames ),
    m_num_files( filenames.Count() )
{
    m_jpeg_handler = new SL_JPEGHandler();
    wxString old_name = (new wxJPEGHandler)->GetName();
    wxImage::RemoveHandler( old_name );
    wxImage::AddHandler(m_jpeg_handler);

}

ImageViewer::~ImageViewer()
{
    wxImage::AddHandler( new wxJPEGHandler );
}

void ImageViewer::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxImage test(_T("/share/springdata/screenshots/screen000.jpg" ));
    wxPaintDC dc( this );
    dc.DrawBitmap( wxBitmap(test), 0, 0, true /* use mask */ );
}
