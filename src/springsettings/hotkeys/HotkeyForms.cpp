///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "HotkeyForms.h"

///////////////////////////////////////////////////////////////////////////

AddSelectionCmdDlgBase::AddSelectionCmdDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Source") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer51;
	fgSizer51 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer51->SetFlexibleDirection( wxBOTH );
	fgSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioBtnSrcMap = new wxRadioButton( this, wxID_ANY, wxT("Complete map"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtnSrcMap->SetValue( true ); 
	fgSizer51->Add( m_radioBtnSrcMap, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_radioBtnSrcView = new wxRadioButton( this, wxID_ANY, wxT("All in view"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer51->Add( m_radioBtnSrcView, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_radioBtnSrcSel = new wxRadioButton( this, wxID_ANY, wxT("Previous selection"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer51->Add( m_radioBtnSrcSel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtnSrcMouse = new wxRadioButton( this, wxID_ANY, wxT("Max. distance from mouse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_radioBtnSrcMouse, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrlSrcMouseDist = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), wxTE_RIGHT );
	m_textCtrlSrcMouseDist->Enable( false );
	
	bSizer3->Add( m_textCtrlSrcMouseDist, 0, wxALL, 5 );
	
	fgSizer51->Add( bSizer3, 1, wxEXPAND, 5 );
	
	
	fgSizer51->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxSrcDistCyl = new wxCheckBox( this, wxID_ANY, wxT("Use Distance cylinder"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_checkBoxSrcDistCyl->Enable( false );
	
	fgSizer51->Add( m_checkBoxSrcDistCyl, 0, wxALL, 5 );
	
	sbSizer8->Add( fgSizer51, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( sbSizer8, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Select") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioBtnSelAll = new wxRadioButton( this, wxID_ANY, wxT("All"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtnSelAll->SetValue( true ); 
	fgSizer3->Add( m_radioBtnSelAll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtnSelCount = new wxRadioButton( this, wxID_ANY, wxT("Max. count"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_radioBtnSelCount, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlSelCount = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 30,-1 ), wxTE_RIGHT );
	m_textCtrlSelCount->Enable( false );
	
	bSizer1->Add( m_textCtrlSelCount, 0, wxALL, 5 );
	
	fgSizer3->Add( bSizer1, 1, wxEXPAND, 5 );
	
	m_radioBtnSelOne = new wxRadioButton( this, wxID_ANY, wxT("One (go to)"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_radioBtnSelOne, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtnSelPerc = new wxRadioButton( this, wxID_ANY, wxT("Percentage"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_radioBtnSelPerc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlSelPerc = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 30,-1 ), wxTE_RIGHT );
	m_textCtrlSelPerc->Enable( false );
	
	bSizer2->Add( m_textCtrlSelPerc, 0, wxALL, 5 );
	
	m_staticText20 = new wxStaticText( this, wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	bSizer2->Add( m_staticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	fgSizer3->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_checkBoxClearPrevSel = new wxCheckBox( this, wxID_ANY, wxT("Clear previous selection"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_checkBoxClearPrevSel->SetValue(true);
	
	fgSizer3->Add( m_checkBoxClearPrevSel, 0, wxALL, 5 );
	
	sbSizer7->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( sbSizer7, 1, wxALL|wxEXPAND, 5 );
	
	fgSizer5->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Conditions") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 7, 7, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Include"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer6->Add( m_staticText9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Exclude"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer6->Add( m_staticText11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	
	fgSizer6->Add( 255, 0, 1, wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("Include"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer6->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("Exclude"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer6->Add( m_staticText13, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxBuilderInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxBuilderInc, 0, wxALIGN_CENTER|wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxBuilderEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxBuilderEx, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, wxT("Builder"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer6->Add( m_staticText14, 0, wxALL, 5 );
	
	m_checkBoxBuildingInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxBuildingInc, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_checkBoxBuildingEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxBuildingEx, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("Building"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer6->Add( m_staticText15, 0, wxALL, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxComInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxComInc, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_checkBoxComEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxComEx, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("Manual Fire Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer6->Add( m_staticText17, 0, wxALL, 5 );
	
	m_checkBoxTypeSelInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxTypeSelInc, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_checkBoxTypeSelEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxTypeSelEx, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("Type in selection"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	fgSizer6->Add( m_staticText18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxAirInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxAirInc, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_checkBoxAirEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxAirEx, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText19 = new wxStaticText( this, wxID_ANY, wxT("Aircraft"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer6->Add( m_staticText19, 0, wxALL, 5 );
	
	m_checkBoxWaitingInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxWaitingInc, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_checkBoxWaitingEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxWaitingEx, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText231 = new wxStaticText( this, wxID_ANY, wxT("Waiting"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	fgSizer6->Add( m_staticText231, 0, wxALL, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxWeaponsInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxWeaponsInc, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_checkBoxWeaponsEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxWeaponsEx, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, wxT("Weapons"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer6->Add( m_staticText21, 0, wxALL, 5 );
	
	m_checkBoxWeapRangeInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxWeapRangeInc, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_checkBoxWeapRangeEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxWeapRangeEx, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText22 = new wxStaticText( this, wxID_ANY, wxT("Weapon range >"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer6->Add( m_staticText22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlWeapRange = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_textCtrlWeapRange->Enable( false );
	
	fgSizer6->Add( m_textCtrlWeapRange, 0, wxALL, 5 );
	
	m_checkBoxIdleInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxIdleInc, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_checkBoxIdleEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxIdleEx, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, wxT("Idle"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer6->Add( m_staticText23, 0, wxALL, 5 );
	
	m_checkBoxAbsHealthInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxAbsHealthInc, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_checkBoxAbsHealthEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxAbsHealthEx, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText24 = new wxStaticText( this, wxID_ANY, wxT("Aboslute health >"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer6->Add( m_staticText24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlAbsHealth = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_textCtrlAbsHealth->Enable( false );
	
	fgSizer6->Add( m_textCtrlAbsHealth, 0, wxALL, 5 );
	
	m_checkBoxRadarInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxRadarInc, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_checkBoxRadarEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxRadarEx, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, wxT("Radar, Sonar, Jammer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer6->Add( m_staticText25, 0, wxALL, 5 );
	
	m_checkBoxNameInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxNameInc, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_checkBoxNameEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxNameEx, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText26 = new wxStaticText( this, wxID_ANY, wxT("Name contains"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer6->Add( m_staticText26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_textCtrlName->Enable( false );
	
	fgSizer6->Add( m_textCtrlName, 0, wxALL, 5 );
	
	m_checkBoxTransInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxTransInc, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_checkBoxTransEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxTransEx, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, wxT("Transport"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer6->Add( m_staticText27, 0, wxALL, 5 );
	
	m_checkBoxCategoryInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxCategoryInc, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_checkBoxCategoryEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxCategoryEx, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText29 = new wxStaticText( this, wxID_ANY, wxT("Category"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	fgSizer6->Add( m_staticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlCategory = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_textCtrlCategory->Enable( false );
	
	fgSizer6->Add( m_textCtrlCategory, 0, wxALL, 5 );
	
	m_checkBoxHotkeyInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxHotkeyInc, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_checkBoxHotkeyEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxHotkeyEx, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_staticText221 = new wxStaticText( this, wxID_ANY, wxT("In hotkey group"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	fgSizer6->Add( m_staticText221, 0, wxALL, 5 );
	
	m_checkBoxRelHealthInc = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxRelHealthInc, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_checkBoxRelHealthEx = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer6->Add( m_checkBoxRelHealthEx, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText32 = new wxStaticText( this, wxID_ANY, wxT("Relative health"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	fgSizer6->Add( m_staticText32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlRelHealth = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_textCtrlRelHealth->Enable( false );
	
	fgSizer6->Add( m_textCtrlRelHealth, 0, wxALL, 5 );
	
	sbSizer9->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	fgSizer5->Add( sbSizer9, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer24->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_buttonOk = new wxButton( this, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonCancel, 0, wxALL, 5 );
	
	fgSizer5->Add( bSizer24, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer5 );
	this->Layout();
	fgSizer5->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_radioBtnSrcMap->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcMapClick ), NULL, this );
	m_radioBtnSrcView->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcViewClick ), NULL, this );
	m_radioBtnSrcSel->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcSelClick ), NULL, this );
	m_radioBtnSrcMouse->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcDistClick ), NULL, this );
	m_radioBtnSelAll->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelAllClick ), NULL, this );
	m_radioBtnSelCount->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelCountClick ), NULL, this );
	m_radioBtnSelOne->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelOneClick ), NULL, this );
	m_radioBtnSelPerc->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelPercClick ), NULL, this );
	m_checkBoxBuilderInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuilderIncClick ), NULL, this );
	m_checkBoxBuilderEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuilderExClick ), NULL, this );
	m_checkBoxBuildingInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuildingIncClick ), NULL, this );
	m_checkBoxBuildingEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuildingExClick ), NULL, this );
	m_checkBoxComInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondComIncClick ), NULL, this );
	m_checkBoxComEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondComExClick ), NULL, this );
	m_checkBoxTypeSelInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTypeSelIncClick ), NULL, this );
	m_checkBoxTypeSelEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTypeSelExClick ), NULL, this );
	m_checkBoxAirInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAirIncClick ), NULL, this );
	m_checkBoxAirEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAirExClick ), NULL, this );
	m_checkBoxWaitingInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWaitingIncClick ), NULL, this );
	m_checkBoxWaitingEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWaitingExClick ), NULL, this );
	m_checkBoxWeaponsInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeaponsIncClick ), NULL, this );
	m_checkBoxWeaponsEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeaponsExClick ), NULL, this );
	m_checkBoxWeapRangeInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeapRangeIncClick ), NULL, this );
	m_checkBoxWeapRangeEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeapRangeExClick ), NULL, this );
	m_checkBoxIdleInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondIdleIncClick ), NULL, this );
	m_checkBoxIdleEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondIdleExClick ), NULL, this );
	m_checkBoxAbsHealthInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAbsHealthIncClick ), NULL, this );
	m_checkBoxAbsHealthEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAbsHealthExClick ), NULL, this );
	m_checkBoxRadarInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRadarIncClick ), NULL, this );
	m_checkBoxRadarEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRadarExClick ), NULL, this );
	m_checkBoxNameInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondNameIncClick ), NULL, this );
	m_checkBoxNameEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondNameExClick ), NULL, this );
	m_checkBoxTransInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTransIncClick ), NULL, this );
	m_checkBoxTransEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTransExClick ), NULL, this );
	m_checkBoxCategoryInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondCategoryIncClick ), NULL, this );
	m_checkBoxCategoryEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondCategoryExClick ), NULL, this );
	m_checkBoxHotkeyInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondHotkeyIncClick ), NULL, this );
	m_checkBoxHotkeyEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondHotkeyExClick ), NULL, this );
	m_checkBoxRelHealthInc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRelHealthIncClick ), NULL, this );
	m_checkBoxRelHealthEx->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRelHealthExClick ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnButtonAddClick ), NULL, this );
}

