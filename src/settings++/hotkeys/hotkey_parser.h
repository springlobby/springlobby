#ifndef __TAB_PARSER_h__
#define __TAB_PARSER_h__

#include <map>
#include <set>
#include <vector>

#include <wx/string.h>

#include "HotkeyTypes.h"

class hotkey_parser
{
public:
	hotkey_parser( const wxString& filename );
	~hotkey_parser();

	const key_binding& getBindingsC2K() const;
	const key_binding& getBindingsK2C() const;
	
	void writeBindingsToFile( const key_binding& bindings );

	void dumpIncludeSourceCode( const wxString& filename );

private:
	static std::vector< wxString > tokenize_uikeys_line( const wxString& line );
	bool processLine( const wxString& line );

	void updateBindsC2K();

	key_binding		bindsK2C;
	key_binding		bindsC2K;

	wxString		filename;
};

#endif