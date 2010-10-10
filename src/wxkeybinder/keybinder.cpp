/////////////////////////////////////////////////////////////////////////////
// Name:        keybinder.cpp
// Purpose:     wxKeyBind, wxCmd, wxKeyBinder, wxKeyConfigPanel
// Author:      Aleksandras Gluchovas
// Modified by: Francesco Montorsi
// Created:     2000/02/10
// RCS-ID:      $Id: keybinder.cpp 443 2007-03-01 19:15:57Z frm $
// Copyright:   (c) Aleksandras Gluchovas and (c) Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////



// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

// includes
#include "wx/statline.h"
#include "keybinder.h"
#include "menuutils.h"
#include "wx/config.h"
#include "wx/tokenzr.h"

#include "CommandOrderDlg.h"


// class definition for wxKeyProfile
IMPLEMENT_CLASS(wxKeyProfile, wxKeyBinder)


// event table for wxKeyBinder
IMPLEMENT_CLASS(wxKeyBinder, wxObject)//EvtHandler)
IMPLEMENT_CLASS(wxBinderEvtHandler, wxEvtHandler)

BEGIN_EVENT_TABLE(wxBinderEvtHandler, wxEvtHandler)

    // this is obviously the most important event handler; we don't
    // want to intercept wxEVT_KEY_UP because we don't need them:
    // a command must be immediately executed when one of its shortcuts
    // is sent to the window.
    EVT_KEY_DOWN(wxBinderEvtHandler::OnChar)

    // if we intercept also these events, then we would have some problems
    // with the ENTER keypresses: the wxBinderEvtHandler would be called
    // three times with three different events which would then generate
    // three command executions
#if 0
    EVT_KEY_UP(wxBinderEvtHandler::OnChar)
    EVT_CHAR(wxBinderEvtHandler::OnChar)
#endif

#if defined( __WXMSW__  )       // supported only on Win32
#if wxCHECK_VERSION(2, 5, 1)    // and from wxWidgets 2.5.1

    // I don't think this is needed because wxEVT_HOTKEY are generated
    // only in some special cases...
	//FIXME: maybe re-enable this?
	//EVT_HOTKEY(wxID_ANY, wxBinderEvtHandler::OnChar)
#endif
#endif

END_EVENT_TABLE()



// event table for wxKeyMonitorTextCtrl
IMPLEMENT_CLASS(wxKeyMonitorTextCtrl, wxTextCtrl)
BEGIN_EVENT_TABLE(wxKeyMonitorTextCtrl, wxTextCtrl)

    // keydown events
    EVT_KEY_DOWN(wxKeyMonitorTextCtrl::OnKey)
    EVT_KEY_UP(wxKeyMonitorTextCtrl::OnKey)

END_EVENT_TABLE()



// event table for wxKeyConfigPanel
IMPLEMENT_CLASS(wxKeyConfigPanel, wxPanel)
BEGIN_EVENT_TABLE(wxKeyConfigPanel, wxPanel)

    // miscellaneous
    EVT_TEXT(wxKEYBINDER_KEY_FIELD_ID, wxKeyConfigPanel::OnKeyPressed)
    EVT_TEXT(wxKEYBINDER_KEYPROFILES_ID, wxKeyConfigPanel::OnProfileEditing)
    EVT_COMBOBOX(wxKEYBINDER_KEYPROFILES_ID, wxKeyConfigPanel::OnProfileSelected)
    EVT_LISTBOX(wxKEYBINDER_BINDINGS_BOX_ID, wxKeyConfigPanel::OnBindingSelected)

    // used when the treectrl is used
    EVT_TREE_SEL_CHANGED(wxKEYBINDER_COMMANDS_BOX_ID,
                        wxKeyConfigPanel::OnTreeCommandSelected)

    // used when the listbox+combobox is used
    EVT_COMBOBOX(wxKEYBINDER_CATEGORIES_ID, wxKeyConfigPanel::OnCategorySelected)
    EVT_LISTBOX(wxKEYBINDER_COMMANDS_BOX_ID, wxKeyConfigPanel::OnListCommandSelected)

    // buttons
    EVT_BUTTON(wxID_APPLY, wxKeyConfigPanel::OnApplyChanges)
    EVT_BUTTON(wxKEYBINDER_ASSIGN_KEY_ID, wxKeyConfigPanel::OnAssignKey)
    EVT_BUTTON(wxKEYBINDER_REMOVE_KEY_ID, wxKeyConfigPanel::OnRemoveKey)
    EVT_BUTTON(wxKEYBINDER_REMOVEALL_KEY_ID, wxKeyConfigPanel::OnRemoveAllKey)
    EVT_BUTTON(wxKEYBINDER_ADD_PROFILEBTN_ID, wxKeyConfigPanel::OnAddProfile)
    EVT_BUTTON(wxKEYBINDER_REMOVE_PROFILEBTN_ID, wxKeyConfigPanel::OnRemoveProfile)
	EVT_LISTBOX_DCLICK(wxKEYBINDER_BINDINGS_BOX_ID, wxKeyConfigPanel::OnBindingDblClick)
    EVT_CHECKBOX(wxKEYBINDER_ANY_MODIFIER_ID, wxKeyConfigPanel::OnAnyModifier)
	
    // during idle cycles, wxKeyConfigPanel checks if the wxKeyMonitorTextCtrl
    // associated must be cleared...

END_EVENT_TABLE()


#if !wxCHECK_VERSION(2, 5, 1)

    // with wx previous to 2.5 we need to use wxWindow::Node* instead
    // of wxWindow::compatibility_iterator (thanks to Sebastien Berthet for this)
    #define compatibility_iterator          Node*
#endif


// some statics
int wxCmd::m_nCmdTypes = 0;
wxCmd::wxCmdType wxCmd::m_arrCmdType[];




// ----------------------------------------------------------------------------
// wxKeyBind STATIC utilities
// ----------------------------------------------------------------------------

wxString wxKeyBind::NumpadKeyCodeToString(int keyCode)
{
    wxString res;

    switch (keyCode)
    {
        // NUMPAD KEYS
        // ---------------------------

    case WXK_NUMPAD0:
    case WXK_NUMPAD1:
    case WXK_NUMPAD2:
    case WXK_NUMPAD3:
    case WXK_NUMPAD4:
    case WXK_NUMPAD5:
    case WXK_NUMPAD6:
    case WXK_NUMPAD7:
    case WXK_NUMPAD8:
    case WXK_NUMPAD9:
        res << keyCode - WXK_NUMPAD0;
        break;

    case WXK_NUMPAD_SPACE:
        res << wxT("SPACE"); break;
    case WXK_NUMPAD_TAB:
        res << wxT("TAB"); break;
    case WXK_NUMPAD_ENTER:
        res << wxT("ENTER"); break;

    case WXK_NUMPAD_F1:
    case WXK_NUMPAD_F2:
    case WXK_NUMPAD_F3:
    case WXK_NUMPAD_F4:
        res << wxT("F") << keyCode - WXK_NUMPAD_F1;
        break;

    case WXK_NUMPAD_LEFT:
        res << wxT("LEFT"); break;
    case WXK_NUMPAD_UP:
        res << wxT("UP"); break;
    case WXK_NUMPAD_RIGHT:
        res << wxT("RIGHT"); break;
    case WXK_NUMPAD_DOWN:
        res << wxT("DOWN"); break;

    case WXK_NUMPAD_HOME:
        res << wxT("HOME"); break;
    case WXK_NUMPAD_PAGEUP:
        res << wxT("PAGEUP"); break;
    case WXK_NUMPAD_PAGEDOWN:
        res << wxT("PAGEDOWN"); break;
    case WXK_NUMPAD_END:
        res << wxT("END"); break;
    case WXK_NUMPAD_BEGIN:
        res << wxT("BEGIN"); break;
    case WXK_NUMPAD_INSERT:
        res << wxT("INSERT"); break;
    case WXK_NUMPAD_DELETE:
        res << wxT("DELETE"); break;
    case WXK_NUMPAD_EQUAL:
        res << wxT("="); break;
    case WXK_NUMPAD_MULTIPLY:
        res << wxT("*"); break;
    case WXK_NUMPAD_ADD:
        res << wxT("+"); break;
    case WXK_NUMPAD_SEPARATOR:
        res << wxT("SEPARATOR"); break;
    case WXK_NUMPAD_SUBTRACT:
        res << wxT("-"); break;
    case WXK_NUMPAD_DECIMAL:
        res << wxT("."); break;
    case WXK_NUMPAD_DIVIDE:
        res << wxT("/"); break;
    }

    return res;
}

