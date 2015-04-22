
#ifndef _VOTE_PANEL_CPP_
#define _VOTE_PANEL_CPP_

//VotePanel: shows current active vote (if any) and buttons
//for quick vote commands (!y, !b, !n)
//Designed for use in BattleRoomTab, but probably can be used anywhere
//Depends: ChatPanel

#include <wx/wx.h>
#include <wx/panel.h>

#include "gui/chatpanel.h"



class VotePanel : public wxPanel
{
public:
  VotePanel(wxWindow* parentWindow);
  ~VotePanel();
  
  void OnChatAction(const wxString& actionAuthor, const wxString& actionDescription);
  void ResetState();
  void SetChatPanel(ChatPanel* chatPanel);
  
private:
  void showButtons(bool showState);
  
  void onYesButtonEvent(wxCommandEvent& );
  void onDontCareButtonEvent(wxCommandEvent& );
  void onNoButtonEvent(wxCommandEvent& );
  
private:
  enum
  {
    ID_YES_BUTTON,
    ID_NO_BUTTON,
    ID_DONTCARE_BUTTON
  };

  const wxString VOTE_HAS_BEGAN = _T("CALLED A VOTE FOR COMMAND");
  const wxString SAY_YES 		= _T("!vote y");
  const wxString SAY_DONTCARE 	= _T("!vote b");
  const wxString SAY_NO 		= _T("!vote n");
  const wxString VOTING_END		= _T("VOTE FOR COMMAND");
  
private:
  wxBoxSizer* mainSizer;
  wxStaticText* voteTextLabel;
  wxButton* yesButton;
  wxButton* noButton;
  wxButton* dontCareButton;
  
  ChatPanel* chatPanel;
  wxWindow* parentWnd;
    
  wxDECLARE_EVENT_TABLE();
};

#endif

