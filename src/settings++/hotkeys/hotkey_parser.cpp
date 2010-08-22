#include "hotkey_parser.h"

#include <fstream>
#include <iostream>

#include <wx/hashmap.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>

#include "SpringDefaultProfile.h"

hotkey_parser::hotkey_parser(const wxString& uikeys_filename) : filename( uikeys_filename )
{
	//we will read the uikeys.txt now to get the key profile
	//1. Fill the profile with spring's default bindings
	this->bindsK2C = SpringDefaultProfile::getAllBindingsK2C();

	//2. now read uikeys.txt and modify the default profile
	wxTextFile uiFile( this->filename );

	if ( !uiFile.Open() )
	{
		wxLogWarning( _( "can't open " + uikeys_filename ) );
		return;
	}

	wxString line;
	for ( line = uiFile.GetFirstLine(); !uiFile.Eof(); line = uiFile.GetNextLine() )
	{
		if ( line.Trim().StartsWith( "//" ) )
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
		if ( tokLine[0] == "unbindall" )
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

		if ( cmd == "fakemeta" )
		{
			this->bindsK2C[key].insert( cmd );
			return true;
		}
	}

	const wxString& cmd = tokLine[0];
	const wxString& key = tokLine[1];

	//append all following tokens to the action string for stuff like "buildspacing inc"
	wxString action; 
	for( unsigned i=2; i < tokLine.size(); ++i )
	{
		action.append( tokLine[i] + " " );
	}
	action.Trim();

	if ( cmd == "bind" )
	{
		this->bindsK2C[key].insert( action );
		return true;
	}
	else if ( cmd == "unbind" )
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
		wxLogWarning( _( "skipping uikeys.txt line (unknown token '" ) + cmd + "'): " + line );
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
		throw std::runtime_error( std::string("Could not open file for writing: ") + filename.ToAscii() );
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
	wxStringTokenizer tkz( line, " \t" );

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

void hotkey_parser::writeBindingsToFile( const key_binding& bindings )
{
	const wxString newTmpFilename = this->filename + _T(".tmp");

	//open new file for writing
	std::ofstream newFile;
	newFile.open( newTmpFilename.To8BitData() );
	if ( !newFile.is_open() )
	{
		throw std::runtime_error( std::string("Error opening file for writing: ") + newTmpFilename.ToAscii() );
	}

	//open old file for reading
	std::ifstream oldFile;
	oldFile.open( this->filename.To8BitData() );
	if ( !oldFile.is_open() )
	{
		throw std::runtime_error( std::string("Error opening file for writing: ") + newTmpFilename.ToAscii() );
	}

	//now read the old uikeys.txt line after line and copy all comments to the new file
	std::string line;
	while ( !oldFile.eof() )
	{
		std::getline (oldFile,line);
		wxString wxLine = line;
		wxLine.Trim();
		if ( wxLine.StartsWith("//") )
		{
			newFile << line;
		}
	}
}
