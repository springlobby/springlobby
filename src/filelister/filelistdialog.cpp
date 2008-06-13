#include "filelistdialog.h"
#include "filelistctrl.h"
#include <wx/sizer.h>
#include <wx/stattext.h>


FileListDialog::FileListDialog(wxWindow* parent) :
    wxDialog(parent, -1, _("Filestufflistchangemeplease"), wxDefaultPosition, wxSize(800, 600),
           wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER)
{
    m_filecount = new wxStaticText( this, wxID_ANY, _("Files displayed") );

    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* m_list_sizer;
    m_list_sizer = new wxBoxSizer( wxVERTICAL );
    m_filelistctrl = new FileListCtrl( this );
    m_list_sizer->Add( m_filelistctrl, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* m_filter_sizer;
    m_filter_sizer = new wxBoxSizer( wxVERTICAL );
    m_filter = new FileListFilter ( this, wxID_ANY, this );
    m_filter_sizer->Add( m_filter,0, wxALL|wxEXPAND, 5 );

    SetData( torrent()->GetSystemFileList() );
    UpdateList();



    m_main_sizer->Add( m_list_sizer,1, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_filter_sizer,0, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_filecount,0, wxALL|wxEXPAND, 5 );
    SetSizer( m_main_sizer );
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
    m_filecount->SetLabel( wxString::Format( _T("%u files displayed"), m_torrentdata.size() ) );
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
