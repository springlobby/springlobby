#ifndef NO_TORRENT_SYSTEM

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include "filelistdialog.h"
#include "filelistctrl.h"
#include "../iunitsync.h"
#include "../utils/conversion.h"
#include "../utils/downloader.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

BEGIN_EVENT_TABLE(FileListDialog, wxDialog)

  EVT_BUTTON    ( BUTTON_DOWNLOAD   , FileListDialog::OnDownload    )
  EVT_BUTTON    ( BUTTON_REFRESH    , FileListDialog::OnRefreshList )
  EVT_BUTTON    ( BUTTON_SELECT_ALL , FileListDialog::OnSelectAll   )
  EVT_BUTTON    ( BUTTON_SELECT_NONE, FileListDialog::OnSelectNone  )
  EVT_BUTTON    ( BUTTON_SELECT_INV , FileListDialog::OnSelectInv   )


END_EVENT_TABLE()

FileListDialog::FileListDialog(wxWindow* parent)
    : wxDialog(parent, -1, _("Search and download files"), wxDefaultPosition, wxSize(800, 600),
           wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER),
    m_onResourceListParsed( this, &GetGlobalEventSender( GlobalEvents::PlasmaResourceListParsed ) )
{
    m_filecount = new wxStaticText( this, wxID_ANY, _("Files displayed") );

    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* m_list_sizer;
    m_list_sizer = new wxBoxSizer( wxVERTICAL );
    m_filelistctrl = new FileListCtrl( this );
    m_list_sizer->Add( m_filelistctrl, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* m_select_sizer = new wxBoxSizer( wxHORIZONTAL );
/*
    m_selectAll_button = new wxButton( this, BUTTON_SELECT_ALL, _T("Select all") );
    m_selectNone_button = new wxButton( this, BUTTON_SELECT_NONE, _T("Select none") );
    m_selectInv_button = new wxButton( this, BUTTON_SELECT_INV, _T("Invert selection") );
    m_select_sizer->Add( m_selectAll_button );
    m_select_sizer->Add( m_selectNone_button );
    m_select_sizer->Add( m_selectInv_button );
*/
    wxBoxSizer* m_filter_sizer;
    m_filter_sizer = new wxBoxSizer( wxVERTICAL );
    m_filter = new FileListFilter ( this, wxID_ANY, this );
    m_filter_sizer->Add( m_filter,0, wxALL|wxEXPAND, 5 );

    wxBoxSizer* m_button_sizer;
    m_button_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_download_button = new wxButton( this, BUTTON_DOWNLOAD, _("Download selected"), wxDefaultPosition, wxSize( -1,28 ), 0 );
    m_button_sizer->Add( m_download_button );


    //needs dummy event data
    UpdateList( GlobalEvents::GlobalEventData() );

    m_main_sizer->Add( m_list_sizer,1, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_select_sizer,0, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_filter_sizer,0, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_button_sizer,0, wxALL|wxEXPAND, 5 );
    m_main_sizer->Add( m_filecount,0, wxALL|wxEXPAND, 5 );
    SetSizer( m_main_sizer );
    Refresh();
}

FileListDialog::~FileListDialog()
{

}

FileListCtrl* FileListDialog::GetListCtrl()
{
    return m_filelistctrl;
}

void FileListDialog::UpdateList( GlobalEvents::GlobalEventData )
{
    unsigned int count = 0;

    const PlasmaInterface::ResourceList& rl = plasmaInterface().GetResourceList();
    for ( PlasmaInterface::ResourceList::const_iterator i = rl.begin(); i != rl.end(); ++i ){
//        if ( !m_filter->DoFilterResource( *i ) )
            count += m_filelistctrl->AddFile( &(*i) );
//            if ( count > 400 )
//                break;
    }
    m_filecount->SetLabel( wxString::Format( _("%u files displayed"), m_filelistctrl->GetItemCount() ) );
    m_filelistctrl->SortList();
    m_filelistctrl->RefreshVisibleItems();
}

//bool FileListDialog::AddTorrentRow(TorrentTable::PRow data)
//{
//  if(!data.ok())return false;
//
//  if ( !m_filter->FilterTorrentData( data ) )
//      return false;
//  try
//  {
//      int index = m_filelistctrl->InsertItem( m_filelistctrl->GetItemCount(), data->hash);
//
//      //this enables me to later retrieve the index from itemtext (used in sort funcs)
//      m_filelistctrl->SetItemText( index, data->name );
//
//      //setting hash as item's data means we can retrieve it later for download
//      m_filelistctrl->SetItemData( index, (unsigned int)FromwxString<long>(data->hash) );
//      m_filelistctrl->SetItem( index, 0, data->name );
//      m_filelistctrl->SetItem( index, 1, data->type == IUnitSync::map ? _("Map") : _("Mod") );
//      m_filelistctrl->SetItem( index, 2, data->hash );
//
//  } catch (...) { return false; }
//  return true;
//}

void FileListDialog::OnDownload( wxCommandEvent& /*unused*/ )
{
    typedef FileListCtrl::InternalNameVector InternalNameVector;
    InternalNameVector names;
    m_filelistctrl->GetSelectedHashes(names);
    for ( InternalNameVector::const_iterator it = names.begin(); it != names.end(); ++it)
    {
        wxString name = *it;
        if (torrent().RequestFileByName(name) != TorrentWrapper::success)
            wxLogError(_("remote file not found ") + name );
    }
}

void FileListDialog::OnRefreshList( wxCommandEvent& /*unused*/ )
{

}

void FileListDialog::OnSelectAll( wxCommandEvent& /*unused*/ )
{
    m_filelistctrl->SelectAll();
}

void FileListDialog::OnSelectNone( wxCommandEvent& /*unused*/ )
{
    m_filelistctrl->SelectNone();
}

void FileListDialog::OnSelectInv( wxCommandEvent& /*unused*/ )
{
    m_filelistctrl->SelectInverse();
}

#endif
