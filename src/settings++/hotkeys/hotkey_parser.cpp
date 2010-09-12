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


hotkey_parser::hotkey_parser(const wxString& uikeys_filename) : filename( uikeys_filename )
{
	//we will read the uikeys.txt now to get the key profile
	//1. Fill the profile with spring's default bindings
	KeynameConverter::initialize();

	this->bindsK2C = SpringDefaultProfile::getAllBindingsK2C();

	//2. now read uikeys.txt and modify the default profile
	wxTextFile uiFile( this->filename );

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

	//update the reverse map
	this->updateBindsC2K();
}

bool hotkey_parser::processLine( const wxString& line )
{
	//tokenize line
	std::vector<wxString> tokLine = hotkey_parser::tokenize_uikeys_line( line.c_str() );

	if ( tokLine.size() == 1 )
	{ //unbindall?
		if ( tokLine[0] == wxT("unbindall") )
		{
			bindsK2C.clear();
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
			this->bindsK2C[key].insert( cmd );
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
		this->bindsK2C[key].insert( action );
		return true;
	}
	else if ( cmd == wxT("unbind") )
	{
		if ( this->bindsK2C.find( key ) == this->bindsK2C.end() )
		{
			//nothing to unbind
			return true;
		}

		if ( this->bindsK2C[key].find( action ) == this->bindsK2C[key].end() )
		{
			//nothing to unbind
			return true;
		} 

		this->bindsK2C[key].erase( action );
		if ( this->bindsK2C[key].size() == 0 )
		{
			//delete empty keys
			this->bindsK2C.erase( key );
		}
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

void hotkey_parser::dumpIncludeSourceCode( const wxString& filename )
{
	std::ofstream f;
	f.open( C_STRING(filename) );

	if ( !f.is_open() )
	{
		throw HotkeyException( _("Could not open file for writing: ") + filename );
	}

	for( key_binding::const_iterator iter = bindsC2K.begin(); iter != bindsC2K.end(); ++iter )
	{
		const key_set& keys = iter->second;
		for( key_set::const_iterator iiter = keys.begin(); iiter != keys.end(); iiter++ )
		{
			f << "SpringDefaultProfile::addBinding( _T(\"";
			f << iter->first;
			f << "\"), _T(\"";
			f << (*iiter);
			f << "\") );";
			f << '\n';
		}
	}
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

const key_binding& hotkey_parser::getBindingsK2C() const
{
	return this->bindsK2C;
}

const key_binding& hotkey_parser::getBindingsC2K() const
{
	return this->bindsC2K;
}

void hotkey_parser::updateBindsC2K()
{
	this->bindsC2K.clear();

	for( key_binding::const_iterator iter = this->bindsK2C.begin(); iter != this->bindsK2C.end(); ++iter )
	{
		for( key_set::const_iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2 )
		{
			this->bindsC2K[*iter2].insert( iter->first );
		}
	}
}

void hotkey_parser::writeBindingsToFile( const key_binding& springbindings )
{
	const wxString newTmpFilename = this->filename + wxT(".tmp");
	wxTextFile newFile( newTmpFilename );

	//open new file for writing
	if ( ( !newFile.Exists() && !newFile.Create() ) || ( newFile.Exists() && !newFile.Open() ) )
	{
		throw HotkeyException( _("Error opening file for writing: ") + newTmpFilename );
	}

	//now read the old uikeys.txt line after line and copy all comments to the new file
	wxTextFile oldFile( this->filename );
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

	const key_binding defBinds = SpringDefaultProfile::getAllBindingsC2K();
	for( key_binding::const_iterator iter = defBinds.begin(); iter != defBinds.end(); ++iter )
	{
		for( key_set::const_iterator iiter = iter->second.begin(); iiter != iter->second.end(); ++iiter )
		{
			if ( !hotkey_panel::isBindingInProfile( springbindings, iter->first, (*iiter) ) )
			{
				newFile.AddLine( wxT("unbind\t\t") + (*iiter) + wxT("\t") + iter->first );
			}
		}
	}

	for( key_binding::const_iterator iter = springbindings.begin(); iter != springbindings.end(); ++iter )
	{
		for( key_set::const_iterator iiter = iter->second.begin(); iiter != iter->second.end(); ++iiter )
		{
			if ( !hotkey_panel::isDefaultBinding( iter->first, (*iiter) ) )
			{
				newFile.AddLine( wxT("bind\t\t") + KeynameConverter::spring2wxKeybinder( (*iiter), true ) + wxT("\t") + iter->first );
			}
		}
	}
	newFile.Write();

	const wxString prevFilenameBak = this->filename + wxT(".bak");

	int errno = 0;

	//backup our current uikeys.txt
	{
		if ( wxRenameFile( this->filename, prevFilenameBak ) == false )
		{
			throw HotkeyException( _("Error renaming uikeys.txt to uikeys.txt.bak: ") + TowxString(errno) );
		}
	}

	//rename our new tmp file to uikeys.txt. restore backup if failed
	{
		if ( wxRenameFile( newTmpFilename, this->filename ) == false )
		{
			wxString msg = _("Error renaming uikeys.txt.tmp to uikeys.txt: ") + TowxString(errno);
			
			//restore backup
			if ( wxRenameFile( prevFilenameBak, this->filename ) == false )
			{
				msg += _(" Restoring backup failed also: ") + TowxString(errno);
			}
			throw HotkeyException( msg );
		}
	}
}
