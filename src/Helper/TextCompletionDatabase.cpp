#include "TextCompletionDatabase.hpp"

//--------------------------------------------------------------------------------
///
/// Konstruktor
///
//--------------------------------------------------------------------------------
TextCompletionDatabase::TextCompletionDatabase() {

}

//--------------------------------------------------------------------------------
///
/// Destruktor
///
//--------------------------------------------------------------------------------
TextCompletionDatabase::~TextCompletionDatabase() {

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
TextCompletionDatabase::Size() {

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
void
TextCompletionDatabase::Insert_Mapping( wxString abbreviation, wxString mapping ) {

	HashMap_String_String::iterator iter = hm.find( abbreviation );

	if ( iter == hm.end() ) {
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
void
TextCompletionDatabase::Delete_Mapping( wxString abbreviation ) {

	HashMap_String_String::iterator iter = hm.find( abbreviation );

	if ( iter != hm.end() ) {
		hm.erase( iter );
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
TextCompletionDatabase::GetMapping( wxString abbreviation ) {

	HashMap_String_String hashmap;

	// Search for all Abbreviations, containing the provided one.

	// The Regex Container
	wxRegEx regex_Abbreviations;

	// We build the regular Expression:
	wxString regex_Text;
	regex_Text.append( abbreviation );
	regex_Text.append( wxT( ".*?" ) );

	// We compile the regular Expression and if it's correct, we store it in the Regex Container
	regex_Abbreviations.Compile( regex_Text, wxRE_ADVANCED );

	// Now we iterate over all stored Abbreviations and search for Abbreviations containing the provided Abbreviation
	for ( HashMap_String_String::iterator iter = hm.begin() ; iter != hm.end() ; ++iter ) {
		if ( regex_Abbreviations.Matches( iter->first ) ) {
			hashmap[ iter->first ] = iter->second;
		}
	}

	return hashmap;
}
