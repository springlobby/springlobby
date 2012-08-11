#ifndef SPRINGLOBBY_HEADERGUARD_MISC_H
#define SPRINGLOBBY_HEADERGUARD_MISC_H

#include <vector>

class wxArrayString;
class wxString;

/**
 * @brief Computes Levenshtein distance (edit distance) between two strings.
 * @return the Levenshtein distance normalized by the longest string's length.
 * @note Source: http://en.wikipedia.org/wiki/Levenshtein_distance
 */
double LevenshteinDistance(wxString s, wxString t);

/**
 * @brief Gets the closest match for s in a, using LevenshteinDistance.
 * @param distance If not NULL, *distance is set to the edit distance from s to the return value.
 */
wxString GetBestMatch(const wxArrayString& a, const wxString& s, double* distance = 0 );
std::string GetBestMatch(const std::vector<std::string>& a, const std::string& s, double* distance = 0 );

wxString Paste2Pastebin( const wxString& message );


/** Try to create the named directory, if it doesn't exist.
 *
 * @param name Path to directory that should exist or be created
 *
 * @param perm Value of @p perm parameter for wxFileName::Mkdir.
 *
 * @param flags Value of @p flags parameter for wxFileName::Mkdir.
 *
 * @return @c true if the directory already exists, or the return
 * value of wxFileName::Mkdir if it does not.
 */
bool tryCreateDirectory( const wxString& name, int perm = 0775, int flags = 0 );

#endif // SPRINGLOBBY_HEADERGUARD_MISC_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