wxString wxKeyBind::KeyCodeToString(int keyCode)
{
    wxString res;

    switch (keyCode)
    {
        // IGNORED KEYS
        // ---------------------------
    case WXK_START:
    case WXK_LBUTTON:
    case WXK_RBUTTON:
    case WXK_MBUTTON:
    case WXK_CLEAR:

#ifndef wxKEYBINDER_ALLOW_PAUSE_KEY
	case WXK_PAUSE:
#endif
    case WXK_NUMLOCK:
    case WXK_SCROLL :
        wxKBLogDebug(wxT("wxKeyBind::KeyCodeToString - ignored key: [%d]"), keyCode);
        return wxEmptyString;

/* We want this behavior
        // these must be ABSOLUTELY ignored: they are key modifiers
        // we won't output any LOG message since these keys could be pressed
        // for long time while the user choose its preferred keycombination:
        // this would result into a long long queue of "ignored key" messages
        // which would be useless even in debug builds...
    case WXK_SHIFT:
    case WXK_CONTROL:
        return wxEmptyString;
*/

        // FUNCTION KEYS
        // ---------------------------

    case WXK_F1: case WXK_F2:
    case WXK_F3: case WXK_F4:
    case WXK_F5: case WXK_F6:
    case WXK_F7: case WXK_F8:
    case WXK_F9: case WXK_F10:
    case WXK_F11: case WXK_F12:
    case WXK_F13: case WXK_F14:
    case WXK_F15: case WXK_F16:
    case WXK_F17: case WXK_F18:
    case WXK_F19: case WXK_F20:
    case WXK_F21: case WXK_F22:
    case WXK_F23: case WXK_F24:
        res << wxT('F') << keyCode - WXK_F1 + 1;
        break;



        // MISCELLANEOUS KEYS
        // ---------------------------

    case WXK_BACK:
        res << wxT("BACK"); break;
    case WXK_TAB:
        res << wxT("TAB"); break;
    case WXK_RETURN:
        res << wxT("RETURN"); break;
    case WXK_ESCAPE:
        res << wxT("ESCAPE"); break;
    case WXK_SPACE:
        res << wxT("SPACE"); break;
    case WXK_DELETE:
        res << wxT("DELETE"); break;
    case WXK_MULTIPLY:
        res << wxT("*"); break;
    case WXK_ADD:
        res << wxT("+"); break;
    case WXK_SEPARATOR:
        res << wxT("SEPARATOR"); break;
    case WXK_SUBTRACT:
        res << wxT("-"); break;
    case WXK_DECIMAL:
        res << wxT("."); break;
    case WXK_DIVIDE:
        res << wxT("/"); break;
    case WXK_PAGEUP:
        res << wxT("PAGEUP"); break;
    case WXK_PAGEDOWN:
        res << wxT("PAGEDOWN"); break;
    case WXK_LEFT:
        res << wxT("LEFT"); break;
    case WXK_UP:
        res << wxT("UP"); break;
    case WXK_RIGHT:
        res << wxT("RIGHT"); break;
    case WXK_DOWN:
        res << wxT("DOWN"); break;
    case WXK_SELECT:
        res << wxT("SELECT"); break;
    case WXK_PRINT:
        res << wxT("PRINT"); break;
    case WXK_EXECUTE:
        res << wxT("EXECUTE"); break;
    case WXK_SNAPSHOT:
        res << wxT("SNAPSHOT"); break;
    case WXK_INSERT:
        res << wxT("INSERT"); break;
    case WXK_HELP:
        res << wxT("HELP"); break;
    case WXK_CANCEL:
        res << wxT("CANCEL"); break;
    case WXK_MENU:
        res << wxT("MENU"); break;
    case WXK_CAPITAL:
        res << wxT("CAPITAL"); break;
    case WXK_END:
        res << wxT("END"); break;
    case WXK_HOME:
        res << wxT("HOME"); break;
#ifdef wxKEYBINDER_ALLOW_PAUSE_KEY
	case WXK_PAUSE:
		res << wxT("PAUSE"); break;			//pause added
#endif

    case WXK_CONTROL:
        res << wxT("CTRL"); break;
    case WXK_SHIFT:
        res << wxT("SHIFT"); break;
    case WXK_ALT:
        res << wxT("ALT"); break;

	default:

        // ASCII chars...
		if (keyCode < 256
#ifndef wxKEYBINDER_ALLOW_NON_ALPHANUM_KEYS
							&& wxIsalnum(keyCode)
#endif
							)
        {
            res << (wxChar)keyCode;
            break;

        } else if ((res=NumpadKeyCodeToString(keyCode)) != wxEmptyString) {

            res << wxT(" (numpad)");        // so it is clear it's different from other keys
            break;

        } else {

            // we couldn't create a description for the given keycode...
            wxKBLogDebug(wxT("wxKeyBind::KeyCodeToString - unknown key: [%d]"), keyCode);
            return wxEmptyString;
        }
    }

    return res;
}

int wxKeyBind::StringToKeyCode(const wxString &keyName)
{
    // a function key ?
    if (keyName.StartsWith(wxT("F")) && keyName.Len() > 1) {

        long n;
        keyName.Right(keyName.Len()-1).ToLong(&n);
        return WXK_F1+n-1;
    }

    // a special key ?
    if (keyName == wxT("BACK")) return WXK_BACK;
    if (keyName == wxT("ENTER")) return WXK_RETURN;
    if (keyName == wxT("RETURN")) return WXK_RETURN;
    if (keyName == wxT("TAB")) return WXK_TAB;
    if (keyName == wxT("ESCAPE")) return WXK_ESCAPE;
    if (keyName == wxT("SPACE")) return WXK_SPACE;
    if (keyName == wxT("DELETE")) return WXK_DELETE;
	if (keyName == wxT("INSERT")) return WXK_INSERT;

	if (keyName == wxT("HOME")) return WXK_HOME;
	if (keyName == wxT("END")) return WXK_END;
	if (keyName == wxT("PAGEUP")) return WXK_PAGEUP;
	if (keyName == wxT("PAGEDOWN")) return WXK_PAGEDOWN;

	if (keyName == wxT("RIGHT")) return WXK_RIGHT;
	if (keyName == wxT("LEFT")) return WXK_LEFT;
	if (keyName == wxT("UP")) return WXK_UP;
	if (keyName == wxT("DOWN")) return WXK_DOWN;

	if (keyName == wxT("CTRL")) return WXK_CONTROL;
	if (keyName == wxT("ALT")) return WXK_ALT;
	if (keyName == wxT("SHIFT")) return WXK_SHIFT;

	if (keyName == wxT("PAUSE")) return WXK_PAUSE;
	if (keyName == wxT("PRINT")) return WXK_PRINT;

	if (keyName == wxT("- (numpad)")) return WXK_NUMPAD_SUBTRACT;
	if (keyName == wxT("+ (numpad)")) return WXK_NUMPAD_ADD;
	if (keyName == wxT("* (numpad)")) return WXK_NUMPAD_MULTIPLY;
	if (keyName == wxT(". (numpad)")) return WXK_NUMPAD_DECIMAL;
	if (keyName == wxT("/ (numpad)")) return WXK_NUMPAD_DIVIDE;
	if (keyName == wxT("0 (numpad)")) return WXK_NUMPAD0;
	if (keyName == wxT("1 (numpad)")) return WXK_NUMPAD1;
	if (keyName == wxT("2 (numpad)")) return WXK_NUMPAD2;
	if (keyName == wxT("3 (numpad)")) return WXK_NUMPAD3;
	if (keyName == wxT("4 (numpad)")) return WXK_NUMPAD4;
	if (keyName == wxT("5 (numpad)")) return WXK_NUMPAD5;
	if (keyName == wxT("6 (numpad)")) return WXK_NUMPAD6;
	if (keyName == wxT("7 (numpad)")) return WXK_NUMPAD7;
	if (keyName == wxT("8 (numpad)")) return WXK_NUMPAD8;
	if (keyName == wxT("9 (numpad)")) return WXK_NUMPAD9;
	if (keyName == wxT("= (numpad)")) return WXK_NUMPAD_EQUAL;
	if (keyName == wxT("ENTER (numpad)")) return WXK_NUMPAD_ENTER;

	// it should be an ASCII key...
    return (int)keyName.GetChar(0);
}

wxString wxKeyBind::KeyModifierToString(int keyModifier)
{
    wxString result;

    if (keyModifier & wxACCEL_ANY)
        result += wxT("Any+");
    if (keyModifier & wxACCEL_CTRL)
        result += wxT("Ctrl+");
    if (keyModifier & wxACCEL_ALT)
        result += wxT("Alt+");
    if (keyModifier & wxACCEL_SHIFT)
        result += wxT("Shift+");

    return result;
}

int wxKeyBind::StringToKeyModifier(const wxString &keyModifier)
{
    int mod = 0;

    // this search must be case-insensitive
    wxString str = keyModifier;
    str.MakeUpper();

    if (str.Contains(wxT("ALT+")))
        mod |= wxACCEL_ALT;

    if (str.Contains(wxT("CTRL+")))
        mod |= wxACCEL_CTRL;

    if (str.Contains(wxT("SHIFT+")))
        mod |= wxACCEL_SHIFT;

	if (str.Contains(wxT("ANY+")))
        mod |= wxACCEL_ANY;

    return mod;
}

int wxKeyBind::GetKeyModifier(wxKeyEvent &event)
{
    int mod = 0;
    if (event.AltDown())
        mod |= wxACCEL_ALT;

    if (event.ControlDown())
        mod |= wxACCEL_CTRL;

    if (event.ShiftDown())
        mod |= wxACCEL_SHIFT;

    return mod;
}

wxString wxKeyBind::GetKeyStrokeString(wxKeyEvent &event)
{
    // key stroke string = key modifiers (Ctrl, Alt or Shift) + key code
	const wxString str = KeyCodeToString(event.GetKeyCode());
	int mods = GetKeyModifier(event);

	//delete modifiers if the actual key is a modifier key
	if ( event.GetKeyCode() == WXK_CONTROL )
	{
		mods &= ~wxACCEL_CTRL;
	}
	if ( event.GetKeyCode() == WXK_SHIFT )
	{
		mods &= ~wxACCEL_SHIFT;
	}
	if ( event.GetKeyCode() == WXK_ALT )
	{
		mods &= ~wxACCEL_ALT;
	}

	return KeyModifierToString(mods) + str;
}






// ----------------------------------------------------------------------------
// wxKeyBind
// ----------------------------------------------------------------------------

bool wxKeyBind::MatchKey(const wxKeyEvent &key) const
{
    bool b = (key.GetKeyCode() == m_nKeyCode);

    // all the flags must match
    b &= (key.AltDown() == ((m_nFlags & wxACCEL_ALT) != 0));
    b &= (key.ControlDown() == ((m_nFlags & wxACCEL_CTRL) != 0));
    b &= (key.ShiftDown() == ((m_nFlags & wxACCEL_SHIFT) != 0));

    return b;
}




// --------------------
// wxCmd
// --------------------

// static
wxCmd::wxCmdType *wxCmd::FindCmdType(int type)
{
    int found = -1;
    for (int i=0; i < m_nCmdTypes; i++)
        if (m_arrCmdType[i].type == type)
            found = i;

    if (found == -1)
        return NULL;
    return &m_arrCmdType[found];
}

// static
wxCmd *wxCmd::CreateNew(int type, int id)
{
    wxCmdType *found = FindCmdType(type);
    if (!found) return NULL;

    // get the creation function address
    wxCmdCreationFnc fnc = found->cmdCreateFnc;
    wxASSERT(fnc);          // for debug builds
    if (!fnc) return NULL;  // for release builds

    // create the wxCmd-derived class & init it
    wxCmd *ret = fnc(id);
    wxASSERT(ret);          // for debug builds
    if (!ret) return NULL;  // for release builds
    ret->Update();

    return ret;
}

