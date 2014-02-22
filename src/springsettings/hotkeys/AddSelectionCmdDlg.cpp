/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "AddSelectionCmdDlg.h"
#include "../../utils/customdialogs.h"


AddSelectionCmdDlg::AddSelectionCmdDlg( wxWindow* parent ) : AddSelectionCmdDlgBase( parent )
{
}


wxString AddSelectionCmdDlg::validateInputFields()
{
	long dummy = 0;
	if ( this->m_radioBtnSrcMouse->GetValue() )
	{
		if ( this->m_textCtrlSrcMouseDist->GetLabel().ToLong( &dummy ) == false || dummy < 0 )
		{
			return _("Invalid value for distance from mouse!");
		}
	}

	if ( this->m_radioBtnSelCount->GetValue() )
	{
		if ( this->m_textCtrlSelCount->GetLabel().ToLong( &dummy ) == false || dummy < 0 )
		{
			return _("Invalid value for selection count!");
		}
	}
	else if ( this->m_radioBtnSelPerc->GetValue() )
	{
		if ( this->m_textCtrlSelPerc->GetLabel().ToLong( &dummy ) == false || dummy < 0 || dummy > 100 )
		{
			return _("Invalid value for selection percentage!");
		}
	}

	if ( this->m_textCtrlAbsHealth->IsEnabled() )
	{
		if ( this->m_textCtrlAbsHealth->GetLabel().ToLong( &dummy ) == false || dummy < 0 )
		{
			return _("Invalid value for absolute health!");
		}
	}

	if ( this->m_textCtrlRelHealth->IsEnabled() )
	{
		if ( this->m_textCtrlRelHealth->GetLabel().ToLong( &dummy ) == false || dummy < 0 )
		{
			return _("Invalid value for relative health!");
		}
	}

	if ( this->m_textCtrlWeapRange->IsEnabled() )
	{
		if ( this->m_textCtrlWeapRange->GetLabel().ToLong( &dummy ) == false || dummy < 0 )
		{
			return _("Invalid value for weapon range!");
		}
	}

	if ( this->m_textCtrlAbsHealth->IsEnabled() )
	{
		if ( this->m_textCtrlAbsHealth->GetLabel().ToLong( &dummy ) == false || dummy < 0 )
		{
			return _("Invalid value for absolute health!");
		}
	}

	return wxEmptyString;
}

wxString AddSelectionCmdDlg::getSourceString() const
{
	wxString str;
	if ( this->m_radioBtnSrcMap->GetValue() )
	{
		str += wxT("AllMap+");
	}
	else if ( this->m_radioBtnSrcView->GetValue() )
	{
		str += wxT("Visible+");
	}
	else if ( this->m_radioBtnSrcSel->GetValue() )
	{
		str += wxT("PrevSelection+");
	}
	else if ( this->m_radioBtnSrcMouse->GetValue() )
	{
		if ( this->m_checkBoxSrcDistCyl->GetValue() )
		{
			str += wxT("FromMouseC_");
		}
		else
		{
			str += wxT("FromMouse_");
		}

		str += this->m_textCtrlSrcMouseDist->GetLabel();
		str += wxT("+");
	}

	return str;
}

