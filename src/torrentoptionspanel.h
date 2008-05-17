#ifndef SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
#define SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED

#ifndef NO_TORRENT_SYSTEM

#include <wx/panel.h>

class wxCheckBox;
class wxTextCtrl;
class Ui;
class wxStaticBox;
class wxRadioButton;
class wxButton;
class wxStaticText;

class TorrentOptionsPanel: public wxPanel
{
    public:
        TorrentOptionsPanel( wxWindow* parent, Ui& ui);
        ~TorrentOptionsPanel();

        //void OnEnableP2P( wxCommandEvent& event );
        void OnMaxUp( wxCommandEvent& event );
        void OnMaxDown( wxCommandEvent& event );
        void OnP2PPort( wxCommandEvent& event );
        void OnMaxConnections( wxCommandEvent& event );
        void OnApply( wxCommandEvent& event );
        void OnManStart( wxCommandEvent& event );
        void OnManStop( wxCommandEvent& event );
        void OnRestore( wxCommandEvent& event );
        void OnStatusUpdate( wxCommandEvent& event );

    private:
        wxTextCtrl* m_maxUp;
        wxTextCtrl* m_maxDown;
        wxTextCtrl* m_p2pport;
        wxTextCtrl* m_maxConnections;
        wxStaticBox* m_autostart_box;
        wxStaticBox* m_gamestart_box;
        wxRadioButton* m_autostart_start;
        wxRadioButton* m_autostart_logon;
        wxRadioButton* m_autostart_manual;
        wxRadioButton* m_gamestart_pause;
        wxRadioButton* m_gamestart_throttle;
        wxTextCtrl* m_gamestart_throttle_up;
        wxTextCtrl* m_gamestart_throttle_down;
        wxButton* m_sys_start;
        wxButton* m_sys_stop;
        wxButton* m_status_color;
        wxStaticText* m_gamestart_throttle_up_lbl;
        wxStaticText* m_gamestart_throttle_down_lbl;
        wxStaticText* m_status_color_text;

        Ui& m_ui;


        void EnableSettings( bool enable);
        void EnableStartStopButtons( bool system_is_running);
        void SetStatusDisplay();


        enum
        {
            ID_ENABLEP2P = wxID_HIGHEST,
            ID_MAXUP,
            ID_MAXDOWN,
            ID_P2PPORT,
            ID_MAXCONNECTIONS,
            ID_APPLY,
            ID_RESTORE,
            ID_AUTOSTART_RADIO,
            ID_GAMESTART_RADIO,
            ID_MAN_START,
            ID_MAN_STOP,
            ID_INGAME_UP,
            ID_INGAME_DOWN
        };

        DECLARE_EVENT_TABLE()
};

#endif

#endif // SPRINGLOBBY_HEADER_GUARD_TORRENTOPTIONSPANEL_H_INCLUDED