// static
void wxCmd::AddCmdType(int type, wxCmdCreationFnc f)
{
    if (FindCmdType(type) != NULL)
        return;     // already registered

    m_arrCmdType[m_nCmdTypes].type = type;
    m_arrCmdType[m_nCmdTypes].cmdCreateFnc = f;
    m_nCmdTypes++;
}

bool wxCmd::Save(wxConfigBase *p, const wxString &key, bool bCleanOld) const
{
    // build the shortcut string separing each one with a "|"
    wxString shortcuts;
    for (int j=0; j < GetShortcutCount(); j++)
        shortcuts += GetShortcut(j)->GetStr() + wxT("|");

    // write the entry in the format NAME|DESC|SHORTCUT1|SHORTCUT2...|SHORTCUTn
    wxString value = wxString::Format(wxT("%s|%s|%s"),
                                    GetName().c_str(),
                                    GetDescription().c_str(),
                                    shortcuts.c_str());

    // does the given key already exists ?
    if (bCleanOld && p->Exists(key))
        p->DeleteEntry(key);        // delete old stuff...

    return p->Write(key, value);
}

bool wxCmd::Load(wxConfigBase *p, const wxString &key)
{
    wxString fmt;
    if (!p->Read(key, &fmt, wxT("|")))
        return FALSE;

    // extract name & desc
    wxStringTokenizer tknzr(fmt, wxT("|"));
    m_strName = tknzr.GetNextToken();
    m_strDescription = tknzr.GetNextToken();
    if (m_strName.IsEmpty())
        return FALSE;   // this is an invalid entry...

    // the ID of this command should have been already set by the caller
    // which created us...
    wxASSERT_MSG(m_nId != wxID_INVALID,
        wxT("ID must be set while creating of this wxCmd"));

    // extract the keybindings...
    while (tknzr.HasMoreTokens())
        AddShortcut(tknzr.GetNextToken());

    Update();
    return TRUE;
}






// --------------------
// wxCmdArray
// --------------------

void wxCmdArray::Remove(size_t n)
{
	if ( n >= GetCount())
        return;

    // first, delete the memory associated with the n-th wxCmd
    delete Item(n);

    // then, remove that pointer from the array
    m_arr.RemoveAt(n);
}

void wxCmdArray::Clear()
{
    for (size_t i=GetCount(); i > 0; i--)
        Remove(0);

    // the array should be already empty
    m_arr.Clear();
}





// ----------------------------------------------------------------------------
// wxBinderEvtHandler
// ----------------------------------------------------------------------------

void wxBinderEvtHandler::OnChar(wxKeyEvent &p)
{
    // we'll just call the wxKeyBinder OnChar function telling it
    // to execute the command on the next handler in the chain...
    // we do this because only wxKeyBinder holds the array of
    // commands & command-shortcuts...
    m_pBinder->OnChar(p, GetNextHandler());
}




/*
// ----------------------------------------------------------------------------
// wxBinderApp
// ----------------------------------------------------------------------------

int wxBinderApp::FilterEvent(wxEvent &ev)
{
    wxEvtHandler *client = m_pGlobalHdl;
    wxEvtHandler *top = GetTopWindow();

    wxWindow *focused = wxWindow::FindFocus();

    if (!client) client=top;
    if (client != top) return -1;

    wxASSERT(client);
    wxEventType t = ev.GetEventType();
    if (t == wxEVT_KEY_DOWN) {

        if (focused != NULL && focused != client &&
            GetTopLevelParent(focused) != client) {

            wxKBLogDebug(wxT("wxBinderApp::FilterEvent - discarding this keypress because our ")
                    wxT("main frame does not have the focus..."));
            return -1;
        }

        // pass this event to our keybinder
        m_pGlobalBinder->OnChar((wxKeyEvent &)ev, client);
        return ev.GetSkipped();
    }

    return -1;
}

// static
wxWindow *wxBinderApp::GetTopLevelParent(wxWindow *wnd)
{
    if (wnd->IsTopLevel())
        return wnd;
    if (wnd->GetParent())
        return GetTopLevelParent(wnd->GetParent());

    return FALSE;
}

// static
bool wxBinderApp::IsChildOf(wxWindow *parent, wxWindow *child)
{
    if (parent == child)
        return TRUE;

    for (wxWindowList::compatibility_iterator node = parent->GetChildren().GetFirst();
        node;
        node = node->GetNext())
    {
        // recursively check each child
        wxWindow *win = (wxWindow *)node->GetData();

        if (win && IsChildOf(win, child))
            return TRUE;
    }

    return FALSE;
}
*/




// ----------------------------------------------------------------------------
// wxKeyBinder
// ----------------------------------------------------------------------------

wxBinderEvtHandler *wxKeyBinder::FindHandlerFor(wxWindow *p) const
{
    int idx = FindHandlerIdxFor(p);
    if (idx == wxNOT_FOUND) return NULL;

    return (wxBinderEvtHandler *)m_arrHandlers.Item(idx);
}

int wxKeyBinder::FindHandlerIdxFor(wxWindow *p) const
{
    for (int i=0; i<(int)m_arrHandlers.GetCount(); i++)
        if (((wxBinderEvtHandler *)m_arrHandlers.Item(i))->IsAttachedTo(p))
            return i;

    return wxNOT_FOUND;
}

void wxKeyBinder::Attach(wxWindow *p)
{
    if (!p || IsAttachedTo(p))
        return;     // already attached !!!

    if (p->GetExtraStyle() & wxWS_EX_TRANSIENT)
        return;     // do not attach ourselves to temporary windows !!

    wxKBLogDebug(wxT("wxKeyBinder::Attach - attaching to [%s]"), p->GetName().c_str());

    // create a new event handler for this window
    wxEvtHandler *h = new wxBinderEvtHandler(this, p);

    // add the handler to our lists
    m_arrHandlers.Add((void*)h);

    // we need to update our commands...
    UpdateAllCmd();
}

void wxKeyBinder::AttachRecursively(wxWindow *p)
{
    if (!p)
        return;
    Attach(p);

    // this is the standard way wxWidgets uses to iterate through children...
    for (wxWindowList::compatibility_iterator node = p->GetChildren().GetFirst();
        node;
        node = node->GetNext())
    {
        // recursively attach each child
        wxWindow *win = (wxWindow *)node->GetData();

        if (win)
            AttachRecursively(win);
    }
}

void wxKeyBinder::Detach(wxWindow *p)
{
    if (!p || !IsAttachedTo(p))
        return;     // this is not attached...

    wxKBLogDebug(wxT("wxKeyBinder::Detach - detaching from [%s]"), p->GetName().c_str());

    // remove the event handler
    int idx = FindHandlerIdxFor(p);
    wxBinderEvtHandler *toremove = (wxBinderEvtHandler*)m_arrHandlers.Item(idx);
    m_arrHandlers.RemoveAt(idx, 1);

    // the wxBinderEvtHandler will remove itself from p's event handlers
    delete toremove;
}

void wxKeyBinder::DetachAll()
{
    wxKBLogDebug(wxT("wxKeyBinder::DetachAll - detaching from all my [%d] targets"),
				 (int)GetAttachedWndCount());

    // delete all handlers (they will automatically remove themselves from
    // event handler chains)
    for (int i=0; i < (int)m_arrHandlers.GetCount(); i++)
        delete (wxBinderEvtHandler*)m_arrHandlers.Item(i);

    // and clear the array
    m_arrHandlers.Clear();
}


void wxKeyBinder::OnChar(wxKeyEvent &event, wxEvtHandler *next)
{
    // this is the first function which is called when the user presses
    // a key in the window Attach()ed to this key binder: we have to
    // find if the given keyevent matches a key shortcut and eventually
    // exec the command associated with that shortcut...
    wxCmd *p = GetMatchingCmd(event);

    // AVOID TO INTERCEPT Alt+F4 KEYPRESSES !!!
    // For some reasons on wxMSW 2.5.2 (at least) this provokes a crash
    // which is really difficult to spot... better leave it...
    if (p && p->IsBindTo(wxKeyBind(wxT("Alt+F4")))) {

        wxKBLogDebug(wxT("wxKeyBinder::OnChar - ignoring an Alt+F4 event [%d]"),
                    event.GetKeyCode());
        event.Skip();
        return;
    }

#if 0
    // for some reason we need to avoid processing also of the ENTER keypresses...
    if (p && p->IsBindTo(wxKeyBind(wxT("ENTER")))) {

        wxKBLogDebug(wxT("wxKeyBinder::OnChar - ignoring an ENTER event [%d]"),
                    event.GetKeyCode());
        event.Skip();
        return;
    }
#endif

    // if the given event is not a shortcut key...
    if (p == NULL) {

        wxKBLogDebug(wxT("wxKeyBinder::OnChar - ignoring this keyevent [%d]"),
                    event.GetKeyCode());
        event.Skip();       // ... skip it

    } else {

        wxEvtHandler *client = next;//this->GetNextHandler();
        /*if (client == NULL) {

            //int found = -1;
            for (int i=0; i < (int)m_arrAttachedWnd.GetCount(); i++)
                if (((wxWindow*)m_arrAttachedWnd.Item(i)) == event.GetEventObject())//->GetId() == event.GetId())
                    client = (wxWindow*)m_arrAttachedWnd.Item(i);
        }*/

        if (client == NULL) {

            wxKBLogDebug(wxT("wxKeyBinder::OnChar - ignoring this keyevent [%d] because I'm not ")
                wxT("attached to the window which generated the keypress..."), event.GetKeyCode());
            event.Skip();       // ... skip it
            return;
        }

        wxKBLogDebug(wxT("wxKeyBinder::OnChar - calling the Exec() function of the [%s] ")
                wxT("wxCmd on the keycode [%d] (event timestamp: %d)"),
                p->GetName().c_str(), event.GetKeyCode(), event.GetTimestamp());
        p->Exec(event.GetEventObject(),     // otherwise, tell wxCmd to send the
                client);    // associated wxEvent to the next handler in the chain
    }
}

