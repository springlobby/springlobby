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
#include "utils.h"
#include <wx/artprov.h>
#include <wx/statbmp.h>

#include "../images/springsettings.xpm"
#include "../images/springlobby.xpm"


wxWindow* CustomMessageBox::m_settingsWindow = 0;
wxWindow* CustomMessageBox::m_lobbyWindow = 0;

//CustomMessageBox* GetNonModalMsgBox()
//{
//
//}

CustomMessageBox::CustomMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
        const wxString& caption ,
        long style, const wxPoint& pos )
			: wxDialog(parent,-1,caption,pos,wxDefaultSize,style|wxFRAME_FLOAT_ON_PARENT|wxDEFAULT_DIALOG_STYLE)
{
	SetIcon(*icon);
	/*wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);
	box->Add(0,15,1,wxEXPAND|wxALL);
	box->Add(new wxStaticText(this, -1, message), 0, wxALL| wxEXPAND, 20);
	box->Add(0,15,1,wxEXPAND|wxALL);
	box->Add(CreateButtonSizer(style));

	SetSizer(box);
    box->SetSizeHints( this );*/

//******** copied from wxsource/generic/msgdlgg.cpp with small modifications***********************************************************
//    SetMessageDialogStyle(style);

        wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );


    // 1) icon
    if (style & wxICON_MASK)
    {
        wxBitmap bitmap;
        switch ( style & wxICON_MASK )
        {
            default:
                wxLogError(_T("wrong icon in msgbox specified"));
                // fall through

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

        wxStaticBitmap *icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
        icon_text->Add( icon, 0, wxCENTER );
    }




    // 2) text
    icon_text->Add( CreateTextSizer( message ), 0, wxALIGN_CENTER | wxLEFT, 10 );

    topsizer->Add( icon_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );


    // 3) buttons
    int center_flag = wxEXPAND;
    if (style & wxYES_NO)
        center_flag = wxALIGN_CENTRE;
    wxSizer *sizerBtn = CreateSeparatedButtonSizer(style & ButtonSizerFlags);
    if ( sizerBtn )
        topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10 );

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    wxSize size( GetSize() );
    if (size.x < size.y*3/2)
    {
        size.x = size.y*3/2;
        SetSize( size );
    }

    Centre( wxBOTH | wxCENTER_FRAME);
/***************************************************************************************************/
}

CustomMessageBox::~CustomMessageBox()
{
}


void CustomMessageBox::OnCloseWindow(wxCloseEvent& event)
{
    //wxDialogBase::AcceptAndClose();
}

void CustomMessageBox::setLobbypointer(wxWindow* arg)
{
	m_lobbyWindow = arg;
}

 void CustomMessageBox::setSettingspointer(wxWindow* arg)
{
	m_settingsWindow = arg;
}

 wxWindow* CustomMessageBox::getLobbypointer()
{
//	 if (m_lobbyWindow==0)
//			 wxLogWarning(_T("null parent window in custom message dialog"));
	return m_lobbyWindow;
}

 wxWindow* CustomMessageBox::getSettingspointer()
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
				parent = CustomMessageBox::getLobbypointer();
				break;
			case SS_MAIN_ICON:
				icon = new wxIcon(springsettings_xpm);
				parent = CustomMessageBox::getSettingspointer();
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
				parent = CustomMessageBox::getLobbypointer();
				break;
			case SS_MAIN_ICON:
				icon = new wxIcon(springsettings_xpm);
				parent = CustomMessageBox::getSettingspointer();
				break;
			default:
				icon = new wxIcon(wxNullIcon);
				parent = 0;
				break;

		}
		static CustomMessageBox* s_nonModalMsgBox;
		s_nonModalMsgBox = new  CustomMessageBox(icon,parent,message,caption,style,wxPoint(x,y));

		s_nonModalMsgBox->Show(true);
}

CreditsDialog::CreditsDialog(wxWindow* parent,wxString title,int whichIcon) : wxDialog(parent,-1,title,wxDefaultPosition,
		wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxFRAME_FLOAT_ON_PARENT)
{
	wxBoxSizer* container = new wxBoxSizer(wxVERTICAL);
	text_ctrl = new wxTextCtrl(this,-1,wxT(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH);
	container->Add(text_ctrl,1,wxEXPAND);
	//container->Add(new wxButton(this,wxID_CLOSE),0);

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