wxString AddSelectionCmdDlg::getConditionsAndSelectString() const
{
	wxString str;

	//Conditions
	if ( this->m_checkBoxBuilderInc->GetValue() )
	{
		str += wxT("_Builder");
	}
	else if ( this->m_checkBoxBuilderEx->GetValue() )
	{
		str += wxT("_Not_Builder");
	}

	if ( this->m_checkBoxBuildingInc->GetValue() )
	{
		str += wxT("_Building");
	}
	else if ( this->m_checkBoxBuildingEx->GetValue() )
	{
		str += wxT("_Not_Building");
	}

	if ( this->m_checkBoxComInc->GetValue() )
	{
		str += wxT("_ManualFireUnit");
	}
	else if ( this->m_checkBoxComEx->GetValue() )
	{
		str += wxT("_Not_ManualFireUnit");
	}

	if ( this->m_checkBoxTransInc->GetValue() )
	{
		str += wxT("_Transport");
	}
	else if ( this->m_checkBoxTransEx->GetValue() )
	{
		str += wxT("_Not_Transport");
	}

	if ( this->m_checkBoxAirInc->GetValue() )
	{
		str += wxT("_Aircraft");
	}
	else if ( this->m_checkBoxAirEx->GetValue() )
	{
		str += wxT("_Not_Aircraft");
	}

	if ( this->m_checkBoxWeaponsInc->GetValue() )
	{
		str += wxT("_Weapons");
	}
	else if ( this->m_checkBoxWeaponsEx->GetValue() )
	{
		str += wxT("_Not_Weapons");
	}

	if ( this->m_checkBoxWeapRangeEx->GetValue() )
	{
		str += wxT("_Not");
	}
	if ( this->m_checkBoxWeapRangeInc->GetValue() || this->m_checkBoxWeapRangeEx->GetValue() )
	{
		str += wxT("_WeaponRange_");
		str += this->m_textCtrlWeapRange->GetLabel();
	}

	if ( this->m_checkBoxAbsHealthEx->GetValue() )
	{
		str += wxT("_Not");
	}
	if ( this->m_checkBoxAbsHealthInc->GetValue() || this->m_checkBoxAbsHealthEx->GetValue() )
	{
		str += wxT("_AbsoluteHealth_");
		str += this->m_textCtrlAbsHealth->GetLabel();
	}

	if ( this->m_checkBoxRelHealthEx->GetValue() )
	{
		str += wxT("_Not");
	}
	if ( this->m_checkBoxRelHealthInc->GetValue() || this->m_checkBoxRelHealthEx->GetValue() )
	{
		str += wxT("_RelativeHealth_");
		str += this->m_textCtrlRelHealth->GetLabel();
	}

	if ( this->m_checkBoxNameEx->GetValue() )
	{
		str += wxT("_Not");
	}
	if ( this->m_checkBoxNameInc->GetValue() || this->m_checkBoxNameEx->GetValue() )
	{
		str += wxT("_NameContain_");
		str += this->m_textCtrlName->GetLabel();
	}

	if ( this->m_checkBoxCategoryEx->GetValue() )
	{
		str += wxT("_Not");
	}
	if ( this->m_checkBoxCategoryInc->GetValue() || this->m_checkBoxCategoryEx->GetValue() )
	{
		str += wxT("_Category_");
		str += this->m_textCtrlCategory->GetLabel();
	}

	if ( this->m_checkBoxTypeSelInc->GetValue() )
	{
		str += wxT("_InPrevSel");
	}
	else if ( this->m_checkBoxTypeSelEx->GetValue() )
	{
		str += wxT("_Not_InPrevSel");
	}

	if ( this->m_checkBoxIdleInc->GetValue() )
	{
		str += wxT("_Idle");
	}
	else if ( this->m_checkBoxIdleEx->GetValue() )
	{
		str += wxT("_Not_Idle");
	}

	if ( this->m_checkBoxRadarInc->GetValue() )
	{
		str += wxT("_Radar");
	}
	else if ( this->m_checkBoxRadarEx->GetValue() )
	{
		str += wxT("_Not_Radar");
	}

	if ( this->m_checkBoxHotkeyInc->GetValue() )
	{
		str += wxT("_InHotkeyGroup");
	}
	else if ( this->m_checkBoxHotkeyEx->GetValue() )
	{
		str += wxT("_Not_InHotkeyGroup");
	}

	if ( this->m_checkBoxWaitingInc->GetValue() )
	{
		str += wxT("_Waiting");
	}
	else if ( this->m_checkBoxWaitingEx->GetValue() )
	{
		str += wxT("_Not_Waiting");
	}

	str += wxT("+");

	if ( this->m_checkBoxClearPrevSel->GetValue() )
	{
		str += wxT("_ClearSelection");
	}

	//Source filter
	if ( this->m_radioBtnSelAll->GetValue() )
	{
		str += wxT("_SelectAll");
	}
	else if ( this->m_radioBtnSelOne->GetValue() )
	{
		str += wxT("_SelectOne");
	}
	else if ( this->m_radioBtnSelCount->GetValue() )
	{
		str += wxT("_SelectNum_");
		str += this->m_textCtrlSelCount->GetLabel();
	}
	else if ( this->m_radioBtnSelPerc->GetValue() )
	{
		str += wxT("_SelectPart_");
		str += this->m_textCtrlSelPerc->GetLabel();
	}
	str += wxT("+");

	return str;
}

wxString AddSelectionCmdDlg::getCommandString() const
{
	return this->getSourceString() + this->getConditionsAndSelectString();
}

void AddSelectionCmdDlg::OnButtonAddClick( wxCommandEvent& /*event*/ )
{
	const wxString valResult = this->validateInputFields();

	if ( valResult.size() == 0 )
	{
		this->EndModal( wxID_OK );
	}
	else
	{
		customMessageBox( SS_MAIN_ICON, valResult, _("Invalid input"), wxOK | wxICON_HAND  );
	}
}

void AddSelectionCmdDlg::controlCheckBoxEnable( wxCheckBox* pCheckbClicked, wxCheckBox* pCheckbSibling, wxTextCtrl* pTextCtrl )
{
	if ( pCheckbClicked->GetValue() )
	{
		pCheckbSibling->SetValue( false );

		if ( pTextCtrl )
		{
			pTextCtrl->Enable( true );
		}
	}
	else if ( pTextCtrl && !pCheckbSibling->GetValue() )
	{
		pTextCtrl->Enable( false );
	}
}

void AddSelectionCmdDlg::OnRadioBtnSrcMapClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSrcMouseDist->Enable( false );
	this->m_checkBoxSrcDistCyl->Enable( false );
}