bool wxKeyBinder::Save(wxConfigBase *cfg, const wxString &key, bool bCleanOld) const
{
    wxString basekey = (key.IsEmpty() ? wxString(wxT("")) : wxString(key + wxT("/")));
    bool b = TRUE;

    // does the given key already exists ?
    if (bCleanOld && cfg->Exists(basekey))
        cfg->DeleteGroup(basekey);      // delete old stuff...

    for (size_t i=0; i < m_arrCmd.GetCount(); i++) {

        wxCmd *curr = m_arrCmd.Item(i);

        // write the key in the format: bindID-typeID
        wxString keyname = wxString::Format(wxT("%s%s%d-type%d"),
            basekey.c_str(), wxCMD_CONFIG_PREFIX, curr->GetId(), curr->GetType());

        // save this wxCmd...
        b &= curr->Save(cfg, keyname);
    }

    return b;
}

bool wxKeyBinder::Load(wxConfigBase *p, const wxString &key)
{
    wxString str;
    bool cont;
    bool b = TRUE;
    int total = 0;
    long idx;

    // before starting...
    p->SetPath(key);
    m_arrCmd.Clear();

    cont = p->GetFirstEntry(str, idx);
    while (cont) {

        // try to decode this entry
        if (str.StartsWith(wxCMD_CONFIG_PREFIX)) {

            wxString id(str.BeforeFirst(wxT('-')));
            wxString type(str.AfterFirst(wxT('-')));
            id = id.Right(id.Len()-wxString(wxCMD_CONFIG_PREFIX).Len());
            type = type.Right(type.Len()-wxString(wxT("type")).Len());

            // is this a valid entry ?
            if (id.IsNumber() && type.IsNumber() &&
                p->GetEntryType(str) == wxConfigBase::Type_String) {

                // we will interpret this group as a command ID
                int nid = wxAtoi(id);
                int ntype = wxAtoi(type);

                // create & load this command
                wxCmd *cmd = wxCmd::CreateNew(ntype, nid);
                if (!cmd || !cmd->Load(p, str)) {
                    cont = FALSE;
                    break;
                }

                m_arrCmd.Add(cmd);      // add to the array
                total++;
            }
        }

        // proceed with next entry (if it does exist)
        cont &= p->GetNextEntry(str, idx);
    }

    return (b && total > 0);
}





// ----------------------------------------------------------------------------
// wxKeyProfile
// ----------------------------------------------------------------------------

bool wxKeyProfile::Save(wxConfigBase *cfg, const wxString &key, bool bCleanOld) const
{
    // we will encode our name into the key used to group all this profile
    wxString basekey = key.IsEmpty() ? wxString(wxT("")) : wxString(key + wxT("/"));

    // does the given key already exists ?
    if (bCleanOld && cfg->Exists(basekey))
        cfg->DeleteGroup(basekey);      // delete old stuff...

    // write our name & description into a specific key
    if (!cfg->Write(basekey + wxT("/desc"), GetDesc()))
        return FALSE;
    if (!cfg->Write(basekey + wxT("/name"), GetName()))
        return FALSE;

    // tell wxKeyBinder to save all keybindings into a group with our name
    // and also tell it NOT to delete the given key if it already exists
    // because it ALWAYS exists since we've just created it...
    return wxKeyBinder::Save(cfg, basekey, FALSE);
}

bool wxKeyProfile::Load(wxConfigBase *p, const wxString &key)
{
    p->SetPath(key);        // enter into this group

    wxString name;
    wxString desc;

    // do we have valid DESC & NAME entries ?
    if (p->HasEntry(wxT("desc")) && p->HasEntry(wxT("name"))) {

        if (!p->Read(wxT("desc"), &desc))
            return FALSE;
        if (!p->Read(wxT("name"), &name))
            return FALSE;

        // check for valid name & desc
        if (name.IsEmpty())
            return FALSE;

        SetName(name);
        SetDesc(desc);

        // load from current path (we cannot use '.')
        return wxKeyBinder::Load(p, wxT("../") + key);
    }

    return FALSE;   // no valid DESC/NAME entry...
}




// ----------------------------------------------------------------------------
// wxKeyProfileArray
// ----------------------------------------------------------------------------

bool wxKeyProfileArray::Save(wxConfigBase *cfg, const wxString &key, bool bCleanOld) const
{
    wxString basekey = (key.IsEmpty()) ? wxString(wxT("")) : wxString(key + wxT("/"));
    bool b = TRUE;

    cfg->SetPath(key);
    if (!cfg->Write(basekey + wxT("nSelProfile"), m_nSelected ))
        return FALSE;

    for (size_t i=0; i<GetCount(); i++)

        // save all our elements into a subkey of the given key
        b &= Item(i)->Save(cfg, basekey + wxKEYPROFILE_CONFIG_PREFIX +
                                    wxString::Format(wxT("%d"), (int)i), bCleanOld);

    // if required, remove any previously stored key profile...
    if (bCleanOld) {

        // the Save() calls could have changed our current path...
        cfg->SetPath(key);

        // enumerate all groups
        wxString str;
        long n, idx;

        bool cont = cfg->GetFirstGroup(str, idx);
        while (cont) {
            if (str.StartsWith(wxKEYPROFILE_CONFIG_PREFIX)) {

                // extract the id of this group...
                wxString id=str.Right(str.Len()-wxString(wxKEYPROFILE_CONFIG_PREFIX).Len());
                id.ToLong(&n);

                if (n >= (long)GetCount()) {

                    // this is a profile which was saved in a previous session
                    // but which has now been removed by the user... remove it
                    // from our config settings...
                    cfg->DeleteGroup(str);

                    // re-start our enumeration; otherwise GetNextGroup() won't work...
                    cont = cfg->GetFirstGroup(str, idx);
                    if (!cont) break;
                }
            }

            // proceed with next one...
            cont &= cfg->GetNextGroup(str, idx);
        }
    }

    return b;
}

bool wxKeyProfileArray::Load(wxConfigBase *p, const wxString &key)
{
    wxKeyProfile tmp;
    wxString str;
    bool cont;
    long idx;

    // before starting...
    p->SetPath(key);
    if (!p->Read(wxT("nSelProfile"), ( &m_nSelected ) ))
        return FALSE;

    cont = p->GetFirstGroup(str, idx);
    while (cont) {

        // try to decode this group name
        if (str.StartsWith(wxKEYPROFILE_CONFIG_PREFIX)) {

            // is this a valid entry ?
            if (!tmp.Load(p, str))
                return FALSE;

            Add(new wxKeyProfile(tmp));     // yes, it is; add it to the array
        }

        // set the path again (it could have been changed...)
        p->SetPath(key);

        // proceed with next entry (if it does exist)
        cont &= p->GetNextGroup(str, idx);
    }

    return TRUE;
}




// ----------------------------------------------------------------------------
// wxKeyMonitorTextCtrl
// ----------------------------------------------------------------------------

void wxKeyMonitorTextCtrl::OnKey(wxKeyEvent &event)
{
    // backspace cannot be used as shortcut key...
#ifndef wxKEYBINDER_ALLOW_BACKSPACE
	if (event.GetKeyCode() == WXK_BACK) {

        // this text ctrl contains something and the user pressed backspace...
        // we must delete the keypress...
        Clear();
        return;
    }
#endif

    if (event.GetEventType() == wxEVT_KEY_DOWN ||
        (event.GetEventType() == wxEVT_KEY_UP && !IsValidKeyComb())) {

        // the user pressed some key combination which must be displayed
        // in this text control.... or he has just stopped pressing a
        // modifier key like shift, ctrl or alt without adding any
        // other alphanumeric char, thus generating an invalid keystroke
        // which must be cleared out...
        SetValue(wxKeyBind::GetKeyStrokeString(event));
        SetInsertionPointEnd();
    }
}




// ----------------------------------------------------------------------------
// wxKeyConfigPanel - BUILD functions
// ----------------------------------------------------------------------------

bool wxKeyConfigPanel::Create(wxWindow* parent,
                              wxWindowID id,
                              const wxPoint& pos, const wxSize& size,
                              long style, const wxString& name,
							  const wxString& customButton1Label,
							  const wxObjectEventFunction customButton1Event,
							  const wxString& customButton2Label,
							  const wxObjectEventFunction customButton2Event,
							  const wxString& customLabelText )
{
    if (!wxPanel::Create(parent, id, pos, size, style, name))
        return false;

    m_bProfileHasBeenModified = FALSE;
	m_bProfileModifiedOrChanged = FALSE;

    wxASSERT_MSG(HasFlag(wxKEYBINDER_USE_LISTBOX) ||
                 HasFlag(wxKEYBINDER_USE_TREECTRL),
                 wxT("You must specify one of the two !!"));
    wxASSERT_MSG(!(HasFlag(wxKEYBINDER_USE_LISTBOX) &&
                 HasFlag(wxKEYBINDER_USE_TREECTRL)),
                 wxT("You cannot specify them both !!"));

	if ( customButton1Label.size() > 0 && customButton1Event )
	{
		m_pCustomButton1 = new wxButton( this, wxID_ANY, customButton1Label, wxPoint(20, 20) );
		m_pCustomButton1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, customButton1Event, NULL, parent );
	}

	if ( customButton2Label.size() > 0 && customButton2Event )
	{
		m_pCustomButton2 = new wxButton( this, wxID_ANY, customButton2Label, wxPoint(20, 20) );
		m_pCustomButton2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, customButton2Event, NULL, parent );
	}

	wxCustomLabelText = customLabelText;

    // build everything
    BuildCtrls();
    wxSizer *column1 = BuildColumn1();
    wxSizer *column2 = BuildColumn2();
    wxSizer *main = BuildMain(column1, column2,
        HasFlag(wxKEYBINDER_SHOW_APPLYBUTTON) != 0);

    // set the final sizer as window's sizer
//    SetSizer(main);
//    main->SetSizeHints(this);
    SetSizerAndFit(main);

    // set up the controls: the user of the panel must call one of the
    // ImportXXXX() functions to enable the use of the panel !!!!
    GetMainCtrl()->SetFocus();
    UpdateButtons();

    return true;
}

