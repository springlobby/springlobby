#ifndef JPEGHANDLER_H
#define JPEGHANDLER_H

#include <wx/defs.h>

//-----------------------------------------------------------------------------
// SL_JPEGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG

#include <wx/image.h>

class SL_JPEGHandler: public wxImageHandler
{
public:
    inline SL_JPEGHandler()
    {
        m_name = wxT("JPEG file");
        m_extension = wxT("jpg");
        m_type = wxBITMAP_TYPE_JPEG;
        m_mime = wxT("image/jpeg");
    }

    static SL_JPEGHandler& GetInstance() { static SL_JPEGHandler m_handler; return m_handler; }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );
protected:
    virtual bool DoCanRead( wxInputStream& stream );
#endif

private:
    DECLARE_DYNAMIC_CLASS(SL_JPEGHandler)
};




#endif // wxUSE_LIBJPEG
#endif // JPEGHANDLER_H
