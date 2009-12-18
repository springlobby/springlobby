///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/spinctlg.cpp
// Purpose:     implements wxSpinCtrlDbl as a composite control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.01.01
// RCS-ID:      $Id: spinctlg.cpp 52582 2008-03-17 13:46:31Z VZ $
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "spinctld.h"

#if wxUSE_SPINCTRL

#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/log.h>
#include <limits>
#include "utils/conversion.h"
#include "utils/math.h"
#include "utils/debug.h"


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin between the text control and the spin
static const wxCoord MARGIN = 2;

// ----------------------------------------------------------------------------
// wxSpinCtrlDblText: text control used by spin control
// ----------------------------------------------------------------------------

class wxSpinCtrlDblText : public wxTextCtrl
{
public:
    wxSpinCtrlDblText(wxSpinCtrlDbl *spin, const wxString& value)
        : wxTextCtrl(spin->GetParent(), wxID_ANY, value)
    {
        m_spin = spin;

        // remove the default minsize, the spinctrl will have one instead
        SetSizeHints(wxDefaultCoord,wxDefaultCoord);
    }

protected:
    void OnTextChange(wxCommandEvent& event)
    {
        double val;
        if ( m_spin->GetTextValue(&val) )
        {
            m_spin->GetSpinButton()->SetValue(val);
        }

        event.Skip();
    }

    bool ProcessEvent(wxEvent &event)
    {
        // Hand button down events to wxSpinCtrlDbl. Doesn't work.
        if (event.GetEventType() == wxEVT_LEFT_DOWN && m_spin->ProcessEvent( event ))
            return true;

        return wxTextCtrl::ProcessEvent( event );
    }

private:
    wxSpinCtrlDbl *m_spin;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSpinCtrlDblText, wxTextCtrl)
    EVT_TEXT(wxID_ANY, wxSpinCtrlDblText::OnTextChange)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxSpinCtrlDblButton: spin button used by spin control
// ----------------------------------------------------------------------------

class wxSpinCtrlDblButton : public wxSpinButton
{
public:
    wxSpinCtrlDblButton(wxSpinCtrlDbl *spin, int style )
        : wxSpinButton(spin->GetParent())
    {
        m_spin = spin;

        SetWindowStyle(style | wxSP_VERTICAL);

        // remove the default minsize, the spinctrl will have one instead
        SetSizeHints(wxDefaultCoord,wxDefaultCoord);
    }

protected:
    void OnSpinButton(wxSpinEvent& eventSpin) {
        eventSpin.Veto();
    }
    void OnSpinButtonUp(wxSpinEvent& eventSpin)
    {
        m_spin->Increment( true );
        //DO NOT let these events skip or value WILL NOT change
        eventSpin.Veto();
    }
    void OnSpinButtonDown(wxSpinEvent& eventSpin)
    {
        m_spin->Increment( false );
        eventSpin.Veto();
    }

private:
    wxSpinCtrlDbl *m_spin;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSpinCtrlDblButton, wxSpinButton)
    EVT_SPIN(wxID_ANY, wxSpinCtrlDblButton::OnSpinButton) //need to implement this to prevent doupicate increment
    EVT_SPIN_UP(wxID_ANY, wxSpinCtrlDblButton::OnSpinButtonUp)
    EVT_SPIN_DOWN(wxID_ANY, wxSpinCtrlDblButton::OnSpinButtonDown)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================
IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrlDbl, wxControl)
// ----------------------------------------------------------------------------
// wxSpinCtrlDbl creation
// ----------------------------------------------------------------------------
//BEGIN_EVENT_TABLE(wxSpinCtrlDbl,wxControl)
//    EVT_SPIN_UP   ( wxID_ANY, wxSpinCtrlDbl::OnSpinUp    )
//    EVT_SPIN_DOWN ( wxID_ANY, wxSpinCtrlDbl::OnSpinDown  )
////    EVT_TEXT_ENTER( wxID_ANY, wxSpinCtrlDbl::OnTextEnter )
////    EVT_TEXT      ( wxID_ANY, wxSpinCtrlDbl::OnText      )
////    EVT_SET_FOCUS ( wxSpinCtrlDbl::OnFocus     )
////    EVT_KILL_FOCUS( wxSpinCtrlDbl::OnKillFocus )
//END_EVENT_TABLE()

void wxSpinCtrlDbl::Init()
{
    m_text = NULL;
    m_btn = NULL;
}

