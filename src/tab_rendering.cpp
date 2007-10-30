#include "tabs.h"

void rendering_panel::initRendererSizer(wxStaticBoxSizer* sizerL,wxStaticBoxSizer* sizerR ) {
	const int extrema[] = {
		1024, 4096,   600, 3000,   20, 120,   100, 600,   0, 10,   0, 100,   100, 1000,   1000, 20000,   1, 20
	};

	// i < "sizeof"(RO_SLI)
	for (int i = 0; i <= 4; i++) {
		wxSlider* slider = new wxSlider(
			this, RO_SLI[i].id, configHandler.GetInt(RO_SLI[i].key,fromString<int>(RO_SLI[i].def)),
			extrema[i * 2],  extrema[(i * 2) + 1], WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V
		);
		slider->SetTickFreq((extrema[(i*2)+1] - extrema[i * 2]) / 10  ,1);
		sizerL->Add(new wxStaticText(this, _1,  _S(RO_SLI[i].lbl)), 0, wxTOP, 15);
		sizerL->Add(slider, 0, wxALIGN_RIGHT, 0);
	}
	for (int i = 5; i <= 8; i++) {
		wxSlider* slider = new wxSlider(
			this, RO_SLI[i].id, configHandler.GetInt(RO_SLI[i].key,fromString<int>(RO_SLI[i].def)),
			extrema[i * 2],  extrema[(i * 2) + 1], WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V
		);
		slider->SetTickFreq((extrema[(i*2)+1] - extrema[i * 2]) / 10  ,1);
		sizerR->Add(new wxStaticText(this, _1,  _S(RO_SLI[i].lbl)), 0, wxTOP, 15);
		sizerR->Add(slider, 0, wxALIGN_RIGHT, 0);
	}
}

rendering_panel::rendering_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxSizer* parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	wxSizer* childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer
	wxSizer* childRSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
	wxStaticBoxSizer* rendererSizerA = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Rendering Options A"), WX_DEF_P, wxSize(220, 300), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* rendererSizerB = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Rendering Options B"), WX_DEF_P, wxSize(220, 300), 0, _S("")), wxVERTICAL);

	initRendererSizer(rendererSizerA,rendererSizerB);
	

	//childLSizer->Add(0, 5, 0);
	childLSizer->Add(rendererSizerA);
	//childRSizer->Add(0, 10, 0);
	childRSizer->Add(rendererSizerB);

	parentSizer->Add(5, 0, 0);
	parentSizer->Add(childLSizer);
	
	parentSizer->Add(25, 0, 0);
	parentSizer->Add(childRSizer);

	SetSizer(parentSizer);

}

rendering_panel::~rendering_panel(void) {
	
}

BEGIN_EVENT_TABLE(rendering_panel, abstract_panel)
	EVT_SLIDER(wxID_ANY,            rendering_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              rendering_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          rendering_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       rendering_panel::OnRadioButtonToggle)
	EVT_IDLE(                       rendering_panel::update)
END_EVENT_TABLE()