AddSelectionCmdDlgBase::~AddSelectionCmdDlgBase()
{
	// Disconnect Events
	m_radioBtnSrcMap->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcMapClick ), NULL, this );
	m_radioBtnSrcView->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcViewClick ), NULL, this );
	m_radioBtnSrcSel->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcSelClick ), NULL, this );
	m_radioBtnSrcMouse->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSrcDistClick ), NULL, this );
	m_radioBtnSelAll->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelAllClick ), NULL, this );
	m_radioBtnSelCount->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelCountClick ), NULL, this );
	m_radioBtnSelOne->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelOneClick ), NULL, this );
	m_radioBtnSelPerc->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnRadioBtnSelPercClick ), NULL, this );
	m_checkBoxBuilderInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuilderIncClick ), NULL, this );
	m_checkBoxBuilderEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuilderExClick ), NULL, this );
	m_checkBoxBuildingInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuildingIncClick ), NULL, this );
	m_checkBoxBuildingEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondBuildingExClick ), NULL, this );
	m_checkBoxComInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondComIncClick ), NULL, this );
	m_checkBoxComEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondComExClick ), NULL, this );
	m_checkBoxTypeSelInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTypeSelIncClick ), NULL, this );
	m_checkBoxTypeSelEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTypeSelExClick ), NULL, this );
	m_checkBoxAirInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAirIncClick ), NULL, this );
	m_checkBoxAirEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAirExClick ), NULL, this );
	m_checkBoxWaitingInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWaitingIncClick ), NULL, this );
	m_checkBoxWaitingEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWaitingExClick ), NULL, this );
	m_checkBoxWeaponsInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeaponsIncClick ), NULL, this );
	m_checkBoxWeaponsEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeaponsExClick ), NULL, this );
	m_checkBoxWeapRangeInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeapRangeIncClick ), NULL, this );
	m_checkBoxWeapRangeEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondWeapRangeExClick ), NULL, this );
	m_checkBoxIdleInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondIdleIncClick ), NULL, this );
	m_checkBoxIdleEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondIdleExClick ), NULL, this );
	m_checkBoxAbsHealthInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAbsHealthIncClick ), NULL, this );
	m_checkBoxAbsHealthEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondAbsHealthExClick ), NULL, this );
	m_checkBoxRadarInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRadarIncClick ), NULL, this );
	m_checkBoxRadarEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRadarExClick ), NULL, this );
	m_checkBoxNameInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondNameIncClick ), NULL, this );
	m_checkBoxNameEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondNameExClick ), NULL, this );
	m_checkBoxTransInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTransIncClick ), NULL, this );
	m_checkBoxTransEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondTransExClick ), NULL, this );
	m_checkBoxCategoryInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondCategoryIncClick ), NULL, this );
	m_checkBoxCategoryEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondCategoryExClick ), NULL, this );
	m_checkBoxHotkeyInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondHotkeyIncClick ), NULL, this );
	m_checkBoxHotkeyEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondHotkeyExClick ), NULL, this );
	m_checkBoxRelHealthInc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRelHealthIncClick ), NULL, this );
	m_checkBoxRelHealthEx->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnCheckBoxCondRelHealthExClick ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AddSelectionCmdDlgBase::OnButtonAddClick ), NULL, this );
}