bool wxSpinCtrlDbl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        double min,
                        double max,
                        double initial,
                        double increment,
                        const wxString& name)
{
    m_min = min;
    m_max = max;
    m_increment = increment;
    m_current = initial;
    if ( !wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    // the string value overrides the numeric one (for backwards compatibility
    // reasons and also because it is simpler to satisfy the string value which
    // comes much sooner in the list of arguments and leave the initial
    // parameter unspecified)
    if ( !value.empty() )
    {
        long l;
        if ( value.ToLong(&l) )
            initial = l;
    }

    m_text = new wxSpinCtrlDblText(this, value);
    m_btn = new wxSpinCtrlDblButton(this, style);

    m_btn->SetRange(min, max);
    m_btn->SetValue(initial);
    SetTextValue( initial );
    SetInitialSize(size);
    Move(pos);

    // have to disable this window to avoid interfering it with message
    // processing to the text and the button... but pretend it is enabled to
    // make IsEnabled() return true
    wxControl::Enable(false); // don't use non virtual Disable() here!
    m_isEnabled = true;

    // we don't even need to show this window itself - and not doing it avoids
    // that it overwrites the text control
    wxControl::Show(false);
    m_isShown = true;
    return true;
}

wxSpinCtrlDbl::~wxSpinCtrlDbl()
{
    // delete the controls now, don't leave them alive even though they would
    // still be eventually deleted by our parent - but it will be too late, the
    // user code expects them to be gone now
    delete m_text;
    m_text = NULL ;
    delete m_btn;
    m_btn = NULL ;
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize wxSpinCtrlDbl::DoGetBestSize() const
{
    wxSize sizeBtn = m_btn->GetBestSize(),
           sizeText = m_text->GetBestSize();

    return wxSize(sizeBtn.x + sizeText.x + MARGIN, sizeText.y);
}

void wxSpinCtrlDbl::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x, y, width, height);

    // position the subcontrols inside the client area
    wxSize sizeBtn = m_btn->GetSize();

    wxCoord wText = width - sizeBtn.x;
    m_text->SetSize(x, y, wText, height);
    m_btn->SetSize(x + wText + MARGIN, y, wxDefaultCoord, height);
}

// ----------------------------------------------------------------------------
// operations forwarded to the subcontrols
// ----------------------------------------------------------------------------

bool wxSpinCtrlDbl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return false;

    m_btn->Enable(enable);
    m_text->Enable(enable);

    return true;
}

bool wxSpinCtrlDbl::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return false;

    // under GTK Show() is called the first time before we are fully
    // constructed
    if ( m_btn )
    {
        m_btn->Show(show);
        m_text->Show(show);
    }

    return true;
}

bool wxSpinCtrlDbl::Reparent(wxWindow *newParent)
{
    if ( m_btn )
    {
        m_btn->Reparent(newParent);
        m_text->Reparent(newParent);
    }

    return true;
}

// ----------------------------------------------------------------------------
// value and range access
// ----------------------------------------------------------------------------

bool wxSpinCtrlDbl::GetTextValue(double  *val) const
{
    double l;
    if ( !m_text->GetValue().ToDouble(&l) )
    {
        // not a number at all
        return false;
    }

    if ( l < GetMin() || l > GetMax() )
    {
        // out of range
        return false;
    }

    *val = l;

    return true;
}

double wxSpinCtrlDbl::GetValue() const
{
    return m_current;
}

double wxSpinCtrlDbl::GetMin() const
{
    return m_min;
}

double wxSpinCtrlDbl::GetMax() const
{
    return m_max;
}

// ----------------------------------------------------------------------------
// changing value and range
// ----------------------------------------------------------------------------

void wxSpinCtrlDbl::SetTextValue(double val)
{
    wxCHECK_RET( m_text, _T("invalid call to wxSpinCtrlDbl::SetTextValue") );
    m_text->SetValue(wxString::Format(_T("%.1f"), val));

    // select all text
    m_text->SetSelection(0, -1);

    // and give focus to the control!
    // m_text->SetFocus();    Why???? TODO.

#ifdef __WXCOCOA__
    /*  It's sort of a hack to do this from here but the idea is that if the
        user has clicked on us, which is the main reason this method is called,
        then focus probably ought to go to the text control since clicking on
        a text control usually gives it focus.

        However, if the focus is already on us (i.e. the user has turned on
        the ability to tab to controls) then we don't want to drop focus.
        So we only set focus if we would steal it away from a different
        control, not if we would steal it away from ourself.
     */
    wxWindow *currentFocusedWindow = wxWindow::FindFocus();
    if(currentFocusedWindow != this && currentFocusedWindow != m_text)
        m_text->SetFocus();
#endif
}

void wxSpinCtrlDbl::SetValue(double val)
{
    wxCHECK_RET( m_btn, _T("invalid call to wxSpinCtrlDbl::SetValue") );

    if ( val < m_min || val > m_max )
        return;
    wxLogMessage( _T("wxSpinCtrlDbl::SetValue") );
    m_current = clamp<double> ( val, m_min, m_max );
    //remove this and fear the apocalypse! once a ctrl hits  val with full double representation --> kaboom
    m_current *= 1.001;
    SetTextValue(m_current);


    m_btn->SetValue(m_current);
}

void wxSpinCtrlDbl::SetValue(const wxString& text)
{
    wxCHECK_RET( m_text, _T("invalid call to wxSpinCtrlDbl::SetValue") );

    double val;
    if ( text.ToDouble(&val)
            &&  ((val > std::numeric_limits<double>::min() )
            && (val < std::numeric_limits<double>::max() )) )
    {
        SetValue(val);
    }
    else // not a number at all or out of range
    {
        m_text->SetValue(text);
        m_text->SetSelection(0, -1);
    }
}

void wxSpinCtrlDbl::SetRange(double  min, double  max)
{
    wxCHECK_RET( m_btn, _T("invalid call to wxSpinCtrlDbl::SetRange") );

    m_btn->SetRange(min, max);
}

void wxSpinCtrlDbl::Increment(bool up)
{
    double new_val = up ? m_current + m_increment : m_current - m_increment;
    SetValue( new_val );
}



#endif // wxUSE_SPINCTRL

