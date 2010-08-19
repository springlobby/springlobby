#include "hotkey_panel.h"
#include "hotkey_parser.h"
#include "spring_command.h"
#include "commandlist.h"
#include "../../settings.h"

hotkey_panel::hotkey_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
				: wxScrolledWindow(parent, id, pos, size, style|wxTAB_TRAVERSAL|wxHSCROLL,title),
					m_keyConfigPanel( this, -1, wxDefaultPosition, wxDefaultSize, 1|8|16|524288 ),
					m_uikeys_manager("D:\\games\\spring\\uikeys.txt")
{
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 ); 
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
			
	
	/*	wxStaticText* pStatCtrl = new wxStaticText( this, wxID_ANY, iter->first, wxDefaultPosition, wxDefaultSize, 0 );
		staticControls.push_back( pStatCtrl );
		pStatCtrl->Wrap( -1 );
		fgSizer1->Add( pStatCtrl, 0, wxALL, 5 );
		
		wxTextCtrl* pTxtCtrl = new wxTextCtrl( this, wxID_ANY, (*iter->second.begin()), wxDefaultPosition, wxDefaultSize, 0 );
		textControls.push_back( pTxtCtrl );
		fgSizer1->Add( pTxtCtrl, 0, wxALL, 5 );*/
	
//	m_button2 = new wxButton( this, wxID_ANY, wxT("Save hotkeys"), wxDefaultPosition, wxDefaultSize, 0 );
//	fgSizer1->Add( m_button2, 0, wxALL, 5 );
	
	//wxCmd::AddCmdType( spring_command::type_id, hotkey_panel::cmdCreate );
	//wxCmd* pCmd = wxCmd::CreateNew( spring_command::type_id, wxID_ANY );


	//m_KeyProfiles->Add(pPrimary);

	wxKeyConfigPanel::ControlMap ctrlMap;
	{
		const CommandList::CommandMap& commands = CommandList::getCommands();
		for( CommandList::CommandMap::const_iterator iter = commands.begin(); iter != commands.end(); ++iter )
		{
			const CommandList::Command& cmd = iter->second;
			ctrlMap[ cmd.m_category ][ cmd.m_command ] = cmd.m_id;
		}
	}

	m_keyConfigPanel.ImportRawList( ctrlMap, "Commands" ); 
	
	UpdateControls();

	this->SetSizer( fgSizer1 );
	this->SetScrollbars( 10, 10, 62, 62 );
	this->Layout();
}


hotkey_panel::~hotkey_panel(void)
{
	for( std::list<wxTextCtrl*>::const_iterator iter = textControls.begin(); iter != textControls.end(); ++iter )
	{
		delete (*iter);
	}
	textControls.clear();

	for( std::list<wxStaticText*>::const_iterator iter = staticControls.begin(); iter != staticControls.end(); ++iter )
	{
		delete (*iter);
	}
	staticControls.clear();
}


wxCmd* hotkey_panel::cmdCreate(int id)
{
	return new spring_command("s","d",/*,"a",*/ id);
}

void hotkey_panel::SaveSettings()
{
	sett().DeleteHotkeyProfiles();

	wxKeyProfileArray profileArr = m_keyConfigPanel.GetProfiles();
	for( int i=0; i < profileArr.GetCount(); ++i )
	{
		const wxKeyProfile& profile = *profileArr.Item(i);
		if ( profile.IsNotEditable() )
		{
			//skip build-in profiles
			continue;
		}

		const wxCmdArray* pCmdArr = profile.GetArray();
		for( int j=0; j < pCmdArr->GetCount(); ++j )
		{
			const wxCmd& cmd = *pCmdArr->Item(j);
			wxArrayString keys = cmd.GetShortcutsList();
			for( size_t k=0; k < keys.GetCount(); ++k )
			{
				sett().SetHotkey( profile.GetName(), cmd.GetName(), keys.Item(k).ToAscii(), k );
			}
		}
	}
	
	sett().SaveSettings();

	//Write bindings to file
	hotkey_parser::key_binding bindings;
	const wxKeyProfile& selProfile = *this->m_keyConfigPanel.GetSelProfile();
	const wxCmdArray* pCmdArr = selProfile.GetArray();
	for( int j=0; j < pCmdArr->GetCount(); ++j )
	{
		const wxCmd& cmd = *pCmdArr->Item(j);
		wxArrayString keys = cmd.GetShortcutsList();
		for( size_t k=0; k < keys.GetCount(); ++k )
		{
			bindings[cmd.GetName()].insert( keys.Item(k).ToAscii() );
		}
	}

//	this->m_uikeys_manager.writeBindingsToFile( bindings );
}

wxKeyProfile hotkey_panel::buildNewProfile( const wxString& name, const wxString& description, bool readOnly )
{
	wxKeyProfile profile( name, description, readOnly, readOnly );
	
	const CommandList::CommandMap& commands = CommandList::getCommands();
	for( CommandList::CommandMap::const_iterator iter = commands.begin(); iter != commands.end(); ++iter )
	{
		const CommandList::Command& cmd = iter->second;
		
		spring_command* pCmd = new spring_command( cmd.m_command, cmd.m_description, cmd.m_id );
		profile.AddCmd( pCmd );
	}

	return profile;
}

void hotkey_panel::UpdateControls(int /*unused*/)
{
	m_keyConfigPanel.RemoveAllProfiles();
	//m_profilesArray.Cleanup(); //do not use clear. only cleanup deletes objects in pointers

	{	//put default profile
		wxKeyProfile defProf = this->buildNewProfile("Spring default", wxT("Spring's default keyprofile"),true);
		
		hotkey_parser::key_binding b = m_uikeys_manager.getBindingsC2K();
		for( hotkey_parser::key_binding::const_iterator iter = b.begin(); iter != b.end(); ++iter )
		{
			wxCmd* pCmd = defProf.GetCmd( CommandList::getCommandByName(iter->first).m_id );

			const hotkey_parser::key_set& keys = iter->second;
			for( hotkey_parser::key_set::const_iterator iiter = keys.begin(); iiter != keys.end(); iiter++ )
			{
				pCmd->AddShortcut( (*iiter) );
			}
		}

		m_keyConfigPanel.AddProfile( defProf );
	}

	//put user profiles
	wxArrayString profiles = sett().GetHotkeyProfiles();

	for( size_t i=0; i < profiles.GetCount(); ++i)
	{
		wxString profName = profiles.Item(i);

		wxKeyProfile profile = buildNewProfile(profName, wxT("User hotkey profile"),false);
		
		//add keybindings
		wxArrayString commands = sett().GetHotkeyProfileCommands( profName );
		for( size_t k=0; k < commands.GetCount(); ++k )
		{
			wxString cmd = commands.Item(k);
			wxArrayString keys = sett().GetHotkeyProfileCommandKeys( profName, cmd );
			for( size_t j=0; j < keys.GetCount(); ++j )
			{
				wxCmd* pCmd = profile.GetCmd( CommandList::getCommandByName(cmd).m_id );
				pCmd->AddShortcut( sett().GetHotkey( profName, cmd, keys.Item(j) ) );
			}
		}

		m_keyConfigPanel.AddProfile( profile );
	}

	m_keyConfigPanel.SetSelProfile(0);
}