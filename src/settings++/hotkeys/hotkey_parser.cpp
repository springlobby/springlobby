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
	f.open( filename.c_str() );

	if ( !f.is_open() )
	{
		const wxString msg = _("Could not open file for writing: ") + filename;
		throw std::runtime_error( msg.mb_str(wxConvUTF8) );
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
/*
bool hotkey_parser::isKeyInProfile( const key_binding& kbBinding, const wxString& command, const wxString& kbBeystring )
{
	key_binding::const_iterator cmdIter = kbBinding.find( command );
	if ( cmdIter == kbBinding.end() )
	{
		return false;
	}

	for( key_set::const_iterator iter = cmdIter->second.begin(); iter != cmdIter->second.end(); ++iter )
	{
		if ( KeynameConverter::compareSpring2wxKeybinder( kbBeystring, (*iter) ) )
		{
			return true;
		}
	}

	return false;
}*/

void hotkey_parser::writeBindingsToFile( const key_binding& springbindings )
{
	const wxString newTmpFilename = this->filename + wxT(".tmp");

	//open new file for writing
	std::ofstream newFile;
	newFile.open( newTmpFilename.c_str() );
	if ( !newFile.is_open() )
	{
		const wxString msg = _("Error opening file for writing: ") + newTmpFilename;
		throw std::runtime_error( msg.mb_str(wxConvUTF8) );
	}

	//open old file for reading
	std::ifstream oldFile;
	oldFile.open( this->filename.c_str() );
	if ( !oldFile.is_open() )
	{
		const wxString msg = _("Error opening file for reading: ") + this->filename;
		throw std::runtime_error( std::string( msg.mb_str(wxConvUTF8) ) );
	}

	//now read the old uikeys.txt line after line and copy all comments to the new file
	std::string line;
	while ( !oldFile.eof() )
	{
		std::getline (oldFile, line);
		wxString wxLine = line;
		wxLine.Trim();
		if ( wxLine.StartsWith(wxT("//")) )
		{
			newFile << line << wxT('\n');
		}
	}

	const key_binding defBinds = SpringDefaultProfile::getAllBindingsC2K();
	for( key_binding::const_iterator iter = defBinds.begin(); iter != defBinds.end(); ++iter )
	{
		for( key_set::const_iterator iiter = iter->second.begin(); iiter != iter->second.end(); ++iiter )
		{
			if ( !hotkey_panel::isBindingInProfile( springbindings, iter->first, (*iiter) ) )
			{
				newFile << wxT("unbind\t\t") << (*iiter) << wxT("\t") << iter->first << wxT("\n");
			}
		}
	}

	for( key_binding::const_iterator iter = springbindings.begin(); iter != springbindings.end(); ++iter )
	{
		for( key_set::const_iterator iiter = iter->second.begin(); iiter != iter->second.end(); ++iiter )
		{
			if ( !hotkey_panel::isDefaultBinding( iter->first, (*iiter) ) )
			{
				newFile << wxT("bind\t\t") << KeynameConverter::spring2wxKeybinder( (*iiter), true ) << wxT("\t") << iter->first << wxT("\n");
			}
		}
	}

	oldFile.close();
	newFile.close();

	//delete old backup
	const wxString prevFilenameBak = wxT("uikeys.txt.bak");
	{
		int rc = _unlink( prevFilenameBak.mbc_str() );
		if ( rc != 0 )
		{
			const wxString msg = _("Error deleting backup file uikeys.txt.bak: ") + wxString( strerror( errno ) );
			throw std::runtime_error( msg.mb_str() );
		}
	}
	
	//backup our current uikeys.txt
	{
		int rc = rename( this->filename.c_str(), prevFilenameBak.c_str() );
		if ( rc != 0 )
		{
			const wxString msg = _("Error renaming uikeys.txt to uikeys.txt.bak: ") + wxString( strerror( errno ) );
			throw std::runtime_error( msg.mb_str() );
		}
	}

	//rename our new tmp file to uikeys.txt. restore backup if failed
	{
		int rc = rename( newTmpFilename.c_str(), this->filename.c_str() );
		if ( rc != 0 )
		{
			int rc = rename( prevFilenameBak.To8BitData(), this->filename.To8BitData() );
			const wxString msg = _("Error renaming uikeys.txt.tmp to uikeys.txt: ") + wxString( strerror( errno ) );
			throw std::runtime_error( msg.mb_str() );
		}
	}
}
