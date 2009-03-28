#ifndef MAINTORRENTTAB_H
#define MAINTORRENTTAB_H

#ifndef NO_TORRENT_SYSTEM

#include <wx/scrolwin.h>
#include <map>

class wxStaticText;
class wxButton;
class TorrentListCtrl;
class Ui;
struct TorrentInfos;
class wxBoxSizer;
class FileListDialog;
class ColorButton;

typedef std::map<int,TorrentInfos> map_infos;
typedef map_infos::iterator map_infos_iter;

class MainTorrentTab: public wxScrolledWindow
{
	public:

		MainTorrentTab(wxWindow* parent,Ui& ui);
		~MainTorrentTab();

		wxStaticText* m_incoming;
		wxStaticText* m_outgoing;
		wxStaticText* m_incoming_lbl;
		wxStaticText* m_outgoing_lbl;
		wxStaticText* m_status_color_text;

		wxButton* m_but_cancel;
		wxButton* m_but_publish;
		wxButton* m_but_download;
		ColorButton* m_status_color;
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
            ID_DOWNLOAD_DIALOG

        };

        map_infos info_map;
        void AddTorrentInfo(  TorrentInfos& info );
        void UpdateInfo(  TorrentInfos& info );
        void SetInfo(int index,  TorrentInfos& info );
        void OnCancelButton( wxCommandEvent& event );
        void OnDownloadDialog( wxCommandEvent& event );

        wxBoxSizer* m_mainbox;
        FileListDialog* m_download_dialog;

	private:

		//(*Handlers(MainTorrentTab)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif

#endif

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

