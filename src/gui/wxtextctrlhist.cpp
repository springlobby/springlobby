/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "wxtextctrlhist.h"

#include <wx/regex.h>

#include "gui/mainchattab.h"
#include "gui/mainwindow.h"
#include "gui/ui.h"
#include "settings.h"
#include "utils/misc.h"
#include "utils/TextCompletionDatabase.h"

BEGIN_EVENT_TABLE(wxTextCtrlHist, wxTextCtrl)
EVT_TEXT_ENTER(wxID_ANY, wxTextCtrlHist::OnSendMessage)
EVT_KEY_DOWN(wxTextCtrlHist::OnChar)
END_EVENT_TABLE()

void GetArrayStringFromHashMap(const HashMap_String_String& hm, wxArrayString& matches)
{
	for (HashMap_String_String::const_iterator it = hm.begin(); it != hm.end(); ++it)
		matches.Add(it->second);
}

wxTextCtrlHist::wxTextCtrlHist(TextCompletionDatabase& textDb, wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long /*unused*/)
    : wxTextCtrl(parent, id, value, pos, size, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB)
    , textcompletiondatabase(textDb)
    , current_pos(0)
    , history_max(32)
{
}

void wxTextCtrlHist::OnSendMessage(wxCommandEvent& event)
{
	const wxString msg = GetLineText(0);
	if (msg != wxEmptyString) {
		Historical.Add(msg);
		current_pos = Historical.GetCount();

		if (current_pos > history_max) {
			Historical.RemoveAt(0);
			--current_pos;
		}
	}
	event.Skip();
}

void wxTextCtrlHist::OnChar(wxKeyEvent& event)
{
	const int keyCode = event.GetKeyCode();
	const int modifier = event.GetModifiers();

	if (current_pos == int(Historical.GetCount())) {
		m_original = GetValue();
	}

	if (keyCode == WXK_UP) {
		if (current_pos > 0) {
			--current_pos;
			SetValue(Historical[current_pos]);
			SetInsertionPointEnd();
		}
		return;
	}

	if (keyCode == WXK_DOWN) {
		if (current_pos < static_cast<int>(Historical.GetCount()) - 1) {
			++current_pos;
			SetValue(Historical[current_pos]);
		} else {
			current_pos = Historical.GetCount();
			SetValue(m_original);
			SetInsertionPointEnd();
		}
		return;
	}

	if (keyCode == WXK_TAB) {
		if ((modifier & wxMOD_CONTROL) != 0) {
			ui().mw().GetChatTab().AdvanceSelection((modifier & wxMOD_SHIFT) == 0);
		} else {
			wxString text = this->GetValue();
			long pos_Cursor = this->GetInsertionPoint();
			wxString selection_Begin_InsertPos = this->GetRange(0, pos_Cursor);
			wxString selection_InsertPos_End = this->GetRange(pos_Cursor, this->GetLastPosition());

			// Search for the shortest Match, starting from the Insertionpoint to the left, until we find a "\ "
			// Special Characters according to regular Expression Syntax needs to be escaped: [,]
			wxRegEx regex_currentWord;
#ifdef wxHAS_REGEX_ADVANCED
			regex_currentWord.Compile(wxT("(_|\\[|\\]|\\w)+$"), wxRE_ADVANCED);
#else
			regex_currentWord.Compile(wxT("(_|\\[|\\]|\\w)+$"), wxRE_EXTENDED);
#endif

			if (regex_currentWord.Matches(selection_Begin_InsertPos)) {
				wxString currentWord = regex_currentWord.GetMatch(selection_Begin_InsertPos);
				// std::cout << "#########: Current Word: (" << currentWord.char_str() << ")" << std::endl;

				wxString selection_Begin_BeforeCurrentWord = this->GetRange(0, pos_Cursor - currentWord.length());
				// std::cout << "#########: selection_Begin_BeforeCurrentWord: (" << selection_Begin_BeforeCurrentWord.char_str() << ")" << std::endl;

				HashMap_String_String hm = textcompletiondatabase.GetMapping(currentWord);

				// std::cout << "#########: Mapping-Size: (" << hm.size() << ")" << std::endl;

				wxString completed_Text;
				int new_Cursor_Pos = 0;
				if (hm.size() == 1) {
					completed_Text.append(selection_Begin_BeforeCurrentWord);
					completed_Text.append(hm.begin()->second);
					completed_Text.append(selection_InsertPos_End);
					new_Cursor_Pos = selection_Begin_BeforeCurrentWord.length() + hm.begin()->second.length();
				} else {
					//match nearest only makes sense when there's actually more than one match
					if (hm.size() > 1 && sett().GetCompletionMethod() == Settings::MatchNearest) {
						wxArrayString matches;
						GetArrayStringFromHashMap(hm, matches);
						wxString newWord = GetBestMatch(matches, currentWord);

						bool realCompletion = newWord.Len() >= currentWord.Len(); // otherwise we have actually less word than before :P
						if (realCompletion)
							currentWord = newWord;

						completed_Text.append(selection_Begin_BeforeCurrentWord);
						completed_Text.append(currentWord);
						completed_Text.append(selection_InsertPos_End);
						new_Cursor_Pos = selection_Begin_BeforeCurrentWord.length() + currentWord.length();

						// We ring the System Bell, to signalise the User, that no Completion was applied.
						if (!realCompletion)
							wxBell();
					} else {
						completed_Text.append(selection_Begin_BeforeCurrentWord);
						completed_Text.append(currentWord);
						completed_Text.append(selection_InsertPos_End);
						new_Cursor_Pos = selection_Begin_BeforeCurrentWord.length() + currentWord.length();
						// We ring the System Bell, to signalise the User, that no Completion was applied.
						wxBell();
					}
				}
				// Replace the old Text with our completed Text
				// or
				// if nothing was found remove the typed TAB, so that the User stays comfortable not to remove the TAB by himself.
				this->ChangeValue(completed_Text);
				this->SetInsertionPoint(new_Cursor_Pos);
			} else {
				wxString old_Text;
				old_Text.append(selection_Begin_InsertPos);
				old_Text.append(selection_InsertPos_End);
				this->ChangeValue(old_Text);
				this->SetInsertionPoint(selection_Begin_InsertPos.length());
				wxBell();
			}
		}
		return;
	}
	//we didn't handle keycode, pass to default control
	event.Skip();
}

#if wxHAS_TEXT_WINDOW_STREAM
int wxTextCtrlHist::overflow(int i)
{
	return wxTextCtrlBase::overflow(i);
}
#endif // wxHAS_TEXT_WINDOW_STREAM