wxKeyConfigPanel::~wxKeyConfigPanel()
{
	this->RemoveAllProfiles();
}

void wxKeyConfigPanel::BuildCtrls()
{
    if (HasFlag(wxKEYBINDER_USE_TREECTRL)) {

        // use a wxTreeCtrl to show the commands hierarchy
        m_pCommandsTree = new wxTreeCtrl(this, wxKEYBINDER_COMMANDS_BOX_ID, wxDefaultPosition,
                                    wxSize(-1, 200), wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
    } else {

        // use a combobox + a listbox
        m_pCommandsList = new wxListBox(this, wxKEYBINDER_COMMANDS_BOX_ID, wxDefaultPosition,
                                    wxDefaultSize, (int)0, (const wxString *)NULL);
        m_pCategories = new wxComboBox(this, wxKEYBINDER_CATEGORIES_ID,
                                wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                0, NULL, wxCB_READONLY);
    }

    m_pKeyField = new wxKeyMonitorTextCtrl(this, wxKEYBINDER_KEY_FIELD_ID);
    m_pBindings = new wxListBox(this, wxKEYBINDER_BINDINGS_BOX_ID);

    m_pAssignBtn = new wxButton(this, wxKEYBINDER_ASSIGN_KEY_ID, wxT("&Add"));
    m_pRemoveBtn = new wxButton(this, wxKEYBINDER_REMOVE_KEY_ID, wxT("&Remove"));
    m_pRemoveAllBtn = new wxButton(this, wxKEYBINDER_REMOVEALL_KEY_ID, wxT("Remove all"));

	m_pAnyModCbx = new wxCheckBox(this, wxKEYBINDER_ANY_MODIFIER_ID, wxT("\"Any\" modifier") );

    m_pCurrCmdField = new wxStaticText( this, -1, wxT(""), wxDefaultPosition,
        wxSize(-1, 20), wxSIMPLE_BORDER | wxST_NO_AUTORESIZE | wxALIGN_CENTRE);

    // we won't make it white because it must be clear to the user that this
    // is not a text control...
    m_pCurrCmdField->SetBackgroundColour(wxColour(200, 200, 200));

#ifdef __WXGTK__
    m_pDescLabel = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition,
                                 wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);
#else
    m_pDescLabel = new wxStaticText(this, -1, wxT(""), wxDefaultPosition,
        wxSize(-1, 40), wxSIMPLE_BORDER | wxST_NO_AUTORESIZE);
    m_pDescLabel->SetBackgroundColour(wxColour(255, 255, 255));
#endif

    // KEY PROFILES
    // create the key profiles combobox & panel
    m_bEnableKeyProfiles = TRUE;

    // the style of a combobox must be set at the beginning:
    // you cannot change the wxCB_READONLY flag presence later...
    // VERY IMPORTANT: *NEVER* ADD THE CB_SORT STYLE:
    // IT WOULD GIVE US GREAT PROBLEMS WHEN EDITING THE KEYPROFILE...
    long style = HasFlag(wxKEYBINDER_ENABLE_PROFILE_EDITING) ? 0 : wxCB_READONLY;
    m_pKeyProfiles = new wxComboBox(this, wxKEYBINDER_KEYPROFILES_ID,
                                wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                0, NULL, style);

    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_pKeyProfiles, 6, wxGROW);

    if (HasFlag(wxKEYBINDER_SHOW_ADDREMOVE_PROFILE)) {

        // create the Add & remove profile buttons
        sizer->Add(new wxButton(this, wxKEYBINDER_ADD_PROFILEBTN_ID, wxT("Add new")), 0,
                            wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5);
        sizer->Add(new wxButton(this, wxKEYBINDER_REMOVE_PROFILEBTN_ID, wxT("Remove")), 0,
                            wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    }

    m_pKeyProfilesSizer = new wxBoxSizer(wxVERTICAL);
    m_pKeyProfilesSizer->Add(new wxStaticText(this, -1, wxT("Key profile:")), 0, wxGROW | wxALL, 5);
    m_pKeyProfilesSizer->Add(sizer, 0, wxGROW | wxLEFT | wxRIGHT, 5);
    m_pKeyProfilesSizer->Add(new wxStaticLine(this, -1), 0, wxGROW | wxALL, 5);
}

wxSizer *wxKeyConfigPanel::BuildColumn1()
{
    // FIRST COLUMN: "Groups"
    wxBoxSizer *column1 = new wxBoxSizer(wxVERTICAL);

    if (HasFlag(wxKEYBINDER_USE_TREECTRL)) {

        // add the treectrl
        column1->Add(new wxStaticText(this, -1, wxT("Commands:")), 0, wxGROW | wxALL, 5);
        column1->Add(m_pCommandsTree, 1, wxGROW | wxRIGHT | wxLEFT | wxBOTTOM, 5);

    } else {

        // add a combobox + listbox
        column1->Add(new wxStaticText(this, -1, wxT("Categories:")), 0, wxGROW | wxALL, 5);
        column1->Add(m_pCategories, 1, wxGROW | wxRIGHT | wxLEFT | wxBOTTOM, 5);

        column1->Add(new wxStaticText(this, -1, wxT("Commands:")), 0,
                                    wxGROW | wxRIGHT | wxLEFT | wxBOTTOM, 5);
        column1->Add(m_pCommandsList, 5, wxGROW | wxRIGHT | wxLEFT | wxBOTTOM, 5);
    }

	if ( m_pCustomButton1 )
	{
		column1->Add( new wxStaticText(this, -1, wxCustomLabelText ), 0, wxGROW | wxALL, 5 );
		wxSizer* pS = new wxBoxSizer( wxHORIZONTAL );
		pS->Add(m_pCustomButton1, 0, wxGROW | wxALL, 5);
		pS->Add(m_pCustomButton2, 0, wxGROW | wxALL, 5);

		column1->Add(pS);
	}

    return column1;
}

wxSizer *wxKeyConfigPanel::BuildColumn2()
{
    // SECOND COLUMN: wxT("Current keys") + "Press new shortcut key"
    wxBoxSizer *column2 = new wxBoxSizer(wxVERTICAL);
    column2->Add(new wxStaticText(this, -1, wxT("Current shortcuts:")), 0, wxGROW | wxALL, 5);
    column2->Add(m_pBindings, 2, wxGROW | wxRIGHT | wxLEFT, 5);

	wxBoxSizer *extraMods = new wxBoxSizer(wxHORIZONTAL);
    extraMods->Add(m_pAnyModCbx, 1, wxGROW | wxALL, 5);
	column2->Add(extraMods, 0, wxGROW);

    wxBoxSizer *removebtns = new wxBoxSizer(wxHORIZONTAL);
    removebtns->Add(m_pRemoveBtn, 1, wxGROW | wxALL, 5);
    removebtns->Add(m_pRemoveAllBtn, 1, wxGROW | wxALL, 5);

    column2->Add(removebtns, 0, wxGROW);
    //column2->Add(new wxStaticLine(this, -1), 0, wxGROW | wxALL, 10);
    column2->Add(new wxStaticText(this, -1, wxT("New shortcut:")), 0, wxGROW | wxALL, 5);
    column2->Add(m_pKeyField, 0, wxGROW | wxLEFT | wxRIGHT, 5);
    column2->Add(new wxStaticText(this, -1, wxT("Currently assigned to:")), 0, wxGROW | wxALL, 5);
    column2->Add(m_pCurrCmdField, 0, wxGROW | wxLEFT | wxRIGHT, 5);
    column2->Add(m_pAssignBtn, 0, wxGROW | wxALL, 5);

    return column2;
}

wxSizer *wxKeyConfigPanel::BuildMain(wxSizer *column1, wxSizer *column2, bool bApplyBtn)
{
    // set up the column container
    wxBoxSizer *cont = new wxBoxSizer(wxHORIZONTAL);
    cont->Add(column1, 4, wxGROW);
    cont->Add(1, 1, 0, wxGROW);
    cont->Add(column2, 4, wxGROW);

    // create the main sizer
    wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
    main->Add(m_pKeyProfilesSizer, 0, wxGROW);
    main->Add(cont, 5, wxGROW);
    main->Add(new wxStaticLine(this, -1), 0, wxGROW | wxALL, 5);

    // key description
    main->Add(new wxStaticText(this, -1, wxT("Description:")), 0, wxGROW | wxALL, 5);
#ifdef __WXGTK__
    main->Add(m_pDescLabel, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5);
#else
    main->Add(m_pDescLabel, 1, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5);
#endif

    // if required, add Apply and Cancel buttons
    if (bApplyBtn) {

        wxBoxSizer *btn = new wxBoxSizer(wxHORIZONTAL);
        wxButton *applyBtn = new wxButton(this, wxID_APPLY,  wxT("A&pply"));
        wxButton *cancelBtn = new wxButton(this, wxID_CANCEL, wxT("C&ancel"));

        // create the button panel with some interleaved spacer
        btn->Add(1, 1, 1, wxGROW);
        btn->Add(applyBtn, 4, wxGROW | wxALL, 5);
        btn->Add(1, 1, 1, wxGROW);
        btn->Add(cancelBtn, 4, wxGROW | wxALL, 5);
        btn->Add(1, 1, 1, wxGROW);

        main->Add(1, 1, 0, wxGROW);
        main->Add(btn, 1, wxGROW | wxALL, 5);
    }

    return main;
}



// ----------------------------------------------------------------------------
// wxKeyConfigPanel - IMPORT functions
// ----------------------------------------------------------------------------
void wxKeyConfigPanel::ImportRawList(const ControlMap& itemMap, const wxString &rootname)
{
	if (!IsUsingTreeCtrl())
	{
		//not supported
		return;
	}

	// do some std things...
    Reset();

	AddRootIfMissing(rootname);

	wxTreeItemId rootid = m_pCommandsTree->GetRootItem();

	for( ControlMap::const_iterator iter = itemMap.begin(); iter != itemMap.end(); ++iter )
	{
		wxTreeItemId newId = m_pCommandsTree->AppendItem(rootid, iter->first);

		for( CommandList::const_iterator iiter = iter->second.begin(); iiter != iter->second.end(); ++iiter )
		{
			wxExTreeItemData *treedata = new wxExTreeItemData(iiter->second);
			m_pCommandsTree->AppendItem(newId, iiter->first, -1, -1, treedata );
			m_pCommandsTree->SortChildren(newId);
		}
	}


    // expand the root (just for aesthetic/comfort reasons)...
    m_pCommandsTree->Expand(m_pCommandsTree->GetRootItem());
}

