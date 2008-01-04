#include "customrichtextctrl.h"

#include <wx/event.h>

BEGIN_EVENT_TABLE (customRichTextCtrl,wxRichTextCtrl)
	//EVT_KILL_FOCUS (customRichTextCtrl::OnFocusLost)
	EVT_SET_FOCUS (customRichTextCtrl::OnFocusGained)
	EVT_LEFT_DOWN (customRichTextCtrl::OnFocusLost)
END_EVENT_TABLE()

customRichTextCtrl::customRichTextCtrl(wxWindow* parent, wxWindowID id , const wxString& value, const wxPoint& pos , const wxSize& size, 
		long style, const wxValidator& validator , const wxString& name )
								:wxRichTextCtrl(parent, id , value, pos, size, style, validator, name)
{
	GetCaret()->Hide();
}

customRichTextCtrl::~customRichTextCtrl()
{
}

void customRichTextCtrl::OnFocusLost(wxMouseEvent& event)
{
	GetCaret()->Hide(); 
	event.Skip();
}

void customRichTextCtrl::OnFocusGained(wxFocusEvent& event)
{
	GetCaret()->Hide(); 
	event.Skip();
}
