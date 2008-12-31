#include "imageviewer.h"

ImageViewer::ImageViewer(const wxArrayString& filenames, wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style),
    m_filenames( filenames ),
    m_num_files( filenames.Count() )
{
    //ctor
}

ImageViewer::~ImageViewer()
{
    //dtor
}
