#ifndef LOBBYOPTIONSTAB_H
#define LOBBYOPTIONSTAB_H

#include <wx/scrolwin.h>

class wxCheckBox;
class wxBoxSizer;
class wxStaticText;


class LobbyOptionsTab : public wxScrolledWindow
{
    public:
        LobbyOptionsTab(wxWindow* parent);
        virtual ~LobbyOptionsTab();

        void OnApply( wxCommandEvent& event );
        void OnRestore( wxCommandEvent& event );

    protected:

        wxCheckBox* m_autojoin;
        wxCheckBox* m_reportstats;
        wxCheckBox* m_updater;
        wxCheckBox* m_show_tooltips;

        wxStaticText* m_autoconnect_label;
        wxStaticText* m_reportstats_label;
        wxStaticText* m_updater_label;
        wxStaticText* m_show_tooltips_label;

        wxBoxSizer* m_main_sizer;


};

#endif // LOBBYOPTIONSTAB_H
