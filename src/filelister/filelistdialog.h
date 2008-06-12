#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H

#include <wx/dialog.h>

class wxBoxSizer;
class wxButton;
class FileListCtrl;



class FileListDialog : public wxDialog
{
    public:
        FileListDialog(wxWindow* parent);
        ~FileListDialog();

        void UpdateList();

    protected:
        wxBoxSizer* m_main_sizer;
        FileListCtrl* m_filelistctrl;

    private:
};

#endif // SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
