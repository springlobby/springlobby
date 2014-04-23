#ifndef SPRINGLOBBY_HEADERGUARD_SPINCTRLDOUBLE_H
#define SPRINGLOBBY_HEADERGUARD_SPINCTRLDOUBLE_H

//-----------------------------------------------------------------------------
// SlSpinCtrlDouble
//-----------------------------------------------------------------------------

#include "../spinctrl.h"

class  SlSpinCtrlDouble : public SlSpinCtrlGenericBase
{
public:
    SlSpinCtrlDouble() { }
    SlSpinCtrlDouble(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                     double min = 0, double max = 100, double initial = 0,
                     double inc = 1,
                     const wxString& name = wxT("SlSpinCtrlDouble"))
    {
        Create(parent, id, value, pos, size, style, min, max, initial, inc, name);
    }

	// accessors
    double GetValue(wxSPINCTRL_GETVALUE_FIX) const { return DoGetValue(); }
    double GetMin() const { return m_min; }
    double GetMax() const { return m_max; }
    double GetIncrement() const { return m_increment; }

    void SetValue(double value)                 { DoSetValue(value); }
    void SetRange(double minVal, double maxVal) { DoSetRange(minVal, maxVal); }
    void SetIncrement(double inc)               { DoSetIncrement(inc); }
    void SetDigits(unsigned digits);

protected:
    virtual void DoSendEvent();

};

#endif