void AddSelectionCmdDlg::OnRadioBtnSrcViewClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSrcMouseDist->Enable( false );
	this->m_checkBoxSrcDistCyl->Enable( false );
}

void AddSelectionCmdDlg::OnRadioBtnSrcSelClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSrcMouseDist->Enable( false );
	this->m_checkBoxSrcDistCyl->Enable( false );
}

void AddSelectionCmdDlg::OnRadioBtnSrcDistClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSrcMouseDist->Enable( true );
	this->m_checkBoxSrcDistCyl->Enable( true );
}

void AddSelectionCmdDlg::OnRadioBtnSelAllClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSelCount->Enable( false );
	this->m_textCtrlSelPerc->Enable( false );
}

void AddSelectionCmdDlg::OnRadioBtnSelCountClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSelCount->Enable( true );
	this->m_textCtrlSelPerc->Enable( false );
}

void AddSelectionCmdDlg::OnRadioBtnSelOneClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSelCount->Enable( false );
	this->m_textCtrlSelPerc->Enable( false );
}

void AddSelectionCmdDlg::OnRadioBtnSelPercClick( wxCommandEvent& /*event*/ )
{
	this->m_textCtrlSelCount->Enable( false );
	this->m_textCtrlSelPerc->Enable( true );
}

void AddSelectionCmdDlg::OnCheckBoxCondRelHealthIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxRelHealthInc, this->m_checkBoxRelHealthEx, m_textCtrlRelHealth );
}

void AddSelectionCmdDlg::OnCheckBoxCondRelHealthExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxRelHealthEx, this->m_checkBoxRelHealthInc, m_textCtrlRelHealth );
}

void AddSelectionCmdDlg::OnCheckBoxCondWeapRangeIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxWeapRangeInc, this->m_checkBoxWeapRangeEx, m_textCtrlWeapRange );
}

void AddSelectionCmdDlg::OnCheckBoxCondWeapRangeExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxWeapRangeEx, this->m_checkBoxWeapRangeInc, m_textCtrlWeapRange );
}

void AddSelectionCmdDlg::OnCheckBoxCondAbsHealthIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxAbsHealthInc, this->m_checkBoxAbsHealthEx, m_textCtrlAbsHealth );
}

void AddSelectionCmdDlg::OnCheckBoxCondAbsHealthExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxAbsHealthEx, this->m_checkBoxAbsHealthInc, m_textCtrlAbsHealth );
}

void AddSelectionCmdDlg::OnCheckBoxCondNameIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxNameInc, this->m_checkBoxNameEx, m_textCtrlName );
}

void AddSelectionCmdDlg::OnCheckBoxCondNameExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxNameEx, this->m_checkBoxNameInc, m_textCtrlName );
}

void AddSelectionCmdDlg::OnCheckBoxCondCategoryIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxCategoryInc, this->m_checkBoxCategoryEx, m_textCtrlCategory );
}

void AddSelectionCmdDlg::OnCheckBoxCondCategoryExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxCategoryEx, this->m_checkBoxCategoryInc, m_textCtrlCategory );
}

void AddSelectionCmdDlg::OnCheckBoxCondBuilderIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxBuilderInc, this->m_checkBoxBuilderEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondBuilderExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxBuilderEx, this->m_checkBoxBuilderInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondBuildingIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxBuildingInc, this->m_checkBoxBuildingEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondBuildingExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxBuildingEx, this->m_checkBoxBuildingInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondComIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxComInc, this->m_checkBoxComEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondComExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxComEx, this->m_checkBoxComInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondTypeSelIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxTypeSelInc, this->m_checkBoxTypeSelEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondTypeSelExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxTypeSelEx, this->m_checkBoxTypeSelInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondAirIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxAirInc, this->m_checkBoxAirEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondAirExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxAirEx, this->m_checkBoxAirInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondWeaponsIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxWeaponsInc, this->m_checkBoxWeaponsEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondWeaponsExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxWeaponsEx, this->m_checkBoxWeaponsInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondIdleIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxIdleInc, this->m_checkBoxIdleEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondIdleExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxIdleEx, this->m_checkBoxIdleInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondRadarIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxRadarInc, this->m_checkBoxRadarEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondRadarExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxRadarEx, this->m_checkBoxRadarInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondTransIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxTransInc, this->m_checkBoxTransEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondTransExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxTransEx, this->m_checkBoxTransInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondHotkeyIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxHotkeyInc, this->m_checkBoxHotkeyEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondHotkeyExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxHotkeyEx, this->m_checkBoxHotkeyInc, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondWaitingIncClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxWaitingInc, this->m_checkBoxWaitingEx, NULL );
}

void AddSelectionCmdDlg::OnCheckBoxCondWaitingExClick( wxCommandEvent& /*event*/ )
{
	AddSelectionCmdDlg::controlCheckBoxEnable( this->m_checkBoxWaitingEx, this->m_checkBoxWaitingInc, NULL );
}
