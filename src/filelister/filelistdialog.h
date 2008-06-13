#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H

#include <wx/dialog.h>
#include "../torrentwrapper.h"

class wxBoxSizer;
class wxButton;
class FileListCtrl;
class FileListFilter;
class wxStaticText;



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

        bool AddTorrentData( const TorrentData& data);

    private:
};

#endif // SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
