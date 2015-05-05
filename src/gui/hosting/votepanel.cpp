/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <string>

#include "gui/chatpanel.h"
#include "utils/conversion.h"
#include "votepanel.h"


//FIXME: move this outside of gui/ (+ don't use any gui functions / wx widgets)
static const std::string VOTE_HAS_BEGAN = "CALLED A VOTE FOR COMMAND";
static const std::string SAY_YES = "!vote y";
static const std::string SAY_DONTCARE = "!vote b";
static const std::string SAY_NO = "!vote n";
static const std::string VOTING_END = "VOTE FOR COMMAND";
static const std::string VOTE_CANCELLED = "VOTE CANCELLED";
static const std::string VOTE_CANCELLING = "CANCELLING";

wxBEGIN_EVENT_TABLE(VotePanel, wxPanel)
    EVT_BUTTON(VotePanel::ID_YES_BUTTON, VotePanel::onYesButtonEvent)
    EVT_BUTTON(VotePanel::ID_DONTCARE_BUTTON, VotePanel::onDontCareButtonEvent)
    EVT_BUTTON(VotePanel::ID_NO_BUTTON, VotePanel::onNoButtonEvent)
    wxEND_EVENT_TABLE()


    VotePanel::VotePanel(wxWindow* parentWindow)
    : wxPanel(parentWindow)
    , chatPanel(0)
    , parentWnd(parentWindow)
{
	mainSizer = new wxBoxSizer(wxHORIZONTAL);

	voteTextLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	wxFont labelFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	voteTextLabel->SetFont(labelFont);
	mainSizer->Add(voteTextLabel, 0, wxALIGN_CENTER, 2);

	wxFont buttonFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	yesButton = new wxButton(this, VotePanel::ID_YES_BUTTON, "yes", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	yesButton->SetFont(buttonFont);
	yesButton->SetForegroundColour(*wxGREEN);
	yesButton->SetToolTip(_("Vote for YES, (Ctrl-Y)"));
	mainSizer->Add(yesButton);

	dontCareButton = new wxButton(this, VotePanel::ID_DONTCARE_BUTTON, "don't care", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	dontCareButton->SetFont(buttonFont);
	dontCareButton->SetToolTip(_("Vote for DON'T CARE, (Ctrl-B)"));
	mainSizer->Add(dontCareButton);

	noButton = new wxButton(this, VotePanel::ID_NO_BUTTON, "no", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	noButton->SetFont(buttonFont);
	noButton->SetForegroundColour(*wxRED);
	noButton->SetToolTip(_("Vote for NO, (Ctrl-N)"));
	mainSizer->Add(noButton);

	SetSizer(mainSizer);

	//Set up acceleration table for hotkeys
	const int ENTRIES_COUNT = 3;

	wxAcceleratorEntry accelEntries[ENTRIES_COUNT];
	accelEntries[0].Set(wxACCEL_CTRL, (int)'Y', ID_YES_BUTTON);
	accelEntries[1].Set(wxACCEL_CTRL, (int)'B', ID_DONTCARE_BUTTON);
	accelEntries[2].Set(wxACCEL_CTRL, (int)'N', ID_NO_BUTTON);

	wxAcceleratorTable accelTable(ENTRIES_COUNT, accelEntries);
	//TODO: this line overrides parent's window acceleration table!
	parentWindow->SetAcceleratorTable(accelTable);

	//Reset vote panel to "no vote" state
	ResetState();
}

VotePanel::~VotePanel()
{
}

//Process text messages from chat
void VotePanel::OnChatAction(const wxString& /*actionAuthor*/, const wxString& actionDescription)
{
	if (yesButton == NULL || noButton == NULL || dontCareButton == NULL) {
		return;
	}

	wxString actionString = actionDescription.Upper();

	//Vote has been started
	if (actionString.find(VOTE_HAS_BEGAN) != std::string::npos) {
		showButtons(true);
		voteTextLabel->SetLabel(actionDescription);
		parentWnd->Layout();
		parentWnd->Update();
		return;
	}

	//Vote has ended
	if (actionString.find(VOTING_END) != std::string::npos || actionString.find(VOTE_CANCELLED) != std::string::npos ||
	    actionString.find(VOTE_CANCELLING) != std::string::npos) {
		ResetState();
		return;
	}
}

//Clear state of the panel, hide buttons
void VotePanel::ResetState()
{
	if (yesButton == NULL || noButton == NULL || dontCareButton == NULL) {
		return;
	}

	voteTextLabel->SetLabel(wxEmptyString);
	showButtons(false);
}

//Set pointer to ChatPanel that will receive vote commands
void VotePanel::SetChatPanel(ChatPanel* chatPanel)
{
	this->chatPanel = chatPanel;
}

//Show/hide buttons of the panel
void VotePanel::showButtons(bool showState)
{
	if (yesButton == NULL || noButton == NULL || dontCareButton == NULL) {
		return;
	}

	yesButton->Show(showState);
	noButton->Show(showState);
	dontCareButton->Show(showState);

	//Force repaint parent window
	Layout();
	Update();
	parentWnd->Layout();
	parentWnd->Update();
}

void VotePanel::onYesButtonEvent(wxCommandEvent&)
{
	chatPanel->Say(TowxString(SAY_YES));
	showButtons(false);
}

void VotePanel::onDontCareButtonEvent(wxCommandEvent&)
{
	chatPanel->Say(TowxString(SAY_DONTCARE));
	showButtons(false);
}

void VotePanel::onNoButtonEvent(wxCommandEvent&)
{
	chatPanel->Say(TowxString(SAY_NO));
	showButtons(false);
}
