#include "infopanel.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/file.h>
#include <wx/tokenzr.h>
#include <wx/icon.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/wupdlock.h>

#include "widget.h"
#include "../aui/artprovider.h"
#include "../aui/slbook.h"
#include "../utils/conversion.h"
#include "../settings++/custom_dialogs.h"
#include "../Helper/imageviewer.h"
#include "../Helper/slhtmlwindow.h"

BEGIN_EVENT_TABLE( WidgetInfoPanel, wxPanel)
    EVT_BUTTON( WidgetInfoPanel::BUT_DOWNLOAD, WidgetInfoPanel::OnDownload )
    EVT_BUTTON( WidgetInfoPanel::BUT_REMOVE, WidgetInfoPanel::OnRemove )
    EVT_BUTTON( WidgetInfoPanel::BUT_UPDATE, WidgetInfoPanel::OnUpdate )
END_EVENT_TABLE()

WidgetInfoPanel::WidgetInfoPanel( Widget& widget, wxWindow* parent, wxWindowID id, const wxString& /*unused*/,
    const wxPoint& pos , const wxSize& size , long style )
    : wxScrolledWindow (parent,  id, pos, size, style),
    m_widget( widget )
{
    m_busy_notice = new wxBoxSizer( wxVERTICAL );
    wxStaticText* m_busy_notice_lbl = new wxStaticText( this, -1, _("getting infos") );
    m_busy_notice->Add( m_busy_notice_lbl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
    SetSizer( m_busy_notice );

}

void WidgetInfoPanel::Create()
{
    wxWindowUpdateLocker locked( this );

    if ( !m_widget.extendedinfo.parsed ) {
        m_widget.GetFileInfos();
         m_widget.GetImageInfos();
            m_widget.DownloadImages();
    }
    m_widget.extendedinfo.parsed = true;
    m_busy_notice->DeleteWindows();
    m_busy_notice->Show( false );
//    m_busy_notice->Destroy();
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_left_sizer = new wxBoxSizer( wxVERTICAL );
    m_left_button_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_grid_sizer = new wxGridSizer( 2, 5, 5 );
    wxStaticText* name_lbl = new wxStaticText( this, -1, _("Name") );
    m_grid_sizer->Add( name_lbl );
    wxStaticText* name = new wxStaticText( this, -1, m_widget.name );
    m_grid_sizer->Add( name );

    wxStaticText* author_lbl = new wxStaticText( this, -1, _("Author") );
    m_grid_sizer->Add( author_lbl );
    wxStaticText* author = new wxStaticText( this, -1, m_widget.author );
    m_grid_sizer->Add( author );

    wxStaticText* mods_lbl = new wxStaticText( this, -1, _("Suitable mods") );
    m_grid_sizer->Add( mods_lbl );
    wxStaticText* mods = new wxStaticText( this, -1, m_widget.mods );
    m_grid_sizer->Add( mods );

    wxStaticText* rev_lbl = new wxStaticText( this, -1, _("Current version") );
    m_grid_sizer->Add( rev_lbl );
    wxStaticText* rev = new wxStaticText( this, -1, m_widget.rev );
    m_grid_sizer->Add( rev );

    wxStaticText* dl_lbl = new wxStaticText( this, -1, _("# downloaded") );
    m_grid_sizer->Add( dl_lbl );
    wxStaticText* dl = new wxStaticText( this, -1, TowxString( m_widget.num_downloads ) );
    m_grid_sizer->Add( dl );

    wxStaticText* publ_lbl = new wxStaticText( this, -1, _("Published on") );
    m_grid_sizer->Add( publ_lbl );
    wxStaticText* publ = new wxStaticText( this, -1, m_widget.date );
    m_grid_sizer->Add( publ );

    m_left_sizer->Add( m_grid_sizer, 0, wxEXPAND | wxALL, 5 );

    m_download = new wxButton( this, BUT_DOWNLOAD, _("Download") );
//    m_update = new wxButton( this, BUT_UPDATE, _("Update") );
    m_remove = new wxButton( this, BUT_REMOVE, _("Remove") );

    const int flag = wxALL;
    const int spc = 5;
    m_left_button_sizer->Add( m_download, 0, flag, spc );
//    m_button_sizer->Add( m_update, 0, flag, spc );
    m_left_button_sizer->Add( m_remove, 0, flag, spc );
    m_left_sizer->Add( m_left_button_sizer, 0, wxALL, 0 );


    m_right_sizer = new wxBoxSizer ( wxVERTICAL );

    m_desc = new slHtmlWindow( this, CTL_DESC, wxDefaultPosition,
            wxDefaultSize, wxHW_NO_SELECTION|wxHW_SCROLLBAR_AUTO );
    wxString ct = _T("<html><body>") + m_widget.description + _T("</body></html>") ;//content
    m_desc->SetPage( ct );


    m_changelog = new wxTextCtrl( this, -1, _T( "" ), wxDefaultPosition,
                                wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );

    wxStringTokenizer tk( m_widget.changelog, _T("\r\n") );
    while ( tk.HasMoreTokens() )
        m_changelog->AppendText( tk.GetNextToken() );

    m_ext_info = new SLNotebook(  this, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_LEFT );
    m_ext_info->SetArtProvider(new SLArtProvider);
    m_ext_info->AddPage( m_desc, _("Description") , true );
    m_ext_info->AddPage( m_changelog, _("Changelog") , false );


    if ( m_widget.GetImageFilenames().GetCount() > 0 ) {
        m_imageviewer = new ImageViewerPanel( m_widget.GetImageFilenames(), false, this, -1, 0);
        m_ext_info->AddPage( m_imageviewer, _("Screenshots") , false );
    }

    m_right_sizer->Add( m_ext_info, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* top_box = new wxStaticBoxSizer ( wxVERTICAL, this );
    top_box->Add( m_left_sizer );
    m_main_sizer->Add( top_box, 0, wxLEFT | wxEXPAND, 5 );
    m_main_sizer->Add( new wxStaticLine( this ), 0, wxEXPAND );
    m_main_sizer->Add( m_right_sizer, 1, wxLEFT|wxEXPAND, 5 );
    SetButtonStates();

    SetSizer( m_main_sizer );
    Layout();

}

void WidgetInfoPanel::SetButtonStates()
{
    m_download->Enable( !m_widget.is_installed );
//    m_update->Enable( false );
    m_remove->Enable( m_widget.is_installed );
}

WidgetInfoPanel::~WidgetInfoPanel()
{
    //dtor
}

void WidgetInfoPanel::OnDownload( wxCommandEvent& /*unused*/ )
{
    if ( m_widget.Install() ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have been installed."), _("Success") );
    }
    else {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have not been installed."), _("Error") );
    }
    SetButtonStates();
}


void WidgetInfoPanel::OnRemove( wxCommandEvent& /*unused*/ )
{
    if ( m_widget.Remove() ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have been removed."), _("Success") );
    }
    else {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have not been removed."), _("Error") );
    }
    SetButtonStates();
}

void WidgetInfoPanel::OnUpdate( wxCommandEvent& /*unused*/ )
{

}

