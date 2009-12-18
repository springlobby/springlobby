/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/spinctlg.h
// Purpose:     generic wxSpinCtrlDbl class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.10.99
// RCS-ID:      $Id: spinctlg.h 52582 2008-03-17 13:46:31Z VZ $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_SPINCTRL_H_SL
#define _WX_GENERIC_SPINCTRL_H_SL

// ----------------------------------------------------------------------------
// wxSpinCtrlDbl is a combination of wxSpinButton and wxTextCtrl, so if
// wxSpinButton is available, this is what we do - but if it isn't, we still
// define wxSpinCtrlDbl class which then has the same appearance as wxTextCtrl but
// the different interface. This allows to write programs using wxSpinCtrlDbl
// without tons of #ifdefs.
// ----------------------------------------------------------------------------
#include <wx/setup.h>
#if wxUSE_SPINBTN
#include <wx/control.h>
class  wxSpinButton;
class  wxTextCtrl;

// ----------------------------------------------------------------------------
// wxSpinCtrlDbl is a combination of wxTextCtrl and wxSpinButton
// ----------------------------------------------------------------------------
class wxSpinCtrlDblButton;
class  wxSpinCtrlDbl : public wxControl
{
public:
    wxSpinCtrlDbl() { Init(); }

    wxSpinCtrlDbl(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               double min = 0, double max = 100, double initial = 0, double increment = 1.0,
               const wxString& name = _T("wxSpinCtrlDbl"))
    {
        Init();
        Create(parent, id, value, pos, size, style, min, max, initial, increment, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                double min = 0, double max = 100, double initial = 0, double increment = 1.0,
                const wxString& name = _T("wxSpinCtrlDbl"));

    virtual ~wxSpinCtrlDbl();

    // operations
    void SetValue(double val);
    void SetValue(const wxString& text);
    void SetRange(double min, double max);
    void SetSelection(long from, long to);
    void Increment( bool up );

    // accessors
    double GetValue() const;
    double GetMin() const;
    double GetMax() const;

    // implementation from now on

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual bool Reparent(wxWindow *newParent);

    // get the subcontrols
    wxTextCtrl *GetText() const { return m_text; }
    wxSpinButton *GetSpinButton() const { return m_btn; }


    // put the numeric value of the string in the text ctrl doubleo val and return
    // true or return false if the text ctrl doesn't contain a number or if the
    // number is out of range
    bool GetTextValue(double *val) const;

protected:
    // override the base class virtuals involved into geometry calculations
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);
    double m_min,m_max,m_increment,m_current;

    // common part of all ctors
    void Init();

    friend class wxSpinCtrlDblButton;
    // set the value of the text (only)
    void SetTextValue(double val);//putting this here makes sure it's not called fromt he ouside, so we can enable 'special' semantics

private:
    // the subcontrols
    wxTextCtrl *m_text;
    wxSpinButton *m_btn;

private:
    DECLARE_DYNAMIC_CLASS(wxSpinCtrlDbl)
//    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_SPINBTN/!wxUSE_SPINBTN
#include <wx/spinctrl.h> // for EVT_SPINCTRL
#endif // _WX_GENERIC_SPINCTRL_H_

