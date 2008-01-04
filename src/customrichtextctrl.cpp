#include "customrichtextctrl.h"

#include <wx/event.h>

BEGIN_EVENT_TABLE (customRichtextCtrl,wxRichTextCtrl)
	EVT_KILL_FOCUS (customRichtextCtrl::OnFocusLost)
	EVT_SET_FOCUS (customRichtextCtrl::OnFocusGained)
END_EVENT_TABLE()

customRichtextCtrl::customRichtextCtrl(wxWindow* parent, wxWindowID id , const wxString& value, const wxPoint& pos , const wxSize& size, 
		long style, const wxValidator& validator , const wxString& name )
								:wxRichTextCtrl(parent, id , value, pos, size, style, validator, name)
{
	GetCaret()->Hide();
}

customRichtextCtrl::~customRichtextCtrl()
{
}

void customRichtextCtrl::OnFocusLost(wxFocusEvent& event)
{
	GetCaret()->Hide();
}

void customRichtextCtrl::OnFocusGained(wxFocusEvent& event)
{
	GetCaret()->Hide();
}
