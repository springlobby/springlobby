#include "AddSelectionCmdDlg.h"
#include "../../utils/customdialogs.h"


AddSelectionCmdDlg::AddSelectionCmdDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) :
											AddSelectionCmdDlgBase( parent, id, title, pos, size, style )
{
}

wxString AddSelectionCmdDlg::validateInputFields()
{
	long dummy = 0;
	if ( this->m_radioBtnSrcMouse->GetValue() )
	{
		if ( this->m_textCtrlSrcMouseDist->GetLabel().ToLong( &dummy ) == false || dummy < 0 )
		{
			return _("Invalid distance from mouse entered!");
		}
	}
	//TODO: More

	return wxT("");
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
		str += wxT("FromMouse_");
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

	/*
	if(((CButton*)GetDlgItem(IDC_CHECK5))->GetCheck())
		datastore.curString+="_Commander";
	if(((CButton*)GetDlgItem(IDC_CHECK6))->GetCheck())
		datastore.curString+="_Not_Commander";

	if(((CButton*)GetDlgItem(IDC_CHECK7))->GetCheck())
		datastore.curString+="_Transport";
	if(((CButton*)GetDlgItem(IDC_CHECK8))->GetCheck())
		datastore.curString+="_Not_Transport";

	if(((CButton*)GetDlgItem(IDC_CHECK9))->GetCheck())
		datastore.curString+="_Aircraft";
	if(((CButton*)GetDlgItem(IDC_CHECK10))->GetCheck())
		datastore.curString+="_Not_Aircraft";

	if(((CButton*)GetDlgItem(IDC_CHECK11))->GetCheck())
		datastore.curString+="_Weapons";
	if(((CButton*)GetDlgItem(IDC_CHECK12))->GetCheck())
		datastore.curString+="_Not_Weapons";

	if(((CButton*)GetDlgItem(IDC_CHECK14))->GetCheck())
		datastore.curString+="_Not";
	if(((CButton*)GetDlgItem(IDC_CHECK13))->GetCheck() || ((CButton*)GetDlgItem(IDC_CHECK14))->GetCheck()){
		datastore.curString+="_WeaponRange_";
		((CEdit*)GetDlgItem(IDC_EDIT2))->GetLine(0,buf);
		datastore.curString+=buf;
	}
	if(((CButton*)GetDlgItem(IDC_CHECK16))->GetCheck())
		datastore.curString+="_Not";
	if(((CButton*)GetDlgItem(IDC_CHECK15))->GetCheck() || ((CButton*)GetDlgItem(IDC_CHECK16))->GetCheck()){
		datastore.curString+="_AbsoluteHealth_";
		((CEdit*)GetDlgItem(IDC_EDIT3))->GetLine(0,buf);
		datastore.curString+=buf;
	}
	if(((CButton*)GetDlgItem(IDC_CHECK18))->GetCheck())
		datastore.curString+="_Not";
	if(((CButton*)GetDlgItem(IDC_CHECK17))->GetCheck() || ((CButton*)GetDlgItem(IDC_CHECK18))->GetCheck()){
		datastore.curString+="_RelativeHealth_";
		((CEdit*)GetDlgItem(IDC_EDIT4))->GetLine(0,buf);
		datastore.curString+=buf;
	}
	if(((CButton*)GetDlgItem(IDC_CHECK19))->GetCheck())
		datastore.curString+="_InPrevSel";
	if(((CButton*)GetDlgItem(IDC_CHECK20))->GetCheck())
		datastore.curString+="_Not_InPrevSel";

	if(((CButton*)GetDlgItem(IDC_CHECK22))->GetCheck())
		datastore.curString+="_Not";
	if(((CButton*)GetDlgItem(IDC_CHECK21))->GetCheck() || ((CButton*)GetDlgItem(IDC_CHECK22))->GetCheck()){
		datastore.curString+="_NameContain_";
		((CEdit*)GetDlgItem(IDC_EDIT5))->GetLine(0,buf);
		datastore.curString+=buf;
	}
	if(((CButton*)GetDlgItem(IDC_CHECK24))->GetCheck())
		datastore.curString+="_Idle";
	if(((CButton*)GetDlgItem(IDC_CHECK25))->GetCheck())
		datastore.curString+="_Not_Idle";

	if(((CButton*)GetDlgItem(IDC_CHECK26))->GetCheck())
		datastore.curString+="_Radar";
	if(((CButton*)GetDlgItem(IDC_CHECK27))->GetCheck())
		datastore.curString+="_Not_Radar";

	if(((CButton*)GetDlgItem(IDC_CHECK29))->GetCheck())
		datastore.curString+="_Not";
	if(((CButton*)GetDlgItem(IDC_CHECK28))->GetCheck() || ((CButton*)GetDlgItem(IDC_CHECK29))->GetCheck()){
		datastore.curString+="_Category_";
		((CEdit*)GetDlgItem(IDC_EDIT7))->GetLine(0,buf);
		datastore.curString+=buf;
	}

	if(((CButton*)GetDlgItem(IDC_CHECK30))->GetCheck())
		datastore.curString+="_InHotkeyGroup";
	if(((CButton*)GetDlgItem(IDC_CHECK31))->GetCheck())
		datastore.curString+="_Not_InHotkeyGroup";

	datastore.curString+="+";

	if(((CButton*)GetDlgItem(IDC_CHECK23))->GetCheck()){
		datastore.curString+="_ClearSelection";
	}

	if(((CButton*)GetDlgItem(IDC_RADIO5))->GetCheck()){
		datastore.curString+="_SelectAll";
	} else if(((CButton*)GetDlgItem(IDC_RADIO6))->GetCheck()){
		datastore.curString+="_SelectOne";
	} else if(((CButton*)GetDlgItem(IDC_RADIO7))->GetCheck()){
		datastore.curString+="_SelectNum_";
		((CEdit*)GetDlgItem(IDC_EDIT6))->GetLine(0,buf);
		datastore.curString+=buf;
	} else if(((CButton*)GetDlgItem(IDC_RADIO8))->GetCheck()){
		datastore.curString+="_SelectPart_";
		((CEdit*)GetDlgItem(IDC_EDIT8))->GetLine(0,buf);
		datastore.curString+=buf;
	}
	datastore.curString+="+";
	*/

	return str;
}

wxString AddSelectionCmdDlg::getCommandString() const
{
	return this->getSourceString() + this->getConditionsAndSelectString();
}

void AddSelectionCmdDlg::OnButtonAddClick( wxCommandEvent& event )
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
