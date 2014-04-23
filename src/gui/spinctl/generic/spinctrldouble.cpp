//-----------------------------------------------------------------------------
// SlSpinCtrlDouble
//-----------------------------------------------------------------------------

#include "spinctrldouble.h"
#include <wx/log.h>

DEFINE_EVENT_TYPE(SLEVT_COMMAND_SPINCTRLDOUBLE_UPDATED);

void SlSpinCtrlDouble::DoSendEvent()
{
    SlSpinDoubleEvent event(SLEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, GetId(), m_value);
    event.SetEventObject(this);
    event.SetString(m_textCtrl->GetValue());
    if ( !GetEventHandler()->ProcessEvent( event ) ) {
        wxLogError( _T("Error handling SlSpinCtrlDouble") );
	}
}

void SlSpinCtrlDouble::SetDigits(unsigned digits)
{
    wxCHECK_RET( digits <= 20, _T("too many digits for SlSpinCtrlDouble") );
    m_format.Printf(wxT("%%0.%ulf"), digits);
    DoSetValue(m_value);
}
