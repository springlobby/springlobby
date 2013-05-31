#include "hotkey_parser.h"

#include <fstream>
#include <iostream>

#include <wx/hashmap.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

#include "KeynameConverter.h"
#include "SpringDefaultProfile.h"
#include "hotkey_panel.h"
#include "HotkeyException.h"

#include "../../utils/conversion.h"


hotkey_parser::hotkey_parser(const wxString& uikeys_filename) : m_filename( uikeys_filename ), m_dontTouch( false )
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
		if ( line.Trim().StartsWith( wxT("//SPRINGSETTINGS DO NOT TOUCH") ) )
		{
			this->m_dontTouch = true;
		}

		//look for comments
		int cmtPos = line.Find(wxT("//"));
		if (cmtPos != -1) {
			line.Truncate(cmtPos);	//comment found. cut it.
		}

		line = line.Trim();

		if ( line.size() == 0 )
			continue;

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
		}
		else
		{
			wxLogWarning( wxT( "skipping uikeys.txt line: " ) + line );
			return false;
		}
		return true;
	}
	else if ( tokLine.size() == 2 ) 
	{	//fakemeta=
		const wxString& cmd = tokLine[0];
		const wxString& key = tokLine[1];

		if ( cmd == wxT("fakemeta") )
		{
			this->m_bindings.setMetaKey( key );
		}
		else if ( cmd == wxT("unbindkeyset") )
		{
			this->m_bindings.unbindAllKeys( key );
		}
		else if ( cmd == wxT("unbindaction") )
		{
			this->m_bindings.unbindAllCmds( key );
		}
		else
		{
			wxLogWarning( wxT( "skipping uikeys.txt line: " ) + line );
			return false;
		}
		return true;
	}

	const wxString& cmd = tokLine[0];
	const wxString& key = tokLine[1];

	//append all following tokens to the action string for stuff like "buildspacing inc"
	wxString action; 
	for( unsigned i=2; i < tokLine.size(); ++i )
	{
		action.append( tokLine[i] + wxT(" ") );
	}
	action.Trim();

	if ( cmd == wxT("bind") )
	{
		this->m_bindings.bind( action, KeynameConverter::normalizeSpringKey( key ) );
		return true;
	}
	else if ( cmd == wxT("unbind") )
	{
		this->m_bindings.unbind( action, KeynameConverter::normalizeSpringKey( key ) );
	}
	else if ( cmd == wxT("keysym") )
	{
		this->m_bindings.addKeySym( key, action );
	}
	else if ( cmd == wxT("keyset") )
	{
		this->m_bindings.addKeySymSet( key, action );
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
	const wxString newTmpFilename = wxFileName::CreateTempFileName( _T("uikeys."));
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
        throw HotkeyException( _("Error opening file for reading: ") + m_filename );
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

	//add keysyms
	//key_sym_map keySymRev;
	for( key_sym_map::const_iterator iter = springbindings.getKeySyms().begin(); iter != springbindings.getKeySyms().end(); ++iter )
	{
		newFile.AddLine( wxT("keysym\t\t") + iter->first + wxT("\t\t") + iter->second );
		//keySymRev[ KeynameConverter::convertHexValueToKey( iter->second ) ] = iter->first;
	}

	//add keysyms
	//key_sym_map keySymSetRev;
	for( key_sym_set_map::const_iterator iter = springbindings.getKeySymsSet().begin(); iter != springbindings.getKeySymsSet().end(); ++iter )
	{
		newFile.AddLine( wxT("keyset\t\t") + iter->first + wxT("\t\t") + springbindings.resolveKeySymKey(iter->second ) );
		//keySymSetRev[ iter->second ] = iter->first;
	}

	//add fakemeta
	if ( SpringDefaultProfile::getBindings().getMetaKey() != springbindings.getMetaKey() )
	{
		newFile.AddLine( wxT("fakemeta\t\t") + springbindings.getMetaKey() );		
	}

	//check all default bindings if they still exist in current profile
	//do unbind if not
	const key_commands_sorted unbinds = (SpringDefaultProfile::getBindings() - springbindings).getBinds();
	for( key_commands_sorted::const_iterator iter = unbinds.begin(); iter != unbinds.end(); ++iter )
	{
		newFile.AddLine( wxT("unbind\t\t") + springbindings.resolveKeySymKeyAndSet( iter->first ) + wxT("\t\t") + iter->second );
	}

	//add binds, should be ordered
	const key_commands_sorted dobinds = (springbindings - SpringDefaultProfile::getBindings()).getBinds();
	for( key_commands_sorted::const_iterator iter = dobinds.begin(); iter != dobinds.end(); ++iter )
	{
		newFile.AddLine( wxT("bind\t\t") + springbindings.resolveKeySymKeyAndSet( iter->first ) + wxT("\t\t") + iter->second );
	}

	newFile.Write();

	const wxString prevFilenameBak = this->m_filename + wxT(".bak");

	//backup our current uikeys.txt
	{
		if ( wxRenameFile( this->m_filename, prevFilenameBak ) == false )
		{
            throw HotkeyException( (wxFormat(_("Error renaming %s to %s")) % m_filename % prevFilenameBak )  );
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
				msg += _(" Restoring backup failed also. Good luck!");
			}
			throw HotkeyException( msg );
		}
	}
}

bool hotkey_parser::isDontTouchMode() const
{
	return this->m_dontTouch;
}
