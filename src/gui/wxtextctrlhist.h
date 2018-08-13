/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

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
	wxTextCtrlHist(TextCompletionDatabase& textcompletiondatabase, wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
		       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	void SetHistoryMax(int max)
	{
		history_max = max;
	}

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
	void OnSendMessage(wxCommandEvent& event);
	void OnChar(wxKeyEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif //SL_HEADERGUARD_TEXTHISTCONROL_H
