#include "filelistdialog.h"
#include "filelistctrl.h"


FileListDialog::FileListDialog(wxWindow* parent) :
    wxDialog(parent, -1, _("Filestufflistchangemeplease"), wxDefaultPosition, wxSize(800, 600),
           wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER)
{
    m_filelistctrl = new FileListCtrl( this );
    SetData( torrent()->GetSystemFileList() );
    UpdateList();
}

FileListDialog::~FileListDialog()
{

}
void FileListDialog::UpdateList()
{
    HashToTorrentData::const_iterator it = m_torrentdata.begin();
    for ( ; it != m_torrentdata.end(); ++it)
    {
        AddTorrentData( it->second );
    }
}

void FileListDialog::SetData(HashToTorrentData& data )
{
    m_torrentdata = data;
}

void FileListDialog::AddTorrentData( const TorrentData& data)
{
  int index = m_filelistctrl->InsertItem( m_filelistctrl->GetItemCount(), data.name);
  m_filelistctrl->SetItem( index, 0, data.name );
  m_filelistctrl->SetItem( index, 1, data.type == map ? _T("Map") : _T("Mod") );
  m_filelistctrl->SetItem( index, 2, data.hash );
}
