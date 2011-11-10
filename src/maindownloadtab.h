#ifndef MAINTORRENTTAB_H
#define MAINTORRENTTAB_H

#include <wx/scrolwin.h>
#include <map>

class wxStaticText;
class wxButton;
class DownloadListCtrl;
class Ui;
struct DownloadInfo;
class wxBoxSizer;
class ColorButton;
class WidgetDownloadDialog;


typedef std::map<wxString,DownloadInfo> map_infos;
typedef map_infos::iterator map_infos_iter;

class MainDownloadTab: public wxScrolledWindow
{
	public:

		MainDownloadTab( wxWindow* parent );
		~MainDownloadTab();

        void OnUpdate();

	protected:

		enum
		{
            ID_LIST = wxID_HIGHEST,
            ID_BUTTON_CANCEL,
            ID_OUTGOING,
            ID_INCOMING,
            ID_OUTGOING_LBL,
            ID_INCOMING_LBL,
            ID_DOWNLOAD_DIALOG,
            ID_BUTTON_CLEAR,
            ID_BUTTON_WIDGETS
        };

        wxStaticText* m_incoming;
		wxStaticText* m_incoming_lbl;
		wxStaticText* m_status_color_text;

		wxButton* m_but_cancel;
		wxButton* m_but_clear;
		//wxButton* m_but_download;
		ColorButton* m_status_color;
		DownloadListCtrl* m_dl_listctrl;
        WidgetDownloadDialog* m_widgets_dialog;

        map_infos info_map;
        void OnCancelButton( wxCommandEvent& event );
		void OnDownloadDialog( wxCommandEvent& event );
        void OnDLWidgets( wxCommandEvent& event );
        void OnClearFinished( wxCommandEvent& event );

        wxBoxSizer* m_mainbox;

	private:

		//(*Handlers(MainTorrentTab)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

