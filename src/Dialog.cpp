#include "Dialog.hpp"

Dialog::Dialog(const wxString& title): wxFrame(NULL, wxID_ANY, title) {
	//parser = p;
    this->SetBackgroundColour(wxTheColourDatabase->Find(wxT("LIGHT GREY")));
    this->SetForegroundColour(wxTheColourDatabase->Find(wxT("BLACK")));
}
Dialog::~Dialog(void) {
}

// handler called when user presses 'X' on subwindow
void Dialog::OnClose(wxCloseEvent&) {
	Show(false);
}

void Dialog::initMouseSpeedSizer(wxStaticBoxSizer* sizer) {
	// i < "sizeof"(MO_SLI)
	for (int i = 0; i < 5; i++) {
		wxSlider* slider = new wxSlider(this, MO_SLI[i].id, configHandler.GetInt(MO_SLI[i].key,fromString<int>(MO_SLI[i].def)), 0, 10, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
		sizer->Add(new wxStaticText(this, _1, _S(MO_SLI[i].lbl), zeroPoint, wxSize(wxTop,0), 10));
		sizer->Add(slider, 0, wxTOP, 0);
	}
}

void Dialog::initMouseOptsSizer(wxStaticBoxSizer* sizer) {
	wxCheckBox* checkBox0 = new wxCheckBox(this, MO_CBOX[0].id, _S(MO_CBOX[0].lbl));
	wxCheckBox* checkBox1 = new wxCheckBox(this, MO_CBOX[1].id, _S(MO_CBOX[1].lbl));
	wxRadioButton* radioButtonA0 = new wxRadioButton(this, MO_RBUT[0].id, _S(MO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	wxRadioButton* radioButtonA1 = new wxRadioButton(this, MO_RBUT[1].id, _S(MO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA2 = new wxRadioButton(this, MO_RBUT[2].id, _S(MO_RBUT[2].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA3 = new wxRadioButton(this, MO_RBUT[3].id, _S(MO_RBUT[3].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA4 = new wxRadioButton(this, MO_RBUT[4].id, _S(MO_RBUT[4].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	checkBox0->SetValue(configHandler.GetInt(MO_CBOX[0].key,fromString<int>(MO_CBOX[0].def)));
	checkBox1->SetValue(configHandler.GetInt(MO_CBOX[1].key,fromString<int>(MO_CBOX[0].def)));

	// Default Camera Mode radiobuttons
	// (unrelated to which modes are enabled)
	switch (configHandler.GetInt(MO_RBUT[0].key,fromString<int>(MO_RBUT[0].def))) {
		case 0: { radioButtonA3->SetValue(1); } break;	// CamMode 0: FPS
		case 1: { radioButtonA0->SetValue(1); } break;	// CamMode 1: OH
		case 2: { radioButtonA1->SetValue(1); } break;	// CamMode 2: ROH
		case 3: { radioButtonA2->SetValue(1); } break;	// CamMode 3: TW
		case 4: { radioButtonA4->SetValue(1); } break;	// CamMode 4: FC
	}

	sizer->Add(checkBox0, 0, wxTOP, 0);
	sizer->Add(checkBox1, 0, wxTOP, 0);
	sizer->Add(0, 10, 0);
	sizer->Add(new wxStaticText(this, _1, _S("Default Camera Mode"), zeroPoint, wxSize(wxTOP , wxBOTTOM), 10));
	sizer->Add(radioButtonA0, 0, wxTOP, 0);
	sizer->Add(radioButtonA1, 0, wxTOP, 0);
	sizer->Add(radioButtonA2, 0, wxTOP, 0);
	sizer->Add(radioButtonA3, 0, wxTOP, 0);
	sizer->Add(radioButtonA4, 0, wxTOP, 0);
}

void Dialog::init(void) {
	wxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* cSizerL = new wxBoxSizer(wxVERTICAL);
	wxSizer* cSizerR = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* mouseSpeedSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _S("Mouse Speed"), WX_DEF_P, wxSize(270, 200), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* mouseOptsSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _S("Mouse Options"), WX_DEF_P, wxSize(100, 100), 0, _S("")), wxVERTICAL);

	initMouseSpeedSizer(mouseSpeedSizer);
	initMouseOptsSizer(mouseOptsSizer);

	cSizerL->Add(mouseSpeedSizer);
	cSizerR->Add(mouseOptsSizer);
	pSizer->Add(5, 0, 0);
	pSizer->Add(cSizerL);
	pSizer->Add(25, 0, 0);
	pSizer->Add(cSizerR);

	SetSizer(pSizer);
}




void Dialog::OnSliderMove(wxCommandEvent& event) {
	(Main::settingsChanged)	= true;

	wxSlider* slider = (wxSlider*) event.GetEventObject();

    int s1 = slider->GetValue();
    int s2 = ((slider->GetValue() > 0)? 1: 0);
    
	switch (event.GetId()) {
		case ID_MO_SLI_0: {
			configHandler.SetInt(MO_SLI[0].key, s1);
			configHandler.SetInt(MO_SLI_EXT[0].key, s2);
		} break;
		case ID_MO_SLI_1: {
			configHandler.SetInt(MO_SLI[1].key, s1);
			configHandler.SetInt(MO_SLI_EXT[1].key, s2);
		} break;
		case ID_MO_SLI_2: {
			configHandler.SetInt(MO_SLI[2].key, s1);
			configHandler.SetInt(MO_SLI_EXT[2].key, s2);
		} break;
		case ID_MO_SLI_3: {
			configHandler.SetInt(MO_SLI[3].key, s1);
			configHandler.SetInt(MO_SLI_EXT[3].key, s2);
		} break;
		case ID_MO_SLI_4: {
			configHandler.SetInt(MO_SLI[4].key, s1);
			configHandler.SetInt(MO_SLI_EXT[4].key, s2);
		} break;
	}
}


void Dialog::OnCheckBoxTick(wxCommandEvent& event) {
	(Main::settingsChanged) = true;

	wxCheckBox* checkBox = (wxCheckBox*) event.GetEventObject();

	int s = checkBox->IsChecked();

	switch (event.GetId()) {
		case ID_WINDOWC_MO_CBOX_0: { configHandler.SetInt(MO_CBOX[0].key, s); } break;
		case ID_WINDOWC_MO_CBOX_1: { configHandler.SetInt(MO_CBOX[1].key, s); } break;
	}
}


void Dialog::OnRadioButtonToggle(wxCommandEvent& event) {
	(Main::settingsChanged) = true;

	wxRadioButton* radioButton = (wxRadioButton*) event.GetEventObject();
	radioButton->GetValue();

	switch (event.GetId()) {
		case ID_WINDOWC_MO_RBUT_0: { configHandler.SetInt(MO_RBUT[0].key, 1); } break;	// OH button (CamMode 1)
		case ID_WINDOWC_MO_RBUT_1: { configHandler.SetInt(MO_RBUT[0].key, 2); } break;	// ROH button (CamMode 2)
		case ID_WINDOWC_MO_RBUT_2: { configHandler.SetInt(MO_RBUT[0].key, 3); } break;	// TW button (CamMode 3)
		case ID_WINDOWC_MO_RBUT_3: { configHandler.SetInt(MO_RBUT[0].key, 0); } break;	// FPS button (CamMode 0)
		case ID_WINDOWC_MO_RBUT_4: { configHandler.SetInt(MO_RBUT[0].key, 4); } break;	// FC button (CamMode 4)
	}
}




BEGIN_EVENT_TABLE(Dialog, wxFrame)
	EVT_CLOSE(                Dialog::OnClose)
	EVT_SLIDER(wxID_ANY,      Dialog::OnSliderMove)
	EVT_CHECKBOX(wxID_ANY,    Dialog::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY, Dialog::OnRadioButtonToggle)
END_EVENT_TABLE()