/*
void wxKeyConfigPanel::ImportMenuBarCmd(wxMenuBar *p, const wxString &rootname)
{
    // do some std things...
    Reset();

    // the wxKeyBinder class can easily generate a tree structure...
    if (IsUsingTreeCtrl()) {

        AddRootIfMissing(rootname);

        wxMenuTreeWalker wlkr;
        wlkr.FillTreeCtrl(p, m_pCommandsTree, rootname);

        // expand the root (just for aesthetic/comfort reasons)...
        m_pCommandsTree->Expand(m_pCommandsTree->GetRootItem());

    } else {

        wxMenuComboListWalker wlkr;
        wlkr.FillComboListCtrl(p, m_pCategories);

        // select the first item (just for aesthetic/comfort reasons)...
        m_pCategories->SetSelection(0);

        wxCommandEvent fake;
        OnCategorySelected(fake);
    }
}
*/
void wxKeyConfigPanel::ImportKeyProfileCmd(const wxKeyProfile &toimport,
                                           const wxString &rootname)
{
    // do some std things...
    Reset();

    if (IsUsingTreeCtrl()) {

        AddRootIfMissing(rootname);
        wxTreeItemId rootid = m_pCommandsTree->GetRootItem();

        // scan all the commands of the key binder...
        const wxCmdArray *arr = toimport.GetArray();
        for (int i=0; i < (int)arr->GetCount(); i++) {

            // to each tree branch attach a wxTreeItemData containing
            // the ID of the menuitem which it represents...
            wxExTreeItemData *treedata = new wxExTreeItemData(arr->Item(i)->GetId());

            // create the new item in the tree ctrl
            m_pCommandsTree->AppendItem(rootid, arr->Item(i)->GetName(), -1, -1, treedata);
        }

        // expand the root (just for aesthetic/comfort reasons)...
        m_pCommandsTree->Expand(m_pCommandsTree->GetRootItem());

    } else {

        const wxCmdArray *arr = toimport.GetArray();
        for (size_t i=0; i < arr->GetCount(); i++) {

            // create a list of items containing as untyped client data
            // (void*) the INT which is their ID...
            m_pCommandsList->Append(arr->Item(i)->GetName(),
                                    (void*)(arr->Item(i)->GetId()));
        }

        // in the categories combobox just add a generic title
        m_pCategories->Append(wxT("Generic"));
    }
}





// ----------------------------------------------------------------------------
// wxKeyConfigPanel - MISCELLANEOUS functions
// ----------------------------------------------------------------------------

void wxKeyConfigPanel::AddProfile(const wxKeyProfile &p, bool markAsChanged)
{
    // add a new profile to the array
    m_pKeyProfiles->Append(p.GetName(), (void *)(new wxKeyProfile(p)));

    if (m_pKeyProfiles->GetCount() == 1)

        // the profile we added is the only one present... select it
        SetSelProfile(0);

	if (markAsChanged)
		this->m_bProfileModifiedOrChanged = true;
}

void wxKeyConfigPanel::AddProfiles(const wxKeyProfileArray &arr)
{
    // copy the given profiles into the listbox data list
    for (size_t i=0; i < arr.GetCount(); i++) {
        wxKeyProfile *copy = new wxKeyProfile(*arr.Item(i));
        m_pKeyProfiles->Append(arr.Item(i)->GetName(), (void *)copy);
    }

	SetSelProfile( arr.GetSelProfileIdx() );
}

void wxKeyConfigPanel::RemoveAllProfiles()
{
	// with the AddXXXXX functions we created wxKeyProfiles which we
    // then added into the m_pKeyProfiles combobox... we now must delete them.
    for (size_t i=0; i < m_pKeyProfiles->GetCount(); i++) {
        wxKeyProfile *data = (wxKeyProfile *)m_pKeyProfiles->GetClientData(i);

        // we can delete the client data safely because wxComboBox will leave
        // the client data field untouched...
        if (data) delete data;
    }

	m_pKeyProfiles->Clear();
}

wxTreeItemId wxKeyConfigPanel::FindTreeItem( wxTreeItemId root, const wxString& sSearchFor )
{
	wxTreeItemIdValue cookie;
	wxTreeItemId search;
	wxTreeItemId item = m_pCommandsTree->GetFirstChild( root, cookie );
	wxTreeItemId child;

	while( item.IsOk() )
	{
		wxString sData = m_pCommandsTree->GetItemText(item);
		if( sSearchFor.CompareTo(sData) == 0 )
		{
			return item;
		}
		if( m_pCommandsTree->ItemHasChildren( item ) )
		{
			wxTreeItemId search = FindTreeItem( item, sSearchFor );
			if( search.IsOk() )
			{
				return search;
			}
		}
		item = m_pCommandsTree->GetNextChild( root, cookie);
	}

	/* Not found */
	wxTreeItemId dummy;
	return dummy;
}

void wxKeyConfigPanel::SelectCommand( const wxString& cmd )
{
	wxTreeItemId id = this->FindTreeItem( m_pCommandsTree->GetRootItem(), cmd );

	if ( id != wxTreeItemId() )
	{
		m_pCommandsTree->SelectItem( id );
	}
}

void wxKeyConfigPanel::SelectKeyString( const wxString& keyStr )
{
	for ( unsigned i = 0; i < this->m_pBindings->GetCount(); ++i )
	{
		if ( this->m_pBindings->GetString( i ) == keyStr )
		{
			this->m_pBindings->Select( i );
			this->UpdateButtons();
			return;
		}
	}
}

void wxKeyConfigPanel::SetSelProfile(int n)
{
    wxASSERT(m_pKeyProfiles && n >= 0 &&
			 n < (int)m_pKeyProfiles->GetCount());

    m_pKeyProfiles->SetSelection(n);
    m_nCurrentProf = n;

    // generate a fake event: SetSelection does not generate it
    wxCommandEvent ev;
    OnProfileSelected(ev);
}

void wxKeyConfigPanel::Reset()
{
    if (IsUsingTreeCtrl()) {

        m_pCommandsTree->DeleteAllItems();

    } else {

        m_pCommandsList->Clear();
        m_pCategories->Clear();
    }

    m_pBindings->Clear();
#ifdef __WXGTK__
    m_pDescLabel->Clear();
#else
    m_pDescLabel->SetLabel(wxT(""));
#endif
    m_pKeyField->Clear();
}

void wxKeyConfigPanel::AddRootIfMissing(const wxString &rootname)
{
    wxASSERT(IsUsingTreeCtrl());    // this function is treectrl-specific

    if (!m_pCommandsTree->GetRootItem().IsOk()) {

        // I don't know what happens if we try to create the root
        // of the control while there are still some items in the
        // control... even if I don't think some items maybe
        // present in the control if the root is not okay....
        m_pCommandsTree->DeleteAllItems();

        m_pCommandsTree->AddRoot(rootname);
    }
}

wxCmd *wxKeyConfigPanel::GetSelCmd() const
{
    long id;

    if (IsUsingTreeCtrl()) {

        wxTreeItemId treeid = GetSelCmdId();
        if (!treeid.IsOk())
            return NULL;

        // each tree item has a wxMenuTreeItemData associated which contains
        // the ID of the menu item which is attached...
        id  = ((wxExTreeItemData *)m_pCommandsTree->GetItemData(treeid))->GetMenuItemId();

    } else {

        int sel = m_pCommandsList->GetSelection();
        if (sel < 0)
            return NULL;

        id = (long)(m_pCommandsList->GetClientData(sel));
    }

    return m_kBinder.GetCmd(id);
}

wxTreeItemId wxKeyConfigPanel::GetSelCmdId() const
{
    wxTreeItemId selection = m_pCommandsTree->GetSelection();

    // selection must be valid...
    if (!selection.IsOk())
        return selection;

    // ...and the just selected must be a valid menu item...
    wxTreeItemData *p = m_pCommandsTree->GetItemData(selection);
    if (p == NULL)
        return wxTreeItemId();      // an empty wxTreeItemId is always invalid...

    return selection;
}

wxControl *wxKeyConfigPanel::GetMainCtrl() const
{
    if (HasFlag(wxKEYBINDER_USE_TREECTRL))
        return (wxControl*)m_pCommandsTree;
    return m_pCommandsList;
}

wxString wxKeyConfigPanel::GetSelCmdStr() const
{
    wxTreeItemId id = GetSelCmdId();

    // wxEmpyString is returned if there's no valid selection
    if (!id.IsOk())
        return wxEmptyString;

    return m_pCommandsTree->GetItemText(id);
}

bool wxKeyConfigPanel::IsSelectedValidCmd() const
{
    if (IsUsingTreeCtrl())
        return GetSelCmdId().IsOk();
    else
        return m_pCommandsList->GetSelection() >= 0;
}

wxKeyProfileArray wxKeyConfigPanel::GetProfiles() const
{
    wxKeyProfileArray arr;

    // just copy the combobox item's client data (which are wxKeyProfiles)
    // into our array...
    // NB: it's very important to *copy* the profiles into the new array
    //     since the ddestructor of wxKeyConfigPanel expect the m_pKeyProfiles
    //     control to contain always valid pointers NOT shared with anyone else
    for (size_t i=0; i<m_pKeyProfiles->GetCount(); i++)
        arr.Add(new wxKeyProfile(*GetProfile(i)));
    arr.SetSelProfile(GetSelProfileIdx());

    return arr;
}






// ----------------------------------------------------------------------------
// wxKeyConfigPanel - UPDATE functions
// ----------------------------------------------------------------------------

