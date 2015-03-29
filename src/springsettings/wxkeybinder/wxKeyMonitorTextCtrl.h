/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef __WXKEYMONITORTEXTCTRL_G__
#define __WXKEYMONITORTEXTCTRL_G__

#include "wx/textctrl.h"

//! A special text control where the user keypresses are displayed
//! as the contained text.
//! This class is used by wxKeyConfigPanel class to allow the user
//! to input a shortcut key which can be associated with a wxCmd.
//!
//! Example: the user presses CTRL+SHIFT+A inside this window;
//!          the wxKeyMonitorTextCtrl displays exactly the string
//!          "Ctrl+Shift+A"
//!
class wxKeyMonitorTextCtrl : public wxTextCtrl
{
public:
	wxKeyMonitorTextCtrl(
	    wxWindow* parent,
	    wxWindowID id,
	    const wxString& value = wxEmptyString,
	    const wxPoint& pos = wxDefaultPosition,
	    const wxSize& size = wxDefaultSize,
	    long style = wxTE_PROCESS_ENTER,
	    const wxValidator& validator = wxDefaultValidator,
	    const wxString& name = wxTextCtrlNameStr)
	    : wxTextCtrl(parent, id, value, pos, size, style, validator, name)
	{
	}

	virtual ~wxKeyMonitorTextCtrl()
	{
	}

public:
	//! Handles the keypresses and changes accordingly
	//! the text displayed in text ctrl.
	void OnKey(wxKeyEvent&);

	//! Returns TRUE if this window is containing a valid key combination.
	bool IsValidKeyComb() const
	{
		return !GetValue().IsEmpty(); //we allow '+' (&& GetValue().Last() != '+';)
	}

protected:
	int overflow(int i);

private:
	DECLARE_CLASS(wxKeyMonitorTextCtrl)
	DECLARE_EVENT_TABLE()
};

#endif
