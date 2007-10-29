#include "Window.hpp"
#include "Dialog.hpp"

void Window::initRendererSizer(wxStaticBoxSizer* sizer) {
	const int extrema[] = {
		1024, 4096,   600, 3000,   20, 120,   100, 600,   0, 10,   0, 100,   100, 1000,   1000, 20000,   1, 20
	};

	// i < "sizeof"(RO_SLI)
	for (int i = 0; i <= 8; i++) {
		wxSlider* slider = new wxSlider(
			this, RO_SLI[i].id, configHandler.GetInt(RO_SLI[i].key,fromString<int>(RO_SLI[i].def)),
			extrema[i * 2],  extrema[(i * 2) + 1], WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V
		);
		sizer->Add(new wxStaticText(this, _1,  _S(RO_SLI[i].lbl)), 0, wxTOP, 15);
		sizer->Add(slider, 0, wxALIGN_RIGHT, 0);
	}
}


void Window::initGeneralSizer(wxStaticBoxSizer* sizer) {
	// i < "sizeof"(GO_CBOX)
	for (int i = 0; i <= 23; i++) {
		wxCheckBox* checkBox = new wxCheckBox(this, GO_CBOX[i].id, _S(GO_CBOX[i].lbl));
		checkBox->SetValue(configHandler.GetInt(GO_CBOX[i].key,fromString<int>(GO_CBOX[i].def)));

		sizer->Add(checkBox, 0, wxTOP, ((i == 0)? 10: 0));
	}
}


