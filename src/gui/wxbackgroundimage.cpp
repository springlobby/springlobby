/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "wxbackgroundimage.h"

#include <wx/dc.h>
#include <wx/window.h>
#include <wx/dcbuffer.h>
#include <wx/image.h>

#if 1

bool wxBackgroundBitmap::ProcessEvent(wxEvent &Event)
{
    if(Event.GetEventType() == wxEVT_ERASE_BACKGROUND)
    {
        if(Bitmap.IsOk())
        {
        }
        else
            Event.Skip();
    }
    else if(Event.GetEventType() == wxEVT_PAINT)
    {

        bool TransactionIsOk = false;
        if(Bitmap.IsOk())
        {
            wxWindow * TempWindow = wxDynamicCast(Event.GetEventObject(),wxWindow);
            if(TempWindow)
            {
                wxAutoBufferedPaintDC DC(TempWindow);
                int w, h;
                TempWindow->GetClientSize(&w, &h);
				wxSize current( w,h);
				if ( current != m_lastSize )
				{
					wxImage TempImage = Bitmap.ConvertToImage();
					TempImage.Rescale(w,h);
					Bitmap = wxBitmap( TempImage );
				}
				DC.DrawBitmap(Bitmap, 0, 0, false);
				m_lastSize = current;
                TransactionIsOk = true;
            }
        }
        if(TransactionIsOk == false)
            Event.Skip();
    }
    else if(Event.GetEventType() ==  wxEVT_SIZE)
    {
        wxWindow * TempWindow = wxDynamicCast(Event.GetEventObject(),wxWindow);
        if(TempWindow)
        {
            TempWindow->Refresh();
        }
        Event.Skip();
    }
    else
        return wxEvtHandler::ProcessEvent(Event);
    return true;
}
#else
bool wxBackgroundBitmap::ProcessEvent(wxEvent &Event)
{
    if (Event.GetEventType() == wxEVT_ERASE_BACKGROUND) {
        wxEraseEvent &EraseEvent = dynamic_cast<wxEraseEvent &>(Event);
        wxDC *DC = EraseEvent.GetDC();
        DC->DrawBitmap(Bitmap, 0, 0, false);
        return true;
    } else return Inherited::ProcessEvent(Event);
}

#endif

