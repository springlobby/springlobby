#include "filelistdialog.h"
#include "filelistctrl.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

BEGIN_EVENT_TABLE(FileListDialog, wxDialog)

  EVT_BUTTON    ( BUTTON_DOWNLOAD   , FileListDialog::OnDownload        )
  EVT_BUTTON    ( BUTTON_REFRESH    , FileListDialog::OnRefreshList        )

END_EVENT_TABLE()

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

    wxBoxSizer* m_button_sizer;
    m_button_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_download_button = new wxButton( this, BUTTON_DOWNLOAD, _("Download selected"), wxDefaultPosition, wxSize( -1,28 ), 0 );
    m_button_sizer->Add( m_download_button );

    SetData( torrent()->GetSystemFileList() );
    UpdateList();



    m_main_sizer->Add( m_list_sizer,1, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_filter_sizer,0, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_button_sizer,0, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_filecount,0, wxALL|wxEXPAND, 5 );
    SetSizer( m_main_sizer );
}

FileListDialog::~FileListDialog()
{

}
void FileListDialog::UpdateList()
{
    m_filelistctrl->DeleteAllItems();
    unsigned int count = 0;
    HashToTorrentData::const_iterator it = m_torrentdata.begin();
    for ( ; it != m_torrentdata.end(); ++it)
    {
        count += AddTorrentData( it->second );
    }
    m_filecount->SetLabel( wxString::Format( _("%u files displayed"), count ) );
    m_filelistctrl->SetColumnWidths();
}

void FileListDialog::SetData(HashToTorrentData& data )
{
    m_torrentdata = data;
}

bool FileListDialog::AddTorrentData( const TorrentData& data)
{
    if ( !m_filter->FilterTorrentData( data ) )
        return false;
    try
    {
        int index = m_filelistctrl->InsertItem( m_filelistctrl->GetItemCount(), data.hash);
        //setting hash as item's data means we can retrieve it later for download
        m_filelistctrl->SetItemText( index, data.hash );
        m_filelistctrl->SetItem( index, 0, data.name );
        m_filelistctrl->SetItem( index, 1, data.type == map ? _("Map") : _("Mod") );
        m_filelistctrl->SetItem( index, 2, data.hash );
    } catch (...) { return false; }
    return true;
}

void FileListDialog::OnDownload( wxCommandEvent& event )
{
    typedef FileListCtrl::HashVector HashVector;
    HashVector hashs;
    m_filelistctrl->GetSelectedHashes(hashs);
    for ( HashVector::const_iterator it = hashs.begin(); it != hashs.end(); ++it)
    {
        wxString hash = *it;
        if (torrent()->RequestFileByName(hash) != success)
            wxLogError(_("unknown hash ") + hash );

    }

}

void FileListDialog::OnRefreshList( wxCommandEvent& event )
{

}
