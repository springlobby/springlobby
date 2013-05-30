/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/spinctlg.h
// Purpose:     generic SlSpinCtrl class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.10.99
// RCS-ID:      $Id: spinctlg.h 62586 2009-11-10 17:36:53Z PC $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef SL_WX_GENERIC_SPINCTRL_H_
#define SL_WX_GENERIC_SPINCTRL_H_

// ----------------------------------------------------------------------------
// SlSpinCtrl is a combination of SlSpinButton and wxTextCtrl, so if
// SlSpinButton is available, this is what we do - but if it isn't, we still
// define SlSpinCtrl class which then has the same appearance as wxTextCtrl but
// the different interface. This allows to write programs using SlSpinCtrl
// without tons of #ifdefs.
// ----------------------------------------------------------------------------

#if wxUSE_SPINBTN

class SlSpinCtrlGenericBase;

#include <wx/textctrl.h>
class SlSpinCtrlTextGeneric : public wxTextCtrl
{
public:
    SlSpinCtrlTextGeneric(SlSpinCtrlGenericBase *spin, const wxString& value, long style=0);

    virtual ~SlSpinCtrlTextGeneric();

    void OnTextEnter(wxCommandEvent& event);
    void OnChar( wxKeyEvent &event );
    void OnKillFocus(wxFocusEvent& event);
  SlSpinCtrlGenericBase *m_spin;

protected:
  int overflow(int i) { return wxTextCtrl::overflow(i); }

private:
    DECLARE_EVENT_TABLE()
};


// The !wxUSE_SPINBTN version's GetValue() function conflicts with the
// wxTextCtrl's GetValue() and so you have to input a dummy int value.
#define wxSPINCTRL_GETVALUE_FIX

// ----------------------------------------------------------------------------
// SlSpinCtrlGeneric is a combination of wxTextCtrl and SlSpinButton
//
// This class manages a double valued generic spinctrl through the DoGet/SetXXX
// functions that are made public as Get/SetXXX functions for int or double
// for the SlSpinCtrl and SlSpinCtrlDouble classes respectively to avoid
// function ambiguity.
// ----------------------------------------------------------------------------
#include <gui/spinctl/spinctrl.h>

class  SlSpinCtrlGenericBase : public SlSpinCtrlBase
{
public:
    SlSpinCtrlGenericBase() { Init(); }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                double min = 0, double max = 100, double initial = 0,
                double inc = 1,
                const wxString& name = wxT("SlSpinCtrl"));

    virtual ~SlSpinCtrlGenericBase();

    // accessors
    // T GetValue() const
    // T GetMin() const
    // T GetMax() const
    // T GetIncrement() const
    virtual bool GetSnapToTicks() const { return m_snap_to_ticks; }
    // unsigned GetDigits() const                   - SlSpinCtrlDouble only

    // operations
    virtual void SetValue(const wxString& text);
    // void SetValue(T val)
    // void SetRange(T minVal, T maxVal)
    // void SetIncrement(T inc)
    virtual void SetSnapToTicks(bool snap_to_ticks);
    // void SetDigits(unsigned digits)              - SlSpinCtrlDouble only

    // Select text in the textctrl
    void SetSelection(long from, long to);

    // implementation from now on

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual bool Reparent(wxWindowBase *newParent);

    // get the subcontrols
    wxTextCtrl   *GetText() const       { return m_textCtrl; }
    wxSpinButton *GetSpinButton() const { return m_spinButton; }

    // forwarded events from children windows
    void OnSpinButton(wxSpinEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnTextChar(wxKeyEvent& event);

    friend class SlSpinCtrlTextGeneric;

protected:
    // override the base class virtuals involved into geometry calculations
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    // generic double valued functions
    double DoGetValue() const { return m_value; }
    bool DoSetValue(double val);
    void DoSetRange(double min_val, double max_val);
    void DoSetIncrement(double inc);

    // Ensure that the textctrl shows correct value
    void SyncSpinToText();

    // Send the correct event type
    virtual void DoSendEvent() = 0;

    // check if the value is in range
    bool InRange(double n) const { return (n >= m_min) && (n <= m_max); }

    // ensure that the value is in range wrapping it round if necessary
    double AdjustToFitInRange(double value) const;


    double m_value;
    double m_min;
    double m_max;
    double m_increment;
    bool   m_snap_to_ticks;
    wxString m_format;

    int m_spin_value;

    // the subcontrols
    wxTextCtrl   *m_textCtrl;
    wxSpinButton *m_spinButton;

private:
    // common part of all ctors
    void Init();
};

#else // !wxUSE_SPINBTN

#define wxSPINCTRL_GETVALUE_FIX int = 1

// ----------------------------------------------------------------------------
// SlSpinCtrl is just a text control
// ----------------------------------------------------------------------------

#include "wx/textctrl.h"

class  SlSpinCtrlGenericBase : public wxTextCtrl
{
public:
    SlSpinCtrlGenericBase() : m_value(0), m_min(0), m_max(100),
                              m_increment(1), m_snap_to_ticks(false),
                              m_format(wxT("%g")) { }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                double min = 0, double max = 100, double initial = 0,
                double inc = 1,
                const wxString& name = wxT("SlSpinCtrl"))
    {
        m_min = min;
        m_max = max;
        m_value = initial;
        m_increment = inc;

        bool ok = wxTextCtrl::Create(parent, id, value, pos, size, style,
                                     wxDefaultValidator, name);
        DoSetValue(initial);

        return ok;
    }

    // accessors
    // T GetValue() const
    // T GetMin() const
    // T GetMax() const
    // T GetIncrement() const
    virtual bool GetSnapToTicks() const { return m_snap_to_ticks; }
    // unsigned GetDigits() const                   - SlSpinCtrlDouble only

    // operations
    virtual void SetValue(const wxString& text) { wxTextCtrl::SetValue(text); }
    // void SetValue(T val)
    // void SetRange(T minVal, T maxVal)
    // void SetIncrement(T inc)
    virtual void SetSnapToTicks(bool snap_to_ticks)
        { m_snap_to_ticks = snap_to_ticks; }
    // void SetDigits(unsigned digits)              - SlSpinCtrlDouble only

    // Select text in the textctrl
    //void SetSelection(long from, long to);

protected:
    int overflow(int i) { return wxTextCtrl::overflow(i); }

    // generic double valued
    double DoGetValue() const
    {
        double n;
        if ( (wxSscanf(wxTextCtrl::GetValue(), wxT("%lf"), &n) != 1) )
            n = INT_MIN;

        return n;
    }

    bool DoSetValue(double val)
    {
        wxTextCtrl::SetValue(wxFormat(m_format.c_str(), val));
        return true;
    }
    void DoSetRange(double min_val, double max_val)
    {
        m_min = min_val;
        m_max = max_val;
    }
    void DoSetIncrement(double inc) { m_increment = inc; } // Note: unused

    double m_value;
    double m_min;
    double m_max;
    double m_increment;
    bool   m_snap_to_ticks;
    wxString m_format;
};

#endif // wxUSE_SPINBTN/!wxUSE_SPINBTN

#endif // _WX_GENERIC_SPINCTRL_H_
