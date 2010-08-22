#include "hotkey_panel.h"

#include <iostream>

#include "../../utils/customdialogs.h"
#include "hotkey_parser.h"
#include "spring_command.h"
#include "commandlist.h"
#include "../../settings.h"
#include "KeynameConverter.h"
#include "SpringDefaultProfile.h"

hotkey_panel::hotkey_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
: wxScrolledWindow(parent, id, pos, size, style|wxTAB_TRAVERSAL|wxHSCROLL,title),
m_keyConfigPanel( this, -1, wxDefaultPosition, wxDefaultSize, 1|8|16|524288 ),
m_uikeys_manager("D:\\games\\spring\\uikeys.txt")
{
	try
	{
		wxFlexGridSizer* fgSizer1;
		fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 ); 
		fgSizer1->SetFlexibleDirection( wxBOTH );
		fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

		KeynameConverter::initialize();

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
	catch( const std::exception& ex )
	{
		customMessageBox(SS_MAIN_ICON, ex.what(), _("Hotkey panel error"), wxOK );
		throw;
	}
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

bool hotkey_panel::isBindingInProfile( const wxKeyProfile& profile, const wxString& command, const wxString& springkey )
{
	const wxCmdArray* pCmdArr = profile.GetArray();
	for( int j=0; j < pCmdArr->GetCount(); ++j )
	{
		const wxCmd& cmd = *pCmdArr->Item(j);
		wxArrayString keys = cmd.GetShortcutsList();
		for( size_t k=0; k < keys.GetCount(); ++k )
		{
			if ( ( cmd.GetName() == command ) && ( KeynameConverter::compareSpring2wxKeybinder( springkey, keys.Item(k) ) ) )
			{
				return true;
			}
		}
	}
	return false;
}

bool hotkey_panel::isDefaultBinding( const wxString& command, const wxString& keybinderkey )
{
	const key_binding& defBindings = SpringDefaultProfile::getAllBindingsC2K();

	key_binding::const_iterator pos = defBindings.find( command );
	if ( pos == defBindings.end() )
	{
		return false;
	}

	for ( key_set::const_iterator iiter = pos->second.begin(); iiter != pos->second.end(); ++iiter )
	{
		//check now if this default key binding has been deleted from this profile
		if ( KeynameConverter::compareSpring2wxKeybinder( (*iiter), keybinderkey ) )
		{
			return true;
		}
	}

	return false;
}

/*
*	We do only save the bindings that differ from the standard spring keybindings
*/
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

		//add bindings
		const wxCmdArray* pCmdArr = profile.GetArray();
		for( int j=0; j < pCmdArr->GetCount(); ++j )
		{
			const wxCmd& cmd = *pCmdArr->Item(j);
			wxArrayString keys = cmd.GetShortcutsList();
			for( size_t k=0; k < keys.GetCount(); ++k )
			{
				//only write non-default bindings to settings
				if ( false == hotkey_panel::isDefaultBinding( cmd.GetName(), keys.Item( k ) ) )
				{
					sett().SetHotkey( profile.GetName(), cmd.GetName(), keys.Item(k).ToAscii(), false );
				}
			}
		}

		//check if any bindings from the default bindings have been unbind. and save that info to the settings
		const key_binding& defBindings = SpringDefaultProfile::getAllBindingsC2K();
		for( key_binding::const_iterator iter = defBindings.begin(); iter != defBindings.end(); ++iter )
		{
			//check now if this default key binding has been deleted from this profile
			for ( key_set::const_iterator iiter = iter->second.begin(); iiter != iter->second.end(); ++iiter )
			{
				if ( false == hotkey_panel::isBindingInProfile( profile, iter->first, (*iiter) ) )
				{
					sett().SetHotkey( profile.GetName(), iter->first, KeynameConverter::spring2wxKeybinder( (*iiter) ), true );
				}
			}
		}
	}

	sett().SaveSettings();

	//Write bindings to file
	key_binding bindings;
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

	// use this to generate the source code of the spring default key bindings
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

void hotkey_panel::putKeybindingsToProfile( wxKeyProfile& profile, const key_binding& bindings )
{
	for( key_binding::const_iterator iter = bindings.begin(); iter != bindings.end(); ++iter )
	{
		wxCmd& cmd = *(profile.GetCmd( CommandList::getCommandByName(iter->first).m_id ));

		const key_set& keys = iter->second;
		for( key_set::const_iterator iiter = keys.begin(); iiter != keys.end(); iiter++ )
		{
			cmd.AddShortcut( KeynameConverter::spring2wxKeybinder( (*iiter) ) );
		}
	}
}

void hotkey_panel::UpdateControls(int /*unused*/)
{
	m_keyConfigPanel.RemoveAllProfiles();

	//put default profile
	wxKeyProfile defProf = this->buildNewProfile("Spring default", wxT("Spring's default keyprofile"),true);
	key_binding defBinds = SpringDefaultProfile::getAllBindingsC2K();
	this->putKeybindingsToProfile( defProf, defBinds );		
	m_keyConfigPanel.AddProfile( defProf );

	//put user profiles from springsettings configuration
	key_binding_collection configProfiles;
	{
		wxArrayString profiles = sett().GetHotkeyProfiles();
		for( size_t i=0; i < profiles.GetCount(); ++i)
		{
			wxString profName = profiles.Item(i);

			wxKeyProfile profile = buildNewProfile(profName, wxT("User hotkey profile"),false);
			this->putKeybindingsToProfile( profile, defBinds );	

			//add keybindings
			wxArrayString commands = sett().GetHotkeyProfileCommands( profName );
			for( size_t k=0; k < commands.GetCount(); ++k )
			{
				wxString cmd = commands.Item(k);
				wxArrayString keys = sett().GetHotkeyProfileCommandKeys( profName, cmd );
				for( size_t j=0; j < keys.GetCount(); ++j )
				{
					wxCmd* pCmd = profile.GetCmd( CommandList::getCommandByName(cmd).m_id );
					wxString key = sett().GetHotkey( profName, cmd, keys.Item(j) );
					if ( key == "bind" )
					{
						pCmd->AddShortcut( keys.Item(j) );
					}
					else if ( key == "unbind" )
					{
						pCmd->RemoveShortcut( keys.Item(j) );
					}
					else
					{
						throw std::runtime_error( std::string("Unknown key action: ") + key.ToAscii() );
					}
				}
			}

			m_keyConfigPanel.AddProfile( profile );
		}
	}

	//	m_uikeys_manager.g
	m_keyConfigPanel.SetSelProfile(0);
}