#ifndef MAINTORRENTTAB_H
#define MAINTORRENTTAB_H

//(*Headers(MainTorrentTab)
#include <wx/panel.h>
//*)

class wxStaticText;
class wxButton;
class TorrentListCtrl;
class Ui;
class TorrentInfos;

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

        void AddTorrentInfo( TorrentInfos& info );

	private:

		//(*Handlers(MainTorrentTab)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
