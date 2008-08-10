// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxtextctrlhist.h"

BEGIN_EVENT_TABLE(wxTextCtrlHist, wxTextCtrl)
    EVT_TEXT_ENTER(wxID_ANY, wxTextCtrlHist::OnSendMessage)
    //EVT_KEY_DOWN(wxTextCtrlHist::OnChar)
END_EVENT_TABLE()


wxTextCtrlHist::wxTextCtrlHist(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style )
    : wxTextCtrl(parent, id, value, pos, size, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB )
    , current_pos(0)
    , history_max(32)
{
}

void wxTextCtrlHist::OnSendMessage( wxCommandEvent &event )
{

  Historical.Add(GetLineText(0));
        current_pos = Historical.GetCount();
        if(current_pos > history_max)
        {
                Historical.RemoveAt(0);
                --current_pos;
        }
  event.Skip();
}

void wxTextCtrlHist::OnChar(wxKeyEvent & event)
{

        if(event.m_keyCode == WXK_UP)
        {
                if(current_pos > 0)
                {
                        --current_pos;
                        SetValue(Historical[current_pos]);
                        SetInsertionPointEnd();
                }
        }
        else
        if(event.m_keyCode == WXK_DOWN)
        {
                if(current_pos < static_cast<int>(Historical.GetCount())-1)
                {
                    ++current_pos;
                    SetValue(Historical[current_pos]);
                }
                else
                {
                    current_pos = Historical.GetCount();
                    SetValue(wxT(""));
                    SetInsertionPointEnd();
                }
        }
  event.Skip();

}
