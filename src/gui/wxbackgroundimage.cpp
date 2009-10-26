/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

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
                wxBufferedPaintDC DC(TempWindow);
                int w, h;
                TempWindow->GetClientSize(&w, &h);
                wxImage TempImage = Bitmap.ConvertToImage();
                TempImage.Rescale(w,h);
                DC.DrawBitmap(wxBitmap(TempImage), 0, 0, false);
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
        return Inherited::ProcessEvent(Event);
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
