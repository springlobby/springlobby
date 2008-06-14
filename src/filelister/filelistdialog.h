#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H

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
        void SetData(HashToTorrentData& data);

    protected:
        wxBoxSizer* m_main_sizer;
        FileListCtrl* m_filelistctrl;
        HashToTorrentData m_torrentdata;
        FileListFilter* m_filter;
        wxStaticText* m_filecount;
        wxButton* m_download_button;

        bool AddTorrentData( const TorrentData& data);
        void OnDownload( wxCommandEvent& event );
        void OnRefreshList( wxCommandEvent& event );

        enum {
            BUTTON_DOWNLOAD = wxID_HIGHEST,
            BUTTON_REFRESH
        };

    private:
        DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
