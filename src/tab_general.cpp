#include "tabs.h"

void general_panel::initGeneralSizer(wxStaticBoxSizer* sizer) {
	// i < "sizeof"(GO_CBOX)
	for (int i = 0; i <= 21; i++) {
		wxCheckBox* checkBox = new wxCheckBox(this, GO_CBOX[i].id, _S(GO_CBOX[i].lbl));
		checkBox->SetValue(configHandler.GetInt(GO_CBOX[i].key,fromString<int>(GO_CBOX[i].def)));

		sizer->Add(checkBox, 0, wxTOP, ((i == 0)? 10: 0));
	}
}

general_panel::general_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxSizer* parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	wxSizer* childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
	wxStaticBoxSizer* generalSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("General Options"), WX_DEF_P, wxSize(220, 300), 0, _S("")), wxVERTICAL);

	initGeneralSizer(generalSizer);

	childLSizer->Add(0, 5, 0);
	childLSizer->Add(generalSizer);

	parentSizer->Add(10, 0, 0);
	parentSizer->Add(childLSizer);

	SetSizer(parentSizer);

}

general_panel::~general_panel(void) {

}


BEGIN_EVENT_TABLE(general_panel, abstract_panel)
	EVT_SLIDER(wxID_ANY,            general_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              general_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          general_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       general_panel::OnRadioButtonToggle)
	EVT_IDLE(                       general_panel::update)
END_EVENT_TABLE()
