#include "custom_dialogs.h"

#include <wx/icon.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/defs.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/font.h>
#include <wx/event.h>
#include <wx/stattext.h>
#include <wx/artprov.h>
#include <wx/statbmp.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/app.h>
#include <wx/choicdlg.h>


#include "../images/springsettings.xpm"
#include "../images/springlobby.xpm"
#include "../utils/conversion.h"

BEGIN_EVENT_TABLE(CustomMessageBox ,wxDialog)
  EVT_BUTTON(wxID_NO, CustomMessageBox::OnOptionsNo)
END_EVENT_TABLE()

wxWindow* CustomMessageBoxBase::m_settingsWindow = 0;
wxWindow* CustomMessageBoxBase::m_lobbyWindow = 0;
static CustomMessageBox* s_nonmodbox = 0;
static ServerMessageBox* s_serverMsgBox = 0;
static ActNotifBox* s_actNotifBox = 0;
static MutelistWindow* s_mutelistWindow = 0;

CustomMessageBox::CustomMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
        const wxString& caption ,
        long style, const wxPoint& pos )
			: wxDialog(parent,-1,caption,pos,wxDefaultSize,style|wxFRAME_FLOAT_ON_PARENT|wxDEFAULT_DIALOG_STYLE)
{
	SetIcon(*icon);

//******** copied from wxsource/generic/msgdlgg.cpp with small modifications***********************************************************


    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );


    // 1) icon

    wxBitmap bitmap;
    switch ( style & wxICON_MASK )
    {
        default:
            bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
            break;

        case wxICON_ERROR:
            bitmap = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
            break;

        case wxICON_INFORMATION:
            bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
            break;

        case wxICON_WARNING:
            bitmap = wxArtProvider::GetIcon(wxART_WARNING, wxART_MESSAGE_BOX);
            break;

        case wxICON_QUESTION:
            bitmap = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX);
            break;
    }

    wxStaticBitmap *info_icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
    icon_text->Add( info_icon, 0, wxCENTER );

    // 2) text
    icon_text->Add( CreateTextSizer( message ), 0, wxALIGN_TOP| wxLEFT, 10 );

    topsizer->Add( icon_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
    topsizer->Add(0,10);

    // 3) buttons
    int center_flag = wxEXPAND;
    if (style & wxYES_NO)
        center_flag = wxALIGN_CENTRE;
    wxSizer *sizerBtn = CreateButtonSizer(style & ButtonSizerFlags);
    if ( sizerBtn )
        topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10 );


    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    /*
    wxSize size( GetSize() );
    if (size.x > size.y*3/2)
    {
        size.x = size.y*3/2;
        SetSize( size );
    }
    */
    Centre( wxBOTH | wxCENTER_FRAME);
/***************************************************************************************************/
}

CustomMessageBox::~CustomMessageBox()
{
}


void CustomMessageBox::OnOptionsNo(wxCommandEvent& event)
{
   EndModal(wxID_NO);
}

void CustomMessageBoxBase::setLobbypointer(wxWindow* arg)
{
	m_lobbyWindow = arg;
}

 void CustomMessageBoxBase::setSettingspointer(wxWindow* arg)
{
	m_settingsWindow = arg;
}

 wxWindow* CustomMessageBoxBase::getLobbypointer()
{
//	 if (m_lobbyWindow==0)
//			 wxLogWarning(_T("null parent window in custom message dialog"));
	return m_lobbyWindow;
}

 wxWindow* CustomMessageBoxBase::getSettingspointer()
{
//	 if (m_settingsWindow==0)
//		 wxLogWarning(_T("null parent window in custom message dialog"));
	return m_settingsWindow;
}

int customMessageBox( int whichIcon , const wxString& message,const wxString& caption,
		long style , int x, int y )
{
		wxWindow* parent;
		wxIcon* icon;
		switch (whichIcon)
		{
			case SL_MAIN_ICON:
				icon = new wxIcon(springlobby_xpm);
				parent = CustomMessageBoxBase::getLobbypointer();
				break;
			case SS_MAIN_ICON:
				icon = new wxIcon(springsettings_xpm);
				parent = CustomMessageBoxBase::getSettingspointer();
				break;
			default:
				icon = new wxIcon(wxNullIcon);
				parent = 0;
				break;

		}
		CustomMessageBox dlg(icon,parent,message,caption,style,wxPoint(x,y));
		int re = dlg.ShowModal();
		switch (re)
		{
			case wxID_OK: return wxOK;
			case wxID_CANCEL: return wxCANCEL;
			case wxID_YES: return wxYES;
			case wxID_NO: return wxNO;
		}
		return -1;
}

void customMessageBoxNoModal( int whichIcon , const wxString& message,const wxString& caption,
		long style , int x, int y )
{
		wxWindow* parent;
		wxIcon* icon;
		switch (whichIcon)
		{
			case SL_MAIN_ICON:
				icon = new wxIcon(springlobby_xpm);
				parent = CustomMessageBoxBase::getLobbypointer();
				break;
			case SS_MAIN_ICON:
				icon = new wxIcon(springsettings_xpm);
				parent = CustomMessageBoxBase::getSettingspointer();
				break;
			default:
				icon = new wxIcon(wxNullIcon);
				parent = 0;
				break;

		}
		s_nonmodbox = new CustomMessageBox (icon,parent,message,caption,style,wxPoint(x,y));

		s_nonmodbox->Show(true);
}

