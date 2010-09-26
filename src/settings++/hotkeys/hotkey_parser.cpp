#include "hotkey_parser.h"

#include <fstream>
#include <iostream>

#include <wx/hashmap.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>

#include "KeynameConverter.h"
#include "SpringDefaultProfile.h"
#include "hotkey_panel.h"
#include "HotkeyException.h"

#include "../../utils/conversion.h"


hotkey_parser::hotkey_parser(const wxString& uikeys_filename) : m_filename( uikeys_filename )
{
	//we will read the uikeys.txt now to get the key profile
	//1. Fill the profile with spring's default bindings
	KeynameConverter::initialize();

	this->m_bindings = SpringDefaultProfile::getBindings();

	//2. now read uikeys.txt and modify the default profile
	wxTextFile uiFile( this->m_filename );

	if ( !uiFile.Open() )
	{
		wxLogWarning( _( "can't open " ) + uikeys_filename );
		return;
	}

	wxString line;
	for ( line = uiFile.GetFirstLine(); !uiFile.Eof(); line = uiFile.GetNextLine() )
	{
		if ( line.Trim().StartsWith( wxT("//") ) )
		{
			continue;
		}

		if ( line.size() == 0 )
		{
			continue;
		}

		this->processLine( line );
	}
}

bool hotkey_parser::processLine( const wxString& line )
{
	//tokenize line
	std::vector<wxString> tokLine = hotkey_parser::tokenize_uikeys_line( line.c_str() );

	if ( tokLine.size() == 1 )
	{ //unbindall?
		if ( tokLine[0] == wxT("unbindall") )
		{
			this->m_bindings.clear();
			return true;
		}
		wxLogWarning( wxT( "skipping uikeys.txt line: " ) + line );
		return false;
	}
	else if ( tokLine.size() == 2 ) 
	{	//fakemeta=
		const wxString& cmd = tokLine[0];
		const wxString& key = tokLine[1];

		if ( cmd == wxT("fakemeta") )
		{
			this->m_bindings.bind( cmd, key );
			return true;
		}
	}

	const wxString& cmd = tokLine[0];
	const wxString& key = KeynameConverter::normalizeSpringKey( tokLine[1] );

	//append all following tokens to the action string for stuff like "buildspacing inc"
	wxString action; 
	for( unsigned i=2; i < tokLine.size(); ++i )
	{
		action.append( tokLine[i] + wxT(" ") );
	}
	action.Trim();

	if ( cmd == wxT("bind") )
	{
		this->m_bindings.bind( action, key );
		return true;
	}
	else if ( cmd == wxT("unbind") )
	{
		this->m_bindings.unbind( action, key );
	}
	else
	{
		wxLogWarning( _( "skipping uikeys.txt line (unknown token '" ) + cmd + wxT("'): ") + line );
		return false;
	}

	return true;
}

hotkey_parser::~hotkey_parser(void)
{
}
std::vector<wxString> hotkey_parser::tokenize_uikeys_line( const wxString& line )
{
	wxStringTokenizer tkz( line, wxT(" \t") );

	std::vector<wxString> data;
	while ( tkz.HasMoreTokens() )
	{
		data.push_back( tkz.GetNextToken() );
	}
	return data;
}

const key_binding& hotkey_parser::getBindings() const
{
	return this->m_bindings;
}

void hotkey_parser::writeBindingsToFile( const key_binding& springbindings )
{
	const wxString newTmpFilename = this->m_filename + wxT(".tmp");
	wxTextFile newFile( newTmpFilename );

	//open new file for writing
	if ( ( !newFile.Exists() && !newFile.Create() ) || ( newFile.Exists() && !newFile.Open() ) )
	{
		throw HotkeyException( _("Error opening file for writing: ") + newTmpFilename );
	}

	//now read the old uikeys.txt line after line and copy all comments to the new file
	wxTextFile oldFile( this->m_filename );
	if ( !oldFile.Open() )
	{
		throw HotkeyException( _("Error opening file for writing: ") + newTmpFilename );
	}

	for( size_t i = 0; i < oldFile.GetLineCount(); ++i )
	{
		wxString line = oldFile.GetLine( i );
		line.Trim();
		if ( line.StartsWith(wxT("//")) )
		{
			newFile.AddLine( line );
		}
	}
	oldFile.Close();

	//check all default bindings if they still exist in current profile
	//do unbind if not
	const key_binding::key_binding_k2c unbinds = (SpringDefaultProfile::getBindings() - springbindings).getK2C();
	for( key_binding::key_binding_k2c::const_iterator iter = unbinds.begin(); iter != unbinds.end(); ++iter )
	{
		const command_set::command_list& cmdList = iter->second.getCommands();
		for( command_set::command_list::const_iterator iiter = cmdList.begin(); iiter != cmdList.end(); ++iiter )
		{
			newFile.AddLine( wxT("unbind\t\t") + iter->first + wxT("\t") + iiter->command );
		}
	}

	//add binds, should be ordered by orderIdx
	const key_binding::key_binding_k2c dobinds = (springbindings - SpringDefaultProfile::getBindings()).getK2C();
	for( key_binding::key_binding_k2c::const_iterator iter = dobinds.begin(); iter != dobinds.end(); ++iter )
	{
		const command_set::command_list& cmdList = iter->second.getCommands();
		for( command_set::command_list::const_iterator iiter = cmdList.begin(); iiter != cmdList.end(); ++iiter )
		{
			newFile.AddLine( wxT("bind\t\t") + iter->first + wxT("\t") + iiter->command );
		}
	}
	newFile.Write();

	const wxString prevFilenameBak = this->m_filename + wxT(".bak");

	//backup our current uikeys.txt
	{
		if ( wxRenameFile( this->m_filename, prevFilenameBak ) == false )
		{
			throw HotkeyException( _("Error renaming uikeys.txt to uikeys.txt.bak") );
		}
	}

	//rename our new tmp file to uikeys.txt. restore backup if failed
	{
		if ( wxRenameFile( newTmpFilename, this->m_filename ) == false )
		{
			wxString msg = _("Error renaming uikeys.txt.tmp to uikeys.txt.");
			
			//restore backup
			if ( wxRenameFile( prevFilenameBak, this->m_filename ) == false )
			{
				msg += _(" Restoring backup failed also.");
			}
			throw HotkeyException( msg );
		}
	}
}