void Window::initWaterSizer(wxStaticBoxSizer* sizer) {
	wxRadioButton* rButtonBa = new wxRadioButton(this, WR_RBUT[0].id, _S(WR_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	wxRadioButton* rButtonRO = new wxRadioButton(this, WR_RBUT[1].id, _S(WR_RBUT[1].lbl), WX_DEF_P, WX_DEF_S,          0, WX_DEF_V);
	wxRadioButton* rButtonRR = new wxRadioButton(this, WR_RBUT[2].id, _S(WR_RBUT[2].lbl), WX_DEF_P, WX_DEF_S,          0, WX_DEF_V);
	wxRadioButton* rButtonDy = new wxRadioButton(this, WR_RBUT[3].id, _S(WR_RBUT[3].lbl), WX_DEF_P, WX_DEF_S,          0, WX_DEF_V);

	switch (configHandler.GetInt(WR_RBUT[0].key,fromString<int>((WR_RBUT[0].def)))) {
		case 0: { rButtonBa->SetValue(1); } break;	// ReflectiveWater=0, Basic
		case 1: { rButtonRO->SetValue(1); } break;	// ReflectiveWater=1, Reflective Only
		case 2: { rButtonDy->SetValue(1); } break;	// ReflectiveWater=2, Dynamic
		case 3: { rButtonRR->SetValue(1); } break;	// ReflectiveWater=3, Reflective + Refractive
	}

	sizer->Add(rButtonBa, 0, wxTOP, 10);
	sizer->Add(rButtonRO, 0, wxTOP,  0);
	sizer->Add(rButtonRR, 0, wxTOP,  0);
	sizer->Add(rButtonDy, 0, wxTOP,  0);
}


void Window::initVideoSizer(wxStaticBoxSizer* sizer) {
	wxRadioButton* radioButtonA0 = new wxRadioButton(this, VO_RBUT[0].id, _S(VO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	wxRadioButton* radioButtonA1 = new wxRadioButton(this, VO_RBUT[1].id, _S(VO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	switch (configHandler.GetInt(VO_RBUT[0].key,fromString<int>(VO_RBUT[0].def))) {
		case 16: { radioButtonA0->SetValue(1); } break;
		case 24: { radioButtonA1->SetValue(1); } break;
	}

	sizer->Add(radioButtonA0, 0, wxTOP, 10);
	sizer->Add(radioButtonA1, 0, wxTOP,  0);


	// i < "sizeof"(VO_CBOX)
	for (int i = 0; i < 5; i++) {
		wxCheckBox* checkBox = new wxCheckBox(this, VO_CBOX[i].id, _S(VO_CBOX[i].lbl));
		checkBox->SetValue(configHandler.GetInt(VO_CBOX[i].key,fromString<int>(VO_CBOX[i].def)));
		sizer->Add(checkBox, 0, wxTOP, (i == 0)? 10: 0);
	}

	int useFSAA = configHandler.GetInt(VO_SLI_EXT[0].key,fromString<int>(VO_SLI_EXT[0].def));
	int FSAALev = configHandler.GetInt(VO_SLI[0].key,fromString<int>(VO_SLI[0].def));
	wxSlider* slider = new wxSlider(this, VO_SLI[0].id, (useFSAA == 1)? FSAALev: 0, 0, 16, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);


	wxTextCtrl* xResInput = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_X, _S(""), WX_DEF_P, wxSize(60, 20), 0);
	wxTextCtrl* yResInput = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_Y, _S(""), WX_DEF_P, wxSize(60, 20), 0);
        std::string s = (configHandler.GetString(RC_TEXT[0].key,RC_TEXT[0].def));
	xResInput->ChangeValue(_S(s.c_str()));
        s = configHandler.GetString(RC_TEXT[1].key,RC_TEXT[1].def);
	yResInput->ChangeValue(_S(s.c_str()));


	wxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);
	subSizer->Add(xResInput, 0, wxALIGN_LEFT, 10);
	subSizer->Add(5, 0, 0);
	subSizer->Add(new wxStaticText(this, -1, wxT("x")), 0, wxTOP | wxBOTTOM, 0);
	subSizer->Add(5, 0, 0);
	subSizer->Add(yResInput, 0, wxALIGN_RIGHT, 10);


	// note: actually viewport dimension
	sizer->Add(new wxStaticText(this, -1, wxT("Screen Resolution")), 0, wxTOP | wxBOTTOM, 15);
	sizer->Add(subSizer);
	sizer->Add(new wxStaticText(this, -1, _S(VO_SLI[0].lbl)), 0, wxTOP, 15);
	sizer->Add(slider, 0, wxALIGN_LEFT, 0);
}


void Window::initAudioSizer(wxStaticBoxSizer* sizer) {
	wxSlider* slider0 = new wxSlider(this, AO_SLI[0].id, configHandler.GetInt(AO_SLI[0].key,fromString<int>(AO_SLI[0].def)), 8, 128, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	wxSlider* slider1 = new wxSlider(this, AO_SLI[1].id, configHandler.GetInt(AO_SLI[1].key,fromString<int>(AO_SLI[1].def)), 1, 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	wxSlider* slider2 = new wxSlider(this, AO_SLI[2].id, configHandler.GetInt(AO_SLI[2].key,fromString<int>(AO_SLI[2].def)), 1, 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);

	sizer->Add(new wxStaticText(this, -1, _S(AO_SLI[0].lbl)), 0, wxTOP, 15);
	sizer->Add(slider0, 0, wxALIGN_LEFT, 0);
	sizer->Add(new wxStaticText(this, -1, _S(AO_SLI[1].lbl)), 0, wxTOP, 15);
	sizer->Add(slider1, 0, wxALIGN_LEFT, 0);
	sizer->Add(new wxStaticText(this, -1, _S(AO_SLI[2].lbl)), 0, wxTOP, 15);
	sizer->Add(slider2, 0, wxALIGN_LEFT, 0);
}


void Window::initDebugSizer(wxStaticBoxSizer* sizer) {
	wxSlider* slider = new wxSlider(this, DO_SLI[0].id, configHandler.GetInt(DO_SLI[0].key,fromString<int>(DO_SLI[0].def)), 0, 10, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	wxCheckBox* checkBox0 = new wxCheckBox(this, DO_CBOX[0].id, _S(DO_CBOX[0].lbl));
	wxCheckBox* checkBox1 = new wxCheckBox(this, DO_CBOX[1].id, _S(DO_CBOX[1].lbl));

	checkBox0->SetValue(configHandler.GetInt(DO_CBOX[0].key,fromString<int>(DO_SLI[0].def)));
	checkBox1->SetValue(configHandler.GetInt(DO_CBOX[1].key,fromString<int>(DO_SLI[1].def)));

	sizer->Add(0, 10, 0);
	sizer->Add(checkBox0, 0, wxTOP, 0);
	sizer->Add(checkBox1, 0, wxTOP, 0);
	sizer->Add(0, 10, 0);
	sizer->Add(new wxStaticText(this, -1, _S(DO_SLI[0].lbl)), 0, wxTOP, 10);
	sizer->Add(slider, 0, wxTOP, 0);
}


void Window::initMenuBar() {
	wxMenu* menuFile = new wxMenu();
	wxMenu* menuMouse = new wxMenu();
        //not necessary anymore----------------
	//menuFile->Append(ID_MENUITEM_SAVE, wxT("Save"));
	//menuFile->AppendSeparator();
        //-------------------------------------
	menuFile->Append(ID_MENUITEM_QUIT, wxT("Quit"));
	menuMouse->Append(ID_MENUITEM_MOUSE, wxT("Mouse Settings..."));

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, wxT("File"));
	menuBar->Append(menuMouse, wxT("Options"));

	SetMenuBar(menuBar);
}


Window::Window(const wxString& title): wxFrame(NULL, wxID_ANY, title) {
	
    this->SetBackgroundColour(wxTheColourDatabase->Find(wxT("LIGHT GREY")));
    this->SetForegroundColour(wxTheColourDatabase->Find(wxT("BLACK")));

    this->initMenuBar();

	dialog = new Dialog(wxT("Mouse Settings Dialog"));

	dialog->SetSize(0, 0, CWIN_SIZE_X, CWIN_SIZE_Y);
	dialog->SetSizeHints(CWIN_SIZE_X, CWIN_SIZE_Y, CWIN_SIZE_X, CWIN_SIZE_Y);
	dialog->CenterOnScreen(wxBOTH);
	dialog->init();

	wxSizer* parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	wxSizer* childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer
	wxSizer* childMSizer = new wxBoxSizer(wxVERTICAL);		// main window middle column sizer
	wxSizer* childRSizer = new wxBoxSizer(wxVERTICAL);		// main window right column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
	wxStaticBoxSizer* rendererSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Rendering Options"), WX_DEF_P, wxSize(220, 300), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* waterSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Water Rendering"), WX_DEF_P, wxSize(220 + 5, 100), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* generalSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("General Options"), WX_DEF_P, wxSize(250, 250), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* videoSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Video Options"), WX_DEF_P, wxSize(230, 100), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* audioSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Audio Options"), WX_DEF_P, wxSize(230, 100), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* debugSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Debug Options"), WX_DEF_P, wxSize(230, 120), 0, _S("")), wxVERTICAL);

	initRendererSizer(rendererSizer);
	initGeneralSizer(generalSizer);
	initVideoSizer(videoSizer);
	initAudioSizer(audioSizer);
	initWaterSizer(waterSizer);
	initDebugSizer(debugSizer);

	childLSizer->Add(0, 5, 0);
	childLSizer->Add(rendererSizer);

	childMSizer->Add(0, 5, 0);
	childMSizer->Add(videoSizer);
	childMSizer->Add(0, 35, 0);
	childMSizer->Add(audioSizer);
	childMSizer->Add(0, 40, 0);
	childMSizer->Add(debugSizer);

	childRSizer->Add(0, 5, 0);
	childRSizer->Add(generalSizer);
	childRSizer->Add(0, 10, 0);
	childRSizer->Add(waterSizer);

	parentSizer->Add(10, 0, 0);
	parentSizer->Add(childLSizer);
	parentSizer->Add(20, 0, 0);
	parentSizer->Add(childMSizer);
	parentSizer->Add(20, 0, 0);
	parentSizer->Add(childRSizer);

	SetSizer(parentSizer);
	
}

Window::~Window(void) {
	dialog->Destroy();
}

void Window::handleExit(void) {
//	if (Main::settingsChanged) {
//		if ((wxMessageBox(wxT("Save settings before exiting?"), _S(""), wxYES_NO, this)) == wxYES) {
//                        //
//			//parser->writeFile(getenv("HOME"), ".springrc");
//		}
//	}

	Destroy();
}


// close-button event handler
void Window::OnCloseButtonPress(wxCloseEvent&) {
	handleExit();
}


void Window::OnMenuChoice(wxCommandEvent& event) {
	switch (event.GetId()) {
// menu entry removed
//		case ID_MENUITEM_SAVE: {
//			(Main::settingsChanged) = false;
//			parser->writeFile(getenv("HOME"), ".springrc");
//		} break;

		case ID_MENUITEM_QUIT: {
			handleExit();
		} break;

		case ID_MENUITEM_MOUSE: {
			dialog->Show(true);
		} break;
	}
}


void Window::OnSliderMove(wxCommandEvent& event) {
	(Main::settingsChanged) = true;

	wxSlider* slider = (wxSlider*) event.GetEventObject();

    int value = slider->GetValue();
    
	switch (event.GetId()) {
		case ID_RO_SLI_0: {
			// shadow-map sizes
			int val = slider->GetValue();

			if (val < 1536) {
				slider->SetValue(1024);
				configHandler.SetInt(RO_SLI[0].key, 1024);
			}
			if (val > 1536 && val < 3072) {
				slider->SetValue(2048);
				configHandler.SetInt(RO_SLI[0].key, 2048);
			}
			if (val > 3072 && val < 6144) {
				slider->SetValue(4096);
				configHandler.SetInt(RO_SLI[0].key, 4096);
			}
		} break;

		case ID_RO_SLI_1: { configHandler.SetInt(RO_SLI[1].key, value); } break;
		case ID_RO_SLI_2: { configHandler.SetInt(RO_SLI[2].key, value); } break;
		case ID_RO_SLI_3: { configHandler.SetInt(RO_SLI[3].key, value); } break;
		case ID_RO_SLI_4: { configHandler.SetInt(RO_SLI[4].key, value); } break;
		case ID_RO_SLI_5: { configHandler.SetInt(RO_SLI[5].key, value); } break;
		case ID_RO_SLI_6: { configHandler.SetInt(RO_SLI[6].key, value); } break;
		case ID_RO_SLI_7: { configHandler.SetInt(RO_SLI[7].key, value); } break;
		case ID_RO_SLI_8: { configHandler.SetInt(RO_SLI[8].key, value); } break;

		case ID_VO_SLI_0: {
			configHandler.SetInt(VO_SLI_EXT[0].key, (value > 0)? 1: 0);
			configHandler.SetInt(VO_SLI[0].key, value);
		} break;

		case ID_AO_SLI_0: { configHandler.SetInt(AO_SLI[0].key, value); } break;
		case ID_AO_SLI_1: { configHandler.SetInt(AO_SLI[1].key, value); } break;
		case ID_AO_SLI_2: { configHandler.SetInt(AO_SLI[2].key, value); } break;

		case ID_DO_SLI_0: { configHandler.SetInt(DO_SLI[0].key, value); } break;
	}
}


void Window::OnTextUpdate(wxCommandEvent& event) {
	(Main::settingsChanged) = true;

	wxTextCtrl* textField = (wxTextCtrl*) event.GetEventObject();

	switch (event.GetId()) {
		case ID_RES_CHOICES_LBOX_X: {
			// note: input validation?
			configHandler.SetString(RC_TEXT[0].key, std::string(textField->GetValue().mb_str(wxConvUTF8)));
		} break;
		case ID_RES_CHOICES_LBOX_Y: {
			// note: input validation?
			configHandler.SetString(RC_TEXT[1].key, std::string(textField->GetValue().mb_str(wxConvUTF8)));
		} break;
	}
}


void Window::OnCheckBoxTick(wxCommandEvent& event) {
	(Main::settingsChanged) = true;

	wxCheckBox* checkBox = (wxCheckBox*) event.GetEventObject();
	int checked = checkBox->IsChecked();
	int id = event.GetId();
    
	switch (id) {
		case ID_WINDOWP_VO_CBOX_0:
		case ID_WINDOWP_VO_CBOX_1:
		case ID_WINDOWP_VO_CBOX_2:
		case ID_WINDOWP_VO_CBOX_3:
		case ID_WINDOWP_VO_CBOX_4: {
			int i = id - VO_CBOX[0].id;
			configHandler.SetInt(VO_CBOX[i].key, checked);
		} break;

		case ID_WINDOWP_GO_CBOX_0:
		case ID_WINDOWP_GO_CBOX_1:
		case ID_WINDOWP_GO_CBOX_2:
		case ID_WINDOWP_GO_CBOX_3:
		case ID_WINDOWP_GO_CBOX_4:
		case ID_WINDOWP_GO_CBOX_5:
		case ID_WINDOWP_GO_CBOX_6:
		case ID_WINDOWP_GO_CBOX_7:
		case ID_WINDOWP_GO_CBOX_8:
		case ID_WINDOWP_GO_CBOX_9:
		case ID_WINDOWP_GO_CBOX_10:
		case ID_WINDOWP_GO_CBOX_11:
		case ID_WINDOWP_GO_CBOX_12:
		case ID_WINDOWP_GO_CBOX_13:
		case ID_WINDOWP_GO_CBOX_14:
		case ID_WINDOWP_GO_CBOX_15:
		case ID_WINDOWP_GO_CBOX_16:
		case ID_WINDOWP_GO_CBOX_17:
		case ID_WINDOWP_GO_CBOX_18:
		case ID_WINDOWP_GO_CBOX_19:
		case ID_WINDOWP_GO_CBOX_20:
		case ID_WINDOWP_GO_CBOX_21:
		case ID_WINDOWP_GO_CBOX_22:
		case ID_WINDOWP_GO_CBOX_23: {
			int i = id - GO_CBOX[0].id;
			configHandler.SetInt(GO_CBOX[i].key, checked);
		} break;

		case ID_WINDOWP_DO_CBOX_0: { configHandler.SetInt(DO_CBOX[0].key, checked); } break;
		case ID_WINDOWP_DO_CBOX_1: { configHandler.SetInt(DO_CBOX[1].key, checked); } break;
	}
}


void Window::OnRadioButtonToggle(wxCommandEvent& event) {
	(Main::settingsChanged) = true;

	wxRadioButton* radioButton = (wxRadioButton*) event.GetEventObject();
	radioButton->GetValue();

	switch (event.GetId()) {
		case ID_WINDOWP_VO_RBUT_0: { configHandler.SetInt(VO_RBUT[0].key, 16); } break;
		case ID_WINDOWP_VO_RBUT_1: { configHandler.SetInt(VO_RBUT[0].key, 24); } break;

		case ID_WINDOWP_WR_RBUT_0: { configHandler.SetInt(WR_RBUT[0].key, 0); } break;	// Basic
		case ID_WINDOWP_WR_RBUT_1: { configHandler.SetInt(WR_RBUT[0].key, 1); } break;	// Refl
		case ID_WINDOWP_WR_RBUT_2: { configHandler.SetInt(WR_RBUT[0].key, 3); } break;	// Refl + Refr
		case ID_WINDOWP_WR_RBUT_3: { configHandler.SetInt(WR_RBUT[0].key, 2); } break;	// Dyna
	}
}


void Window::update(wxIdleEvent& event) {
	event.RequestMore();
}



BEGIN_EVENT_TABLE(Window, wxFrame)
	EVT_CLOSE(                      Window::OnCloseButtonPress)
	EVT_MENU(wxID_ANY,              Window::OnMenuChoice)
	EVT_SLIDER(wxID_ANY,            Window::OnSliderMove)
	EVT_TEXT(wxID_ANY,              Window::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          Window::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       Window::OnRadioButtonToggle)
	EVT_IDLE(                       Window::update)
END_EVENT_TABLE()
