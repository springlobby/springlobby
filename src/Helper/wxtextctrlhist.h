//copied from http://wxforum.shadonet.com/viewtopic.php?t=7924 (thanks go to Lamego)

// Extends wxTextCtrl with input historical functions
// to allow user to navigate to previously entered messages
class wxTextCtrlHist : public wxTextCtrl
{
  public:
    wxTextCtrlHist(wxWindow* parent, wxWindowID id, const wxString& value = _(""),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    void SetHistoryMax(int max) { history_max = max; }
  private:
    int current_pos;
    int history_max;
    wxArrayString Historical;
    void OnSendMessage( wxCommandEvent &event );
    void OnChar(wxKeyEvent & event);
    DECLARE_EVENT_TABLE()
};
