/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <string>

#include "gui/chatpanel.h"
#include "utils/conversion.h"
#include "votepanel.h"
#include "user.h"

//FIXME: move this outside of gui/ (+ don't use any gui functions / wx widgets)
static const std::string VOTE_HAS_BEGAN = "CALLED A VOTE FOR COMMAND";
static const std::string SAY_YES = "!vote y";
static const std::string SAY_DONTCARE = "!vote b";
static const std::string SAY_NO = "!vote n";
static const std::string VOTING_END = "VOTE FOR COMMAND";
static const std::string VOTE_CANCELLED = "VOTE CANCELLED";
static const std::string VOTE_CANCELED = "VOTE CANCELED";
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
    , player(0)
{
	mainSizer = new wxBoxSizer(wxHORIZONTAL);

	voteTextLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	wxFont labelFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	voteTextLabel->SetFont(labelFont);
	mainSizer->Add(voteTextLabel, 0, wxALIGN_CENTER, 0);

	wxFont buttonFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
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
	if (yesButton == nullptr || noButton == nullptr || dontCareButton == nullptr || player == nullptr) {
		return;
	}

	//Do not show this panel if player in spectator mode
	if (player->GetBattleStatus().spectator == true) {
		return;
	}

	wxString actionString = actionDescription.Upper();

	//Vote has been started
	if (actionString.find(VOTE_HAS_BEGAN) != std::string::npos) {
		onVoteBegins(actionDescription);
		return;
	}

	//Vote has ended
	if (actionString.find(VOTING_END) != std::string::npos || actionString.find(VOTE_CANCELLED) != std::string::npos ||
        actionString.find(VOTE_CANCELED) != std::string::npos || actionString.find(VOTE_CANCELLING) != std::string::npos) {
		onVoteStopped();
		return;
	}
}

//Clear state of the panel, hide buttons
void VotePanel::ResetState()
{
	if (yesButton == nullptr || noButton == nullptr || dontCareButton == nullptr) {
		return;
	}

	voteTextLabel->SetLabel(wxEmptyString);
	showButtons(false);

	//Just to be safe. Maybe this is not needed
	player = nullptr;
}

//Set pointer to ChatPanel that will receive vote commands
void VotePanel::SetChatPanel(ChatPanel* chatPanel)
{
	this->chatPanel = chatPanel;
}

//Set pointer to Player. Needed to watch it's 'spec' state. Optional.
void VotePanel::SetCurrentPlayer(User* user)
{
	player = user;
}

//Show/hide buttons of the panel
void VotePanel::showButtons(bool showState)
{
	if (yesButton == nullptr || noButton == nullptr || dontCareButton == nullptr) {
		return;
	}

	yesButton->Show(showState);
	noButton->Show(showState);
	dontCareButton->Show(showState);

	if (showState == true) {
		enableButtons();
	}

	//Force repaint parent window
	Layout();
	Update();
	parentWnd->Layout();
	parentWnd->Update();
}

//Enable/Disable buttons showing user he voted already
void VotePanel::enableButtons(bool enableState)
{
	yesButton->Enable(enableState);
	noButton->Enable(enableState);
	dontCareButton->Enable(enableState);
}

void VotePanel::onYesButtonEvent(wxCommandEvent&)
{
	chatPanel->Say(TowxString(SAY_YES));
	enableButtons(false);
}

void VotePanel::onDontCareButtonEvent(wxCommandEvent&)
{
	chatPanel->Say(TowxString(SAY_DONTCARE));
	enableButtons(false);
}

void VotePanel::onNoButtonEvent(wxCommandEvent&)
{
	chatPanel->Say(TowxString(SAY_NO));
	enableButtons(false);
}

void VotePanel::onVoteBegins(const wxString& msg)
{
	//Parse vote description string to find text rounded by quotes
	wxString voteDescr = "Vote for: " + msg.AfterFirst('"').BeforeFirst('"') + " ";

	showButtons(true);
	voteTextLabel->SetLabel(voteDescr);
	parentWnd->Layout();
	parentWnd->Update();
}

void VotePanel::onVoteStopped()
{
	ResetState();
}