void wxKeyConfigPanel::UpdateButtons()
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::UpdateButtons"));
    wxString str = wxT("None");

    // is the remove button to be enabled ?
    m_pRemoveBtn->Enable(m_pBindings->GetSelection() >= 0);
    m_pRemoveAllBtn->Enable(m_pBindings->GetCount() > 0);
	m_pAnyModCbx->Enable(m_pBindings->GetSelection() >= 0);

	const wxString& curSelKey = m_pBindings->GetStringSelection();
	if ( curSelKey.size() > 0 )
	{
		bool state = false;
		if ( curSelKey.StartsWith(wxT("Any+")) )
		{
			state = true;
		}
		this->m_pAnyModCbx->SetValue(state);
	}	

	const int selIdx = m_pBindings->GetSelection();
	if ( selIdx == wxNOT_FOUND )
	{
		return;
	}

	CmdSet cmds = m_kBinder.GetCmdBindsTo(m_pBindings->GetStringSelection());

	assert( cmds.size() > 0 );

	CommandOrderDlg dlg( m_pBindings->GetStringSelection(), cmds, this );
	

    // is the assign button to be enabled ?
    bool b = IsSelectedValidCmd() && m_pKeyField->IsValidKeyComb();
    m_pAssignBtn->Enable(b);

    // must the "Currently assigned to" field be updated ?
    if (m_pKeyField->IsValidKeyComb()) {
        //wxCmd *p = m_kBinder.GetCmdBindTo(m_pKeyField->GetValue());
		CmdSet cmds = m_kBinder.GetCmdBindsTo(m_pKeyField->GetValue());

		if ( cmds.size() == 0 )
		{
            str = wxT("None");
            m_pCurrCmd = NULL;
		}
		else
		{
			str.clear();
			for ( CmdSet::const_iterator iter = cmds.begin(); iter != cmds.end(); ++iter )
			{
				if ( iter != cmds.begin() )
				{
					str += wxT(",");
				}
				m_pCurrCmd = (*iter); //dirty, lets look what happens
				str += (*iter)->GetName();
			}
		}
    }

	const wxString::size_type maxSize = 20;
	if ( str.length() > maxSize )
	{
		m_pCurrCmdField->SetToolTip(str);
		str.RemoveLast( str.length() - maxSize );
		str += wxT("...");
	}

    m_pCurrCmdField->SetLabel(str);
}

void wxKeyConfigPanel::UpdateDesc()
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::UpdateDesc"));
    wxCmd *p = GetSelCmd();

    if (p != NULL) {

        // and then update the description
#ifdef __WXGTK__
        m_pDescLabel->SetValue(p->GetDescription());
#else
        m_pDescLabel->SetLabel(p->GetDescription());
#endif
    } else {

        // an invalid command is selected ? clear this field...
        m_pDescLabel->SetLabel(wxT(""));
    }
}

void wxKeyConfigPanel::FillInBindings()
{
    // remove (eventually present) previous shortcuts...
    m_pBindings->Clear();

    // is the selected item okay ?
    wxCmd *p = GetSelCmd();
    if (p == NULL)
        return;

    // update the listbox with the shortcuts for the selected menuitem
    m_pBindings->Append(p->GetShortcutsList());

    // select the first shortcut...
    if (m_pBindings->GetCount() > 0)
        m_pBindings->SetSelection(0);
}

void wxKeyConfigPanel::ApplyChanges()
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::ApplyChanges"));
    wxKeyProfile *prof = GetSelProfile();
    wxASSERT(prof);

    // just copy the internal key binder used to allow a sort of
    // "undo" feature into the currently selected profile
    prof->DeepCopy(m_kBinder);
	this->m_bProfileHasBeenModified = false;

    // and update the label of the m_pKeyProfiles control
    // (the name of the profile could have been changed)
	int id = GetSelProfileIdx();
	if ( m_pKeyProfiles->GetString(id) != m_kBinder.GetName() ) {
		m_pKeyProfiles->SetString(id, m_kBinder.GetName());
	}
}

void wxKeyConfigPanel::EnableKeyProfiles(bool bEnable)
{
    m_bEnableKeyProfiles = bEnable;

    // we must hide the keyprofile combobox
    ShowSizer(m_pKeyProfilesSizer, m_bEnableKeyProfiles);
}

void wxKeyConfigPanel::ShowSizer(wxSizer *toshow, bool show)
{
    // first of all, update the items of the sizer
    toshow->ShowItems(m_bEnableKeyProfiles);

    // then, since the size has been changed...
    wxSizer *main = GetSizer();

    // check the old show state...
    // VERY IMPORTANT: this is not only an optimization
    //                 the code below expect a size change and
    //                 if the size change did not happen it will
    //                 set some vars to wrong values
    bool oldshow = main->IsShown(toshow);
    if ((show && oldshow) || (!show && !oldshow))
        return;

    // add or detach the given sizer
    if (show)
        main->Prepend(toshow, 0, wxGROW);
    else
#if wxCHECK_VERSION(2, 5, 1)
        main->Detach(toshow);
#else
        main->Remove(toshow);
#endif


    // THIS PIECE OF CODE HAS BEEN COPIED & PASTED
    // FROM THE wxLogDialog::OnDetails FUNCTION OF
    // THE wxWidgets/src/generic/logg.cpp FILE
    // -------------------------------------------
    m_minHeight = m_maxHeight = -1;

   // wxSizer::FitSize() is private, otherwise we might use it directly...
    wxSize sizeTotal = GetSize(),
           sizeClient = GetClientSize();

    wxSize size = main->GetMinSize();
    size.x += sizeTotal.x - sizeClient.x;
    size.y += sizeTotal.y - sizeClient.y;

    // we don't want to allow expanding the dialog in vertical direction as
    // this would show the "hidden" details but we can resize the dialog
    // vertically while the details are shown
    if ( !show )
        m_maxHeight = size.y;

    SetSizeHints(size.x, size.y, m_maxWidth, m_maxHeight);

    // don't change the width when expanding/collapsing
#if wxCHECK_VERSION(2, 5, 1)
    SetSize(wxDefaultCoord, size.y);
#else
    SetSize(-1, size.y);
#endif

#ifdef __WXGTK__
    // VS: this is neccessary in order to force frame redraw under
    // WindowMaker or fvwm2 (and probably other broken WMs).
    // Otherwise, detailed list wouldn't be displayed.
    Show();
#endif // wxGTK
}


bool wxKeyConfigPanel::HasProfileBeenModifiedOrSelected() const
{
	return this->m_bProfileModifiedOrChanged;
}

void wxKeyConfigPanel::ResetProfileBeenModifiedOrSelected()
{
	this->m_bProfileModifiedOrChanged = false;
}


// ----------------------------------------------------------------------------
// wxKeyConfigPanel - event handlers
// ----------------------------------------------------------------------------

void wxKeyConfigPanel::OnApplyChanges(wxCommandEvent &event)
{
    // apply changed bindings to the original binder
    ApplyChanges();
    event.Skip();       // let parent know that changes were applied
}

void wxKeyConfigPanel::OnProfileEditing(wxCommandEvent &)
{
    wxASSERT(m_nCurrentProf != -1);

    wxString oldname = m_kBinder.GetName();
    wxString newname = m_pKeyProfiles->GetValue();//ev.GetString();
    if (newname == oldname)
        return;

	if ( GetSelProfile()->IsNotEditable() )
	{
        wxMessageBox(wxT("This profile cannot be renamed."),
                    wxT("Warning"));

		m_pKeyProfiles->Select(GetSelProfileIdx());
		return;
	}

    // now the profile has been changed...
    m_bProfileHasBeenModified = TRUE;
	m_bProfileModifiedOrChanged = TRUE;


    // change the name of the current profile
    m_kBinder.SetName(newname);

#ifdef wxKEYBINDER_AUTO_SAVE
	ApplyChanges();
#endif
#if 0
    // and the string of the combobox...
    int n = m_pKeyProfiles->FindString(oldname);
    if (n != wxNOT_FOUND)
        m_pKeyProfiles->SetString(n, newname);
#endif
}

void wxKeyConfigPanel::OnTreeCommandSelected(wxTreeEvent &)
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::OnTreeCommandSelected"));

    // one of the group in the tree ctrl has been selected:
    // the listbox with the keybindings must be synchronized...
    FillInBindings();

    // if the user selected an invalid/valid item in the tree ctrl,
    // the buttons must be disabled/enabled...
    UpdateButtons();

    // ...and the description must be updated
    UpdateDesc();
}

void wxKeyConfigPanel::OnListCommandSelected(wxCommandEvent &)
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::OnListCommandSelected"));

    // like OnTreeCommandSelected
    FillInBindings();
    UpdateButtons();
    UpdateDesc();
}

void wxKeyConfigPanel::OnBindingSelected(wxCommandEvent &)
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::OnBindingSelected"));

    // the remove button should be enabled if the
    // element just selected is valid...
    UpdateButtons();
}

void wxKeyConfigPanel::OnBindingDblClick(wxCommandEvent &)
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::OnBindingDblClick"));

	const int selIdx = m_pBindings->GetSelection();
	if ( selIdx == wxNOT_FOUND )
	{
		return;
	}

	CmdSet cmds = m_kBinder.GetCmdBindsTo(m_pBindings->GetStringSelection());

	assert( cmds.size() > 0 );

	CommandOrderDlg dlg( m_pBindings->GetStringSelection(), cmds, this );
	if ( dlg.ShowModal() == wxID_OK )
	{
		// now the user has modified the currently selected profile...
		m_bProfileHasBeenModified = TRUE;
		m_bProfileModifiedOrChanged = TRUE;

#ifdef wxKEYBINDER_AUTO_SAVE
		ApplyChanges();
#endif
	}
}

void wxKeyConfigPanel::OnCategorySelected(wxCommandEvent &ev)
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::OnCategorySelected"));

    // update selection
    int sel = m_pCategories->GetSelection();
    if (sel == -1) return;

    wxExComboItemData *data = (wxExComboItemData*)m_pCategories->GetClientObject(sel);
    wxArrayString &arr = data->GetCmdNameArr();

    // clear the old elements & insert the new ones
    m_pCommandsList->Clear();
    for (size_t i=0; i < arr.GetCount(); i++)
        m_pCommandsList->Append(arr.Item(i), (void *)data->GetID(i));

    // select the first
    m_pCommandsList->Select(0);
    OnListCommandSelected(ev);
}

