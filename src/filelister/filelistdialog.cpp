#include "filelistdialog.h"
#include "filelistctrl.h"

FileListDialog::FileListDialog(wxWindow* parent) :
    wxDialog(parent, -1, _("Filestufflistchangemeplease"), wxDefaultPosition, wxSize(300, 300),
           wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN )
{
    m_filelistctrl = new FileListCtrl( this );
}

FileListDialog::~FileListDialog()
{

}
void FileListDialog::UpdateList()
{

}
