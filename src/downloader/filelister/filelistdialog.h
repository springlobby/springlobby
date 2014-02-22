/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H

#ifndef NO_TORRENT_SYSTEM

#include <wx/dialog.h>
#include <utils/globalevents.h>

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

        void UpdateList( GlobalEvents::GlobalEventData );
//        FileListCtrl* GetListCtrl();
    protected:
        wxBoxSizer* m_main_sizer;
        FileListCtrl* m_filelistctrl;

        FileListFilter* m_filter;
        wxStaticText* m_filecount;
        wxButton* m_download_button;
        wxButton* m_selectAll_button;
        wxButton* m_selectNone_button;
        wxButton* m_selectInv_button;

        typedef EventReceiverFunc<FileListDialog, GlobalEvents::GlobalEventData, &FileListDialog::UpdateList>
            EventReceiverFunction;
        EventReceiverFunction m_onResourceListParsed;


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
		DECLARE_EVENT_TABLE()
};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_FILELISTDIALOG_H