void freeStaticBox()
{
    if (s_nonmodbox!=0)
    {
        s_nonmodbox->Show(false);
         s_nonmodbox->Destroy();
         s_nonmodbox = 0;
    }
    if (s_serverMsgBox!=0)
    {
        s_serverMsgBox->Show(false);
         s_serverMsgBox->Destroy();
         s_serverMsgBox = 0;
    }
    if (s_actNotifBox!=0)
    {
        s_actNotifBox->Show(false);
         s_actNotifBox->Destroy();
         s_actNotifBox = 0;
    }

}

CreditsDialog::CreditsDialog(wxWindow* parent,wxString title,int whichIcon) : wxDialog(parent,-1,title,wxDefaultPosition,
		wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxFRAME_FLOAT_ON_PARENT)
{
	wxBoxSizer* container = new wxBoxSizer(wxVERTICAL);
	text_ctrl = new wxTextCtrl(this,-1,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH);
	container->Add(text_ctrl,1,wxEXPAND);

    container->Add(wxDialog::CreateButtonSizer(wxOK));
	SetSizer(container);
	wxIcon* icon;
	switch (whichIcon)
	{
		case SL_MAIN_ICON:
			icon = new wxIcon(springlobby_xpm);
			break;
		case SS_MAIN_ICON:
			icon = new wxIcon(springsettings_xpm);
			break;
		default:
			icon = new wxIcon(wxNullIcon);
			break;

	}
	SetIcon(*icon);
}

void CreditsDialog::AddCredit(wxString person,wxString message)
{
	wxFont* heavyFont = new wxFont(10, wxDEFAULT, wxNORMAL, wxBOLD);
	text_ctrl->SetDefaultStyle(wxTextAttr(wxNullColour,wxNullColour,*heavyFont));
	text_ctrl->AppendText(person + _T(": "));
	text_ctrl->SetDefaultStyle(wxTextAttr(wxNullColour,wxNullColour,*wxNORMAL_FONT));
	text_ctrl->AppendText(message + _T("\n\n"));
}

CreditsDialog::~CreditsDialog()
{
}

ServerMessageBox::~ServerMessageBox()
{
}

ServerMessageBox::ServerMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
        const wxString& caption ,
        long style, const wxPoint& pos )
			: wxDialog(parent,-1,caption,pos,wxDefaultSize,style|wxFRAME_FLOAT_ON_PARENT|wxDEFAULT_DIALOG_STYLE|wxEXPAND)
{
	SetIcon(*icon);

    m_messages = new wxListCtrl(this,-1,wxDefaultPosition,wxDefaultSize,wxLC_NO_HEADER|wxLC_REPORT);
    m_messages->InsertColumn(0,_T(""));
    topsizer = new wxBoxSizer( wxVERTICAL );

    AppendMessage(message);

    topsizer->Add( m_messages, 1, wxALL|wxEXPAND|wxALIGN_CENTRE, 10 );
    topsizer->Add(0,10);

    wxSizer *sizerBtn = CreateButtonSizer(wxOK);
    topsizer->Add(sizerBtn, 0,  wxALL|wxALIGN_CENTRE, 10 );

    SetSizer( topsizer );
    Centre( wxBOTH | wxCENTER_FRAME);

}

void ServerMessageBox::AppendMessage(const wxString& message)
{
    if ( message == wxEmptyString )
        return;
    m_messages->InsertItem(m_messages->GetItemCount(),message);
    m_messages->SetColumnWidth(0, wxLIST_AUTOSIZE);

    SetSize(m_messages->GetColumnWidth(0)+25,-1);
    Layout();
}

ActNotifBox::ActNotifBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
        const wxString& caption ,
        long style, const wxPoint& pos)
    : ServerMessageBox( icon , parent, message, caption, style, pos)
{}

ActNotifBox::~ActNotifBox(){}

void ActNotifBox::AppendMessage( const wxString& message )
{
    wxDateTime now = wxDateTime::Now();
    wxString msg = now.Format( _T("%H:%M: ") ) + message;
    ServerMessageBox::AppendMessage( msg );
}

void serverMessageBox( int whichIcon , const wxString& message,const wxString& caption,
		long style , int x, int y )
{
		wxWindow* parent;
		wxIcon* icon;
		switch (whichIcon)
		{
			case SL_MAIN_ICON:
				icon = new wxIcon(springlobby_xpm);
				parent = CustomMessageBoxBase::getLobbypointer();
				break;
			case SS_MAIN_ICON:
				icon = new wxIcon(springsettings_xpm);
				parent = CustomMessageBoxBase::getSettingspointer();
				break;
			default:
				icon = new wxIcon(wxNullIcon);
				parent = 0;
				break;

		}
		if ( s_serverMsgBox != 0 && s_serverMsgBox->IsShown() )
		{
		    s_serverMsgBox->AppendMessage(message);
		}
		else
		{
            s_serverMsgBox = new ServerMessageBox (icon,parent,message,caption,style,wxPoint(x,y));
            s_serverMsgBox->Show(true);
		}
}

