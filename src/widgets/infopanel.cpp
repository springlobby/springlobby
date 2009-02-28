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

#include "widget.h"
#include "../utils.h"
//#include "../settings.h"
#include "../settings++/custom_dialogs.h"
#include "../Helper/imageviewer.h"
#include "../Helper/slhtmlwindow.h"

BEGIN_EVENT_TABLE( WidgetInfoPanel, wxPanel)
    EVT_BUTTON( WidgetInfoPanel::BUT_CHG_LOG, WidgetInfoPanel::OnChangeLog )
    EVT_BUTTON( WidgetInfoPanel::BUT_DOWNLOAD, WidgetInfoPanel::OnDownload )
    EVT_BUTTON( WidgetInfoPanel::BUT_REMOVE, WidgetInfoPanel::OnRemove )
    EVT_BUTTON( WidgetInfoPanel::BUT_UPDATE, WidgetInfoPanel::OnUpdate )
    EVT_BUTTON( WidgetInfoPanel::BUT_PICS, WidgetInfoPanel::OnPics )
    EVT_BUTTON( WidgetInfoPanel::BUT_DESC, WidgetInfoPanel::OnDescription )

END_EVENT_TABLE()

WidgetInfoPanel::WidgetInfoPanel( Widget& widget, wxWindow* parent, wxWindowID id, const wxString& title,
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
    m_top_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_button_sizer = new wxBoxSizer( wxHORIZONTAL );

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

    m_top_sizer->Add( m_grid_sizer, 0, wxEXPAND, 0 );

    m_desc_sizer = new wxBoxSizer ( wxHORIZONTAL );
    m_screeny_sizer = new wxBoxSizer ( wxHORIZONTAL );
    m_chglog_sizer = new wxBoxSizer ( wxHORIZONTAL );
    m_variable_info_sizer = new wxBoxSizer ( wxHORIZONTAL );

    //wxStaticBoxSizer* desc_frame = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Description") ), wxVERTICAL );
    m_desc = new slHtmlWindow( this, CTL_DESC, wxDefaultPosition,
            wxDefaultSize, wxHW_NO_SELECTION|wxHW_SCROLLBAR_AUTO );
    wxString ct = _T("<html><body>") + m_widget.description + _T("</body></html>") ;//content
    m_desc->SetPage( ct );
    m_desc_sizer->Add( m_desc, 1, wxEXPAND | wxALIGN_CENTER);

    m_changelog = new wxTextCtrl( this, -1, _T( "" ), wxDefaultPosition,
                                wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );

    wxStringTokenizer tk( m_widget.changelog, _T("\r\n") );
    while ( tk.HasMoreTokens() )
        m_changelog->AppendText( tk.GetNextToken() );
    m_chglog_sizer->Add( m_changelog, 1, wxEXPAND | wxALIGN_CENTER );
    m_chglog_sizer->Show( false );

    if ( m_widget.GetImageFilenames().GetCount() > 0 ) {
        m_imageviewer = new ImageViewerPanel( m_widget.GetImageFilenames(), false, this, -1, 0);
        m_screeny_sizer->Add( m_imageviewer, 1, wxEXPAND | wxALIGN_CENTER );
    }
    m_screeny_sizer->Show( false );

    m_variable_info_sizer->Add( m_desc_sizer , 1, wxEXPAND | wxALIGN_CENTER );
    m_variable_info_sizer->Add( m_screeny_sizer , 1, wxEXPAND | wxALIGN_CENTER );
    m_variable_info_sizer->Add( m_chglog_sizer , 1, wxEXPAND | wxALIGN_CENTER );

    m_top_sizer->Add( m_variable_info_sizer, 1, wxEXPAND|wxLEFT, 10 );

    m_main_sizer->Add( m_top_sizer, 1, wxLEFT|wxEXPAND|wxALL, 5 );

    m_download = new wxButton( this, BUT_DOWNLOAD, _("Download") );
    m_chg_log = new wxButton( this, BUT_CHG_LOG, _("Changelog") );
    m_pics = new wxButton( this, BUT_PICS, _("Screenshots") );
    m_show_desc = new wxButton( this, BUT_DESC, _("Description") );
//    m_update = new wxButton( this, BUT_UPDATE, _("Update") );
    m_remove = new wxButton( this, BUT_REMOVE, _("Remove") );

    const int flag = wxALL;
    const int spc = 5;
    m_button_sizer->Add( m_download, 0, flag, spc );
//    m_button_sizer->Add( m_update, 0, flag, spc );
    m_button_sizer->Add( m_remove, 0, flag, spc );
    m_button_sizer->Add( m_chg_log, 0, flag, spc );
    m_button_sizer->Add( m_pics, 0, flag, spc );
    m_button_sizer->Add( m_show_desc, 0, flag, spc );

    m_main_sizer->Add( m_button_sizer, 0, wxLEFT|wxEXPAND|wxALL, 0 );
    SetButtonStates();

    SetSizer( m_main_sizer );
    Layout();

}

void WidgetInfoPanel::SetButtonStates()
{
    m_download->Enable( !m_widget.is_installed );
    m_show_desc->Enable( !m_variable_info_sizer->IsShown( m_desc_sizer ) );
    m_chg_log->Enable( m_widget.changelog != _T("") && !m_variable_info_sizer->IsShown( m_chglog_sizer ) );
    m_pics->Enable( m_widget.extendedinfo.images.size() > 0 && !m_variable_info_sizer->IsShown( m_screeny_sizer ) );
//    m_update->Enable( false );
    m_remove->Enable( m_widget.is_installed );

}

WidgetInfoPanel::~WidgetInfoPanel()
{
    //dtor
}

void WidgetInfoPanel::OnDownload( wxCommandEvent& evt )
{
    if ( m_widget.Install() ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have been installed."), _("Success") );
    }
    else {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have not been installed."), _("Error") );
    }
    SetButtonStates();
}

void WidgetInfoPanel::OnPics( wxCommandEvent& evt )
{
    //do not show, recurse to children
    m_variable_info_sizer->Show( m_desc_sizer, false, true );
    m_variable_info_sizer->Show( m_chglog_sizer, false, true );
    m_variable_info_sizer->Show( m_screeny_sizer, true, true );
    Layout();
    SetButtonStates();
}

void WidgetInfoPanel::OnChangeLog( wxCommandEvent& evt )
{
    m_variable_info_sizer->Show( m_desc_sizer, false, true );
    m_variable_info_sizer->Show( m_screeny_sizer, false, true );
    m_variable_info_sizer->Show( m_chglog_sizer, true, true );
    Layout();
    SetButtonStates();
}

void WidgetInfoPanel::OnDescription( wxCommandEvent& evt )
{
    m_variable_info_sizer->Show( m_screeny_sizer, false, true );
    m_variable_info_sizer->Show( m_chglog_sizer, false, true );
    m_variable_info_sizer->Show( m_desc_sizer, true, true );
    Layout();
    SetButtonStates();
}

void WidgetInfoPanel::OnRemove( wxCommandEvent& evt )
{
    if ( m_widget.Remove() ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have been removed."), _("Success") );
    }
    else {
        customMessageBoxNoModal( SL_MAIN_ICON, _("Widget files have not been removed."), _("Error") );
    }
    SetButtonStates();
}

void WidgetInfoPanel::OnUpdate( wxCommandEvent& evt )
{

}

