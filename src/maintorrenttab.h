#ifndef MAINTORRENTTAB_H
#define MAINTORRENTTAB_H

#ifndef NO_TORRENT_SYSTEM

#include <wx/panel.h>
#include <map>
#include "torrentwrapper.h"
typedef std::map<int,TorrentInfos> map_infos;
typedef map_infos::iterator map_infos_iter;

class wxStaticText;
class wxButton;
class TorrentListCtrl;
class Ui;
struct TorrentInfos;
class wxBoxSizer;

class MainTorrentTab: public wxPanel
{
	public:

		MainTorrentTab(wxWindow* parent,Ui& ui);
		~MainTorrentTab();

		wxStaticText* m_incoming;
		wxStaticText* m_outgoing;
		wxStaticText* m_incoming_lbl;
		wxStaticText* m_outgoing_lbl;

		wxButton* m_but_cancel;
		wxButton* m_but_publish;
		TorrentListCtrl* m_torrent_list;

		Ui& m_ui;
        void OnUpdate();

	protected:

		enum
		{
            ID_LIST = wxID_HIGHEST,
            ID_BUTTON_CANCEL,
            ID_BUTTON_PUB,
            ID_OUTGOING,
            ID_INCOMING,
            ID_OUTGOING_LBL,
            ID_INCOMING_LBL,

        };

        map_infos info_map;
        void AddTorrentInfo(  TorrentInfos& info );
        void UpdateInfo(  TorrentInfos& info );
        void SetInfo(int index,  TorrentInfos& info );
        void OnCancelButton( wxCommandEvent& event );

        wxBoxSizer* m_mainbox;

	private:

		//(*Handlers(MainTorrentTab)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif

#endif
