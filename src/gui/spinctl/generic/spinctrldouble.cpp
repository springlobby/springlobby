//-----------------------------------------------------------------------------
// SlSpinCtrlDouble
//-----------------------------------------------------------------------------

//IMPLEMENT_DYNAMIC_CLASS(SlSpinCtrlDouble, SlSpinCtrlGenericBase)
#include "spinctrldouble.h"
#include "../../../utils/debug.h"
#include <wx/log.h>


void SlSpinCtrlDouble::DoSendEvent()
{
    SlSpinDoubleEvent event( SLEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, GetId());
    event.SetEventObject( this );
    event.SetValue(m_value);
    event.SetString(m_textCtrl->GetValue());

	m_parent_instance->OnSpinCtrlDoubleChange( event );
//    if ( !GetEventHandler()->ProcessEvent( event ) )
//        wxLogError( _T("BULLSHIT") );
}

void SlSpinCtrlDouble::SetDigits(unsigned digits)
{
    wxCHECK_RET( digits <= 20, "too many digits for SlSpinCtrlDouble" );
    m_format.Printf(wxT("%%0.%ulf"), digits);
    DoSetValue(m_value);
}


