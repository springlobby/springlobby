#ifndef LOBBYOPTIONSTAB_H
#define LOBBYOPTIONSTAB_H

#include <wx/scrolwin.h>

class wxCheckBox;
class wxBoxSizer;
class wxStaticText;
class wxRadioButton;


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

        wxStaticText* m_complete_method_label;
        wxRadioButton* m_complete_method_old;
        wxRadioButton* m_complete_method_new;


        wxBoxSizer* m_main_sizer;


};

#endif // LOBBYOPTIONSTAB_H