CommandOrderDlgBase::CommandOrderDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText22 = new wxStaticText( this, wxID_ANY, wxT("Command execution order:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer7->Add( m_staticText22, 0, wxALL, 5 );
	
	
	fgSizer7->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_listBoxCommands = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 200,150 ), 0, NULL, 0 ); 
	fgSizer7->Add( m_listBoxCommands, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonUp = new wxButton( this, wxID_ANY, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonUp, 0, wxALL, 5 );
	
	m_buttonDown = new wxButton( this, wxID_ANY, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonDown, 0, wxALL, 5 );
	
	fgSizer7->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_buttonOk, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_buttonCancel, 0, wxALL, 5 );
	
	this->SetSizer( fgSizer7 );
	this->Layout();
	fgSizer7->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( CommandOrderDlgBase::OnInitDialog ) );
	m_buttonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CommandOrderDlgBase::OnButtonUpClick ), NULL, this );
	m_buttonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CommandOrderDlgBase::OnButtonDownClick ), NULL, this );
}

CommandOrderDlgBase::~CommandOrderDlgBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( CommandOrderDlgBase::OnInitDialog ) );
	m_buttonUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CommandOrderDlgBase::OnButtonUpClick ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CommandOrderDlgBase::OnButtonDownClick ), NULL, this );
}

ChangeMetaDlgBase::ChangeMetaDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText24 = new wxStaticText( this, wxID_ANY, wxT("Current key:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer7->Add( m_staticText24, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_textCtrlCurKey = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer7->Add( m_textCtrlCurKey, 0, wxALL, 5 );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, wxT("New key:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer7->Add( m_staticText23, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_ctrlNewKey = new wxKeyMonitorTextCtrl( this, wxID_ANY );
	fgSizer7->Add( m_ctrlNewKey, 0, wxALL, 5 );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("Assign"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_buttonCancel, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	this->SetSizer( fgSizer7 );
	this->Layout();
	fgSizer7->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( ChangeMetaDlgBase::OnInitDialog ) );
}

ChangeMetaDlgBase::~ChangeMetaDlgBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( ChangeMetaDlgBase::OnInitDialog ) );
}
