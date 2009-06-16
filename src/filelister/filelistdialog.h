#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H

#ifndef NO_TORRENT_SYSTEM

#include <wx/dialog.h>
#include "../torrentwrapper.h"

class wxBoxSizer;
class wxButton;
class FileListCtrl;
class FileListFilter;
class wxStaticText;
class wxCommandEvent;


class FileListDialog : public wxDialog
{
    public:
        FileListDialog(wxWindow* parent);
        ~FileListDialog();

        void UpdateList();
        FileListCtrl* GetListCtrl();

        //void SetData(const TorrentTable& data);
        //TorrentTable &GetData();
        TorrentTable::PRow RowByHash(const wxString& hash );

    protected:
        wxBoxSizer* m_main_sizer;
        FileListCtrl* m_filelistctrl;


        // TorrentTable m_torrent_table;
        std::map<wxString, TorrentTable::PRow> m_hash_to_torrent;

        FileListFilter* m_filter;
        wxStaticText* m_filecount;
        wxButton* m_download_button;
        wxButton* m_selectAll_button;
        wxButton* m_selectNone_button;
        wxButton* m_selectInv_button;

        bool AddTorrentRow(TorrentTable::PRow row);

        void OnDownload( wxCommandEvent& event );
        void OnRefreshList( wxCommandEvent& event );

        void OnSelectAll( wxCommandEvent& event );
        void OnSelectNone( wxCommandEvent& event );
        void OnSelectInv( wxCommandEvent& event );

        enum {
            BUTTON_DOWNLOAD = wxID_HIGHEST,
            BUTTON_REFRESH,
            BUTTON_SELECT_ALL,
            BUTTON_SELECT_NONE,
            BUTTON_SELECT_INV
        };

    private:
        DECLARE_EVENT_TABLE();
};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H

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

