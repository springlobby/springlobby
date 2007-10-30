#include "tabs.h"

void video_panel::initVideoSizer(wxStaticBoxSizer* sizer) {
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


	wxTextCtrl* xResInput = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_X, wxT(""), WX_DEF_P, wxSize(60, 20), 0);
	wxTextCtrl* yResInput = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_Y, wxT(""), WX_DEF_P, wxSize(60, 20), 0);

    std::string s;
     toString<int>(s,configHandler.GetInt(RC_TEXT[0].key,fromString<int>(RC_TEXT[0].def))); 
	xResInput->ChangeValue(_S(s.c_str()));
     toString<int>(s,configHandler.GetInt(RC_TEXT[1].key,fromString<int>(RC_TEXT[1].def))); 
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

video_panel::video_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxSizer* parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	wxSizer* childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
	wxStaticBoxSizer* videoSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Rendering Options"), WX_DEF_P, wxSize(220, 300), 0, _S("")), wxVERTICAL);

	initVideoSizer(videoSizer);


	childLSizer->Add(0, 5, 0);
	childLSizer->Add(videoSizer);

	parentSizer->Add(10, 0, 0);
	parentSizer->Add(childLSizer);

	SetSizer(parentSizer);

}

video_panel::~video_panel(void) {

}

BEGIN_EVENT_TABLE(video_panel, abstract_panel)
	EVT_SLIDER(wxID_ANY,            video_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              video_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          video_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       video_panel::OnRadioButtonToggle)
	EVT_IDLE(                       video_panel::update)
END_EVENT_TABLE()
