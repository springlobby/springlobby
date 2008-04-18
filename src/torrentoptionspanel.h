#ifndef SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
#define SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED

#include <wx/panel.h>

class wxCheckBox;
class wxTextCtrl;


class TorrentOptionsPanel: public wxPanel
{
    public:
        TorrentOptionsPanel( wxWindow* parent);
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

#endif // SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