void actNotifBox( int whichIcon , const wxString& message,const wxString& caption,
		long style , int x, int y )
{
		wxWindow* parent;
		wxIcon* icon;
		switch (whichIcon)
		{
			case SL_MAIN_ICON:
				icon = new wxIcon(springlobby_xpm);
				parent = CustomMessageBoxBase::getLobbypointer();
				break;
			case SS_MAIN_ICON:
				icon = new wxIcon(springsettings_xpm);
				parent = CustomMessageBoxBase::getSettingspointer();
				break;
			default:
				icon = new wxIcon(wxNullIcon);
				parent = 0;
				break;

		}
		if ( s_actNotifBox != 0 && s_actNotifBox->IsShown() )
		{
		    s_actNotifBox->AppendMessage(message);
		}
		else
		{
            s_actNotifBox = new ActNotifBox(icon,parent,wxEmptyString,caption,style,wxPoint(x,y));
            s_actNotifBox->AppendMessage(message);
            s_actNotifBox->Show(true);
		}
}

int GetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& aChoices,
                            const int selected,
                            wxWindow *parent ,
                            int x ,
                            int y ,
                            bool centre )
{
    wxString *choices;
    int n = ConvertWXArrayToC(aChoices, &choices);

    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    dialog.SetSelection( selected );

    int choice;
    if ( dialog.ShowModal() == wxID_OK )
        choice = dialog.GetSelection();
    else
        choice = -1;

    delete [] choices;

    return choice;
}

BEGIN_EVENT_TABLE(ActivityNoticePanel,wxPanel)
    	EVT_TIMER(wxID_ANY, ActivityNoticePanel::OnTimer)
END_EVENT_TABLE()


 ActivityNotice::ActivityNotice(wxWindow* parent,const wxString& file,const wxString& format )
    :wxDialog ( parent,wxID_ANY,_T(""),wxDefaultPosition, wxSize(190,60),wxBORDER_NONE|wxSTAY_ON_TOP)
{
    wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_panel = new ActivityNoticePanel(this,file,format);
    m_main_sizer->Add( m_panel, 1 , wxALL|wxEXPAND, 0 );
    SetSizer(m_main_sizer);
    Layout();
}

 ActivityNoticePanel::ActivityNoticePanel(wxWindow* parent,const wxString& file,const wxString& format )
    :wxPanel ( parent,wxID_ANY,wxDefaultPosition, wxSize(190,60),wxBORDER_NONE),
    m_filename(file),m_format(format)
{
    static int timer_inc = 0;
    m_timer.SetOwner(this,timer_inc++);
    wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_gauge = new wxGauge(this, wxID_ANY,120,wxDefaultPosition,wxSize(80,5) );
    m_gauge->Pulse();
    m_message = new wxStaticText(this,wxID_ANY,_T("") );
    m_main_sizer->Add( m_message, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
    m_main_sizer->Add( m_gauge, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
    SetSizer( m_main_sizer );
    SetString( m_filename );
    Show(true);
}

void ActivityNoticePanel::SetString(const wxString& file)
{
    m_filename = file;
    m_message->SetLabel(wxString::Format( m_format, m_filename.c_str() ) );
    Layout();
}

bool ActivityNoticePanel::Show(bool show )
{
    if ( show )
        m_timer.Start( 80, wxTIMER_CONTINUOUS );
    else
        m_timer.Stop ();
    bool ret = wxPanel::Show(show);
    Layout();
    return ret;
}

void ActivityNoticePanel::OnTimer(wxTimerEvent& event)
{
    m_gauge->Pulse();
}

 ActivityNoticeContainer::ActivityNoticeContainer(wxWindow* parent_,const wxString& file,const wxString& format )
{
    wxWindow* parent = ( parent_ != 0 ) ? parent_ : ( wxTheApp->GetTopWindow() ) ;
    m_window = new ActivityNotice( parent, file, format);
    m_window->Show(true);
}

 ActivityNoticeContainer::~ActivityNoticeContainer()
{
    m_window->Destroy();
}


void mutelistWindow( const wxString& message, const wxString& caption,
        long style, const int x, const int y )
{
        wxWindow* parent = CustomMessageBoxBase::getLobbypointer();
		wxIcon* icon = new wxIcon(springlobby_xpm);

		if ( s_mutelistWindow != 0 && s_mutelistWindow->IsShown() )
		{
		    s_mutelistWindow->AppendMessage(message);
		}
		else
		{
            s_mutelistWindow = new MutelistWindow(icon,parent,wxEmptyString,caption,style,wxPoint(x,y));
            s_mutelistWindow->AppendMessage(message);
            s_mutelistWindow->Show(true);
		}
}

MutelistWindow::MutelistWindow(wxIcon* icon ,wxWindow *parent, const wxString& message,
        const wxString& caption ,
        long style, const wxPoint& pos)
    : ServerMessageBox( icon , parent, message, caption, style, pos)
{}

MutelistWindow::~MutelistWindow ()
{}

