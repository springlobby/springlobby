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

	const key_binding& getBindings() const;
	
	void writeBindingsToFile( const key_binding& springbindings );

	bool isDontTouchMode() const;
	//void dumpIncludeSourceCode( const wxString& filename );

private:
	//static bool isKeyInProfile( const key_binding& binding, const wxString& command, const wxString& keystring );
	static std::vector< wxString > tokenize_uikeys_line( const wxString& line );
	bool processLine( const wxString& line );

	void updateBindsC2K();

	key_binding			m_bindings;
	
    const wxString		m_filename;

	bool				m_dontTouch;
};

#endif
