#ifndef SL_HEADERGUARD_TEXTHISTCONROL_H
#define SL_HEADERGUARD_TEXTHISTCONROL_H

#include <wx/textctrl.h>

//copied from http://wxforum.shadonet.com/viewtopic.php?t=7924 (thanks go to Lamego)
//modified some (textcompletion,etc)

// Extends wxTextCtrl with input historical functions
// to allow user to navigate to previously entered messages
class TextCompletionDatabase;

class wxTextCtrlHist : public wxTextCtrl
{
  public:
    wxTextCtrlHist(TextCompletionDatabase& textcompletiondatabase, wxWindow* parent, wxWindowID id, const wxString& value = _T(""),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    void SetHistoryMax(int max) { history_max = max; }

protected:
#if wxHAS_TEXT_WINDOW_STREAM
    int overflow(int i);
#endif // wxHAS_TEXT_WINDOW_STREAM

  private:
    TextCompletionDatabase& textcompletiondatabase;
    wxString m_original;
    int current_pos;
    int history_max;
    wxArrayString Historical;
    void OnSendMessage( wxCommandEvent &event );
    void OnChar(wxKeyEvent & event);
    DECLARE_EVENT_TABLE()
};

#endif //SL_HEADERGUARD_TEXTHISTCONROL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

