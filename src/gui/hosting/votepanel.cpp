
#include "votepanel.h"

wxBEGIN_EVENT_TABLE(VotePanel, wxPanel)
  EVT_BUTTON(VotePanel::ID_YES_BUTTON, VotePanel::onYesButtonEvent)
  EVT_BUTTON(VotePanel::ID_DONTCARE_BUTTON, VotePanel::onDontCareButtonEvent)
  EVT_BUTTON(VotePanel::ID_NO_BUTTON, VotePanel::onNoButtonEvent)
wxEND_EVENT_TABLE()



VotePanel::VotePanel(wxWindow* parentWindow) : wxPanel(parentWindow),
chatPanel(0),
parentWnd(parentWindow)
{
  mainSizer = new wxBoxSizer(wxHORIZONTAL);
  
  voteTextLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
  wxFont labelFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  voteTextLabel->SetFont(labelFont);
  voteTextLabel->Wrap(500);
  mainSizer->Add(voteTextLabel, 0, wxALIGN_CENTER, 2);
    
  yesButton = new wxButton(this, VotePanel::ID_YES_BUTTON, "!Y", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  mainSizer->Add(yesButton);
 
  dontCareButton = new wxButton(this, VotePanel::ID_DONTCARE_BUTTON, "!B", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  mainSizer->Add(dontCareButton);

  noButton = new wxButton(this, VotePanel::ID_NO_BUTTON, "!N", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  mainSizer->Add(noButton);
  
  SetSizer(mainSizer);
  
  ResetState();
}

VotePanel::~VotePanel()
{

}

//Process text messages from chat
void VotePanel::OnChatAction(const wxString& actionAuthor, const wxString& actionDescription)
{
  if( yesButton==NULL || noButton==NULL || dontCareButton==NULL ) {
    return;
  }

  wxString actionString = actionDescription.Upper();
  
  //Vote has been started
  if( actionString.Find(VOTE_HAS_BEGAN)!=wxNOT_FOUND ) {
    meAlreadyVoted = false;
    showButtons(true);
    voteTextLabel->SetLabel(actionDescription);
    parentWnd->Layout();
    parentWnd->Update();
    return;
  }
  
  //Vote has ended
  if( actionString.Find(VOTING_END)!=wxNOT_FOUND ) {
    ResetState();
    return;
  }
}

//Clear state of the panel, hide buttons
void VotePanel::ResetState()
{
  if( yesButton==NULL || noButton==NULL || dontCareButton==NULL ) {
    return;
  }
  
  voteTextLabel->SetLabel(wxEmptyString);
  
  meAlreadyVoted = false;
  
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
  if( yesButton==NULL || noButton==NULL || dontCareButton==NULL ) {
    return;
  }
  
  yesButton->Show(showState);
  noButton->Show(showState);
  dontCareButton->Show(showState);
  Layout();
  Update();
  parentWnd->Layout();
  parentWnd->Update();
}

void VotePanel::onYesButtonEvent(wxCommandEvent& )
{
  chatPanel->Say(SAY_YES);
  ResetState();
}

void VotePanel::onDontCareButtonEvent(wxCommandEvent& )
{
  chatPanel->Say(SAY_DONTCARE);
  ResetState();
}

void VotePanel::onNoButtonEvent(wxCommandEvent& )
{
  chatPanel->Say(SAY_NO);
  ResetState();  
}
