/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "TextCompletionDatabase.h"

#include <wx/regex.h>
#include <wx/string.h>
//--------------------------------------------------------------------------------
///
/// Konstruktor
///
//--------------------------------------------------------------------------------
TextCompletionDatabase::TextCompletionDatabase()
{
}

//--------------------------------------------------------------------------------
///
/// Destruktor
///
//--------------------------------------------------------------------------------
TextCompletionDatabase::~TextCompletionDatabase()
{
}

//--------------------------------------------------------------------------------
///
/// Returns the current Count of Mapping in the TextCompletionDatabase.
///
/// \return
///		The current Count of Mapping in the TextCompletionDatabase.
///
//--------------------------------------------------------------------------------
unsigned int
TextCompletionDatabase::Size()
{

	return hm.size();
}

//--------------------------------------------------------------------------------
///
/// Insert an Abbreviation and it's corresponding Mapping into the TextCompletionDatabase
///
/// \parem abbreviaton
///		The Abbreviation to insert into the TextCompletionDatabase.
///
/// \parem mapping
///		The Mapping of Abbreviation to insert into the TextCompletionDatabase.
///
//--------------------------------------------------------------------------------
void TextCompletionDatabase::Insert_Mapping(const wxString& abbreviation, const wxString& mapping)
{

	HashMap_String_String::const_iterator iter = hm.find(abbreviation);

	if (iter == hm.end()) {
		hm[abbreviation] = mapping;
	}
}

//--------------------------------------------------------------------------------
///
/// Delete an Abbreviation and it's corresponding Mapping into the TextCompletionDatabase
///
/// \parem abbreviaton
///		The Abbreviation to insert into the TextCompletionDatabase.
///
//--------------------------------------------------------------------------------
void TextCompletionDatabase::Delete_Mapping(const wxString& abbreviation)
{

	HashMap_String_String::iterator iter = hm.find(abbreviation);

	if (iter != hm.end()) {
		hm.erase(iter);
	}
}

//--------------------------------------------------------------------------------
///
/// Get all Abbreviations, that match the provided Abbreviation. All matching Abbreviations and their corresponding Mapping are returned.
///
/// \parem abbreviaton
///		The Abbreviation to search for matching Abbreviations already contained in the TextCompletionDatabase.
///
/// \return
///		A HashMap containing all Matches.
///
//--------------------------------------------------------------------------------
HashMap_String_String
TextCompletionDatabase::GetMapping(wxString abbreviation)
{

	HashMap_String_String hashmap;

	// Search for all Abbreviations, containing the provided one.

	// The Regex Container
	wxRegEx regex_Abbreviations;

	// We build the regular Expression:

	// We need to escape all regular Expression Characters, that have a special Meaning
	abbreviation.Replace(_T("["), _T("\\["));
	abbreviation.Replace(_T("]"), _T("\\]"));

	wxString regex_Text;
	regex_Text.append(abbreviation);
	regex_Text.append(wxT(".*"));

// We compile the regular Expression and if it's correct, we store it in the Regex Container
#ifdef wxHAS_REGEX_ADVANCED
	regex_Abbreviations.Compile(regex_Text, wxRE_ADVANCED | wxRE_ICASE);
#else
	regex_Abbreviations.Compile(regex_Text, wxRE_EXTENDED | wxRE_ICASE);
#endif


	// Now we iterate over all stored Abbreviations and search for Abbreviations containing the provided Abbreviation

	// std::cout << "Abbr: (" << abbreviation.char_str() << ")" << std::endl;

	for (HashMap_String_String::const_iterator iter = hm.begin(); iter != hm.end(); ++iter) {
		// std::cout << "iter->first: (" << iter->first.char_str() << ")" << std::endl;
		if (regex_Abbreviations.Matches(iter->first)) {
			// std::cout << "Match found!" << std::endl;
			hashmap[iter->first] = iter->second;
		}
	}

	return hashmap;
}