void wxKeyConfigPanel::OnProfileSelected(wxCommandEvent &e)
{
    wxKBLogDebug(wxT("wxKeyConfigPanel::OnProfileSelected"));

    int selidx = m_pKeyProfiles->GetSelection();
    wxKeyProfile *sel;

    if (selidx != -1) {

        // did the user modiy the old profile ?
        if (m_bProfileHasBeenModified) {

            // NB: m_nCurrentProf now retains the old profile index
            int choice = wxMessageBox(
                wxString::Format(wxT("The previous profile (named \"%s\") has been modified.\n")
                        wxT("Do you want to save the changes to that profile ?"),
                        GetProfile(m_nCurrentProf)->GetName().c_str()),
                wxT("Warning"), wxYES_NO | wxICON_QUESTION);

            if (choice == wxYES) {

                ApplyChanges();

            } else if (choice == wxCANCEL) {

                // WARNING: the wxCANCEL flag has been removed from the wxMessageBox
                // above because the call to m_pKeyProfiles->SetSelection below
                // provokes (even if wx docs says no) with wxMSW 2.5.4 another event

                // re select the old profile... without generating another event
                m_pKeyProfiles->SetSelection(m_nCurrentProf);
                return;     // and abort this call

            } else if (choice == wxNO) {

                // just restore the original label of the old profile
                // into the wxcombobox...
                // NB: the original label is stored in the client data associated
                // with the previously selected combo item; the user-edited
                // label has been set only in the m_kBinder profile.
                m_pKeyProfiles->SetString(m_nCurrentProf,
                            GetProfile(m_nCurrentProf)->GetName());
            }
        }

        // update the current selected profile index
        m_nCurrentProf = selidx;
        sel = GetProfile(m_nCurrentProf);

    } else {

        // GetSelProfile() will use our current m_nCurrentProf element...
        sel = GetSelProfile();
    }

    if (!sel) return;

    // copy original bindings to the temporary binder,
    // which is the one which the user edits (and which
    // will be copied in the original one if the
    // #ApplyChanges function is called...)
    m_kBinder.DeepCopy(*sel);
    m_bProfileHasBeenModified = FALSE;

	if ( e.GetClientData() != NULL )
	{
		//mark as changed only if event was triggered by actually using the combobox
		//do not trigger on the fake event issued when adding a profile
		m_bProfileModifiedOrChanged = TRUE;
	}

    // call other event handlers
    if (IsUsingTreeCtrl()) {

        wxTreeEvent ev;
        OnTreeCommandSelected(ev);

    } else {

        wxCommandEvent ev;
        OnListCommandSelected(ev);
    }
}

void wxKeyConfigPanel::OnAnyModifier(wxCommandEvent &)
{
	if ( GetSelProfile()->IsNotEditable() )
	{
        wxMessageBox(wxT("This profile cannot be changed."),
                    wxT("Warning"));
		return;
	}

	wxString shortcut = m_pBindings->GetStringSelection();

    wxCmd *sel = GetSelCmd();
	wxKeyBind& kb = *sel->GetShortcut(shortcut);

	wxString newKeyStr = kb.GetStr();
	if ( m_pAnyModCbx->IsChecked() )
	{
		newKeyStr = wxT("Any+") + newKeyStr;
	}
	else
	{
		newKeyStr = newKeyStr.After(wxT('+'));
	}

	kb.Set( newKeyStr, kb.GetOrderIndex() );

    // now the user has modified the currently selected profile...
    m_bProfileHasBeenModified = TRUE;
	m_bProfileModifiedOrChanged = TRUE;

    // and update the list of the key bindings
    FillInBindings();
    m_pKeyField->Clear();

	//select previously selected key
	this->SelectKeyString( newKeyStr ); 

#ifdef wxKEYBINDER_AUTO_SAVE
	ApplyChanges();
#endif
}

void wxKeyConfigPanel::OnAssignKey(wxCommandEvent &)
{
	if ( GetSelProfile()->IsNotEditable() )
	{
        wxMessageBox(wxT("This profile cannot be changed."),
                    wxT("Warning"));
		return;
	}

    // the new key combination should be valid because only when
    // it's valid this button is enabled...
    wxASSERT(m_pKeyField->IsValidKeyComb());
    wxCmd *sel = GetSelCmd();

    if (sel->GetShortcutCount() >= wxCMD_MAX_SHORTCUTS) {

        // sorry...
        wxMessageBox(wxString::Format(wxT("Cannot add more than %d shortcuts ")
                    wxT("to a single command..."), wxCMD_MAX_SHORTCUTS),
                    wxT("Cannot add another shortcut"));
        return;
    }

    // actually add the new shortcut key
    // (if there are already the max. number of shortcuts for
    // this command, the shortcut won't be added).
	const wxString& keystr = m_pKeyField->GetValue();
	sel->AddShortcut(keystr, 1 + this->GetSelProfile()->GetHighestOrderIndex(keystr));

    // now the user has modified the currently selected profile...
    m_bProfileHasBeenModified = TRUE;
	m_bProfileModifiedOrChanged = TRUE;

#ifndef wxKEYBINDER_MULTICMD_PER_KEY
    // if the just added key bind was owned by another command,
    // remove it from the old command...
    if (m_pCurrCmd) {
        wxKeyBind tmp(m_pKeyField->GetValue());
        int n = -1;

#ifdef __WXDEBUG__
		bool bind = m_pCurrCmd->IsBindTo(tmp, &n);
        wxASSERT_MSG(bind, wxT("the m_pCurrCmd variable should be NULL then..."));
#endif      // to avoid warnings in release mode

        m_pCurrCmd->RemoveShortcut(n);
    }
#endif

    // and update the list of the key bindings
    FillInBindings();
    m_pKeyField->Clear();

#ifdef wxKEYBINDER_AUTO_SAVE
	ApplyChanges();
#endif
}

void wxKeyConfigPanel::OnRemoveKey(wxCommandEvent &)
{
	if ( GetSelProfile()->IsNotEditable() )
	{
        wxMessageBox(wxT("This profile cannot be changed."),
                    wxT("Warning"));
		return;
	}

    // now the user has modified the currently selected profile...
    m_bProfileHasBeenModified = TRUE;
	m_bProfileModifiedOrChanged = TRUE;

    // remove the selected shortcut
    GetSelCmd()->RemoveShortcut(m_pBindings->GetSelection());

    // and update the list of the key bindings
    FillInBindings();
    UpdateButtons();

#ifdef wxKEYBINDER_AUTO_SAVE
	ApplyChanges();
#endif
}

void wxKeyConfigPanel::OnRemoveAllKey(wxCommandEvent &)
{
	if ( GetSelProfile()->IsNotEditable() )
	{
        wxMessageBox(wxT("This profile cannot be changed."),
                    wxT("Warning"));
		return;
	}

    // now the user has modified the currently selected profile...
    m_bProfileHasBeenModified = TRUE;
	m_bProfileModifiedOrChanged = TRUE;

    // remove the selected shortcut
    GetSelCmd()->RemoveAllShortcuts();

    // and update the list of the key bindings
    FillInBindings();
    UpdateButtons();

#ifdef wxKEYBINDER_AUTO_SAVE
	ApplyChanges();
#endif
}

void wxKeyConfigPanel::OnAddProfile(wxCommandEvent &)
{
    wxKeyProfile *sel = GetSelProfile();
    if (!sel) return;

    // we'll use one of wx common dialogs
    wxTextEntryDialog dlg(this,
        wxT("Input the name of the new profile.\n")
        wxT("The new profile will be initially set to a copy of the last selected profile."),
        wxT("Add new profile"));
    dlg.SetValue(sel->GetName());

    bool valid = FALSE;
    while (!valid) {

        if (dlg.ShowModal() == wxID_CANCEL)
            return;

        // if the name is the same of one of the existing profiles, we have to abort...
        valid = TRUE;
        for (size_t j=0; j < m_pKeyProfiles->GetCount(); j++)
            valid &= (GetProfile(j)->GetName() != dlg.GetValue());

        if (!valid) {

            wxMessageBox(wxT("The given profile name is already in use.\n")
                        wxT("Enter another name."));
        }
    }

    // create the new profile copying the last selected one
    wxKeyProfile *newprof = new wxKeyProfile(*sel, false, false); //copies of profiles ARE deletable and editable
    newprof->SetName(dlg.GetValue());       // just change its name
    AddProfile(*newprof, true);
    delete newprof;

    // set the new profile as selected (the new profile is the last one)
    SetSelProfile(m_pKeyProfiles->GetCount()-1);
}

void wxKeyConfigPanel::OnRemoveProfile(wxCommandEvent &)
{
    wxASSERT(m_nCurrentProf != -1);

    if (m_pKeyProfiles->GetCount() == 1) {

        wxMessageBox(wxT("Cannot delete this profile. It's the only available profile."),
                    wxT("Warning"));
        return;
    }

    // delete the keyprofile associated with that item...
	wxKeyProfile* pCurProfile = (wxKeyProfile*)m_pKeyProfiles->GetClientData(m_nCurrentProf);

	if ( pCurProfile->IsNotDeletable() ) {
		wxMessageBox(wxT("This profile is not deletable."),
                    wxT("Warning"));
        return;
	}

    delete (pCurProfile);
    m_pKeyProfiles->Delete(m_nCurrentProf);
	m_bProfileModifiedOrChanged = TRUE;

    // update the currently selected profile
    int newsel = m_nCurrentProf-1;
    if (newsel < 0) newsel=0;
    wxASSERT(newsel < (int)m_pKeyProfiles->GetCount());

    // keep sync m_nCurrentProf with the currently really selected item
    SetSelProfile(newsel);
}

void wxKeyConfigPanel::OnKeyPressed(wxCommandEvent &)
{
    // if there were no key combinations in the wxKeyMonitorTextCtrl
    // before this event, maybe that now there is one.... this means
    // that the "Assign" could be enabled...
    UpdateButtons();
}
