
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
  
  voteTextLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
  wxFont labelFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  voteTextLabel->SetFont(labelFont);
  mainSizer->Add(voteTextLabel, 0, wxALIGN_CENTER, 2);
   
  wxFont buttonFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  yesButton = new wxButton(this, VotePanel::ID_YES_BUTTON, "yes", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  yesButton->SetFont(buttonFont);
  yesButton->SetForegroundColour(*wxGREEN);
  mainSizer->Add(yesButton);
 
  dontCareButton = new wxButton(this, VotePanel::ID_DONTCARE_BUTTON, "ban", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  dontCareButton->SetFont(buttonFont);
  mainSizer->Add(dontCareButton);

  noButton = new wxButton(this, VotePanel::ID_NO_BUTTON, "no", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  noButton->SetFont(buttonFont);
  noButton->SetForegroundColour(*wxRED);
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
    showButtons(true);
    voteTextLabel->SetLabel(actionDescription);
    parentWnd->Layout();
    parentWnd->Update();
    return;
  }
  
  //Vote has ended
  if( actionString.Find(VOTING_END)!=wxNOT_FOUND || actionString.Find(VOTE_CANCELLED)!=wxNOT_FOUND ) {
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
  
  //Force repaint parent window
  Layout();
  Update();
  parentWnd->Layout();
  parentWnd->Update();
}

void VotePanel::onYesButtonEvent(wxCommandEvent& )
{
  chatPanel->Say(SAY_YES);
  showButtons(false);
}

void VotePanel::onDontCareButtonEvent(wxCommandEvent& )
{
  chatPanel->Say(SAY_DONTCARE);
  showButtons(false);
}

void VotePanel::onNoButtonEvent(wxCommandEvent& )
{
  chatPanel->Say(SAY_NO);
  showButtons(false);
}
