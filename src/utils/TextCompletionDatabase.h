/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef TEXTCOMPLETIONDATABASE_HPP
#define TEXTCOMPLETIONDATABASE_HPP

// wxWidgets
#include <wx/hashmap.h>
#include <wx/string.h>
WX_DECLARE_STRING_HASH_MAP(wxString, HashMap_String_String);


class TextCompletionDatabase
{
public:
	TextCompletionDatabase();
	virtual ~TextCompletionDatabase();

	unsigned int Size();

	void Insert_Mapping(const wxString& abbreviation, const wxString& mapping);
	void Delete_Mapping(const wxString& abbreviation);
	HashMap_String_String GetMapping(wxString text);

private:
	HashMap_String_String hm;
};

#endif // TEXTCOMPLETIONDATABASE_HPP
