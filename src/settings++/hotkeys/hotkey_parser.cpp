#include "hotkey_parser.h"

#include <fstream>
#include <iostream>

#include <wx/hashmap.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>


hotkey_parser::hotkey_parser(const wxString& uikeys_filename) : filename( uikeys_filename )
{
	const wxString defBindingSearchToken = "Default Bindings:";
	
	wxTextFile uiFile( this->filename );

	if ( !uiFile.Open() )
	{
		wxLogWarning( _( "can't open " + uikeys_filename ) );
		return;
	}

	try
	{
		bool inDefBind = false;
		wxString line;
		for ( line = uiFile.GetFirstLine(); !uiFile.Eof(); line = uiFile.GetNextLine() )
		{
			if ( line.Trim().StartsWith( "//" ) )
			{
				//either skip this comment or read it if we are in the default bindings
				line.Remove( 0, 2 ).Trim(); //remove the comment
				if ( !inDefBind )
				{
					//check if this is the start of the default bindings
					if ( line.Contains( defBindingSearchToken ) )
					{
						inDefBind = true;
					}
					continue;
				}
			}

			if ( line.size() == 0 )
			{
				continue;
			}
			
			if ( !this->processLine( line ) )
			{
				inDefBind = false; //switch to "not-in-default-bindings" if not already
			}
		}
	}
	catch( ... )
	{
		uiFile.Close();
		throw;
	}
	
	this->updateBindsC2K();

	uiFile.Close();
}

bool hotkey_parser::processLine( const wxString& line )
{
	//tokenize line
	std::vector<wxString> tokLine = hotkey_parser::tokenize_uikeys_line( line.c_str() );

	//fakemeta?
	if ( tokLine.size() == 2 ) 
	{
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
		wxLogWarning( _( "skipping uikeys.txt line (unknown token '" + cmd + "'): " + line ) );
		return false;
	}

	return true;
}

hotkey_parser::~hotkey_parser(void)
{
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

const hotkey_parser::key_binding& hotkey_parser::getBindingsK2C() const
{
	return this->bindsK2C;
}

const hotkey_parser::key_binding& hotkey_parser::getBindingsC2K() const
{
	return this->bindsC2K;
}

void hotkey_parser::updateBindsC2K()
{
	this->bindsC2K.clear();

	for( hotkey_parser::key_binding::const_iterator iter = this->bindsK2C.begin(); iter != this->bindsK2C.end(); ++iter )
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
		newFile.close(); //already opened this
		throw std::runtime_error( std::string("Error opening file for writing: ") + newTmpFilename.ToAscii() );
	}

	//now read the old uikeys.txt line after line and copy all comments to the new file
	try
	{
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
		newFile.close();
		oldFile.close();
	}
	catch(...)
	{
		newFile.close();
		oldFile.close();
	}
}
