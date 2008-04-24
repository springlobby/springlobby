#ifndef SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
#define SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED

#ifndef NO_TORRENT_SYSTEM

#include <wx/panel.h>

class wxCheckBox;
class wxTextCtrl;
class Ui;


class TorrentOptionsPanel: public wxPanel
{
    public:
        TorrentOptionsPanel( wxWindow* parent, Ui& ui);
        ~TorrentOptionsPanel();
        void OnEnableP2P( wxCommandEvent& event );
        void OnMaxUp( wxCommandEvent& event );
        void OnMaxDown( wxCommandEvent& event );
        void OnP2PPort( wxCommandEvent& event );
        void OnMaxConnections( wxCommandEvent& event );
        void OnApply( wxCommandEvent& event );
        void OnRestore( wxCommandEvent& event );

    private:
        wxCheckBox* m_enableP2P;
        wxTextCtrl* m_maxUp;
        wxTextCtrl* m_maxDown;
        wxTextCtrl* m_p2pport;
        wxTextCtrl* m_maxConnections;

        Ui& m_ui;

        void EnableSettings( bool enable);


        enum
        {
            ID_ENABLEP2P = wxID_HIGHEST,
            ID_MAXUP,
            ID_MAXDOWN,
            ID_P2PPORT,
            ID_MAXCONNECTIONS,
            ID_APPLY,
            ID_RESTORE
        };

        DECLARE_EVENT_TABLE()
};

#endif

#endif // SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
