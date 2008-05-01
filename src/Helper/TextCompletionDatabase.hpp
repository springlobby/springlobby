#ifndef TEXTCOMPLETIONDATABASE_HPP
#define TEXTCOMPLETIONDATABASE_HPP

// Standard C++ Libraries
#include <iostream>

// wxWidgets
#include <wx/hashmap.h>
#include <wx/regex.h>
#include <wx/string.h>


WX_DECLARE_STRING_HASH_MAP( wxString, HashMap_String_String );


class TextCompletionDatabase {

	private:
		HashMap_String_String hm;

	public:
		TextCompletionDatabase();
		virtual ~TextCompletionDatabase();

		unsigned int Size();

		void Insert_Mapping( wxString abbreviation, wxString mapping );
		void Delete_Mapping( wxString abbreviation );
		HashMap_String_String GetMapping( wxString text );



	protected:

};

#endif // TEXTCOMPLETIONDATABASE_HPP
