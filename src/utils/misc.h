/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_MISC_H
#define SPRINGLOBBY_HEADERGUARD_MISC_H

#include <string>
#include <vector>
class wxArrayString;
class wxString;

/**
 * @brief Computes Levenshtein distance (edit distance) between two strings.
 * @return the Levenshtein distance normalized by the longest string's length.
 * @note Source: https://en.wikipedia.org/wiki/Levenshtein_distance
 */
double LevenshteinDistance(const wxString& _s, const wxString& _t);

/**
 * @brief Gets the closest match for s in a, using LevenshteinDistance.
 * @param distance If not NULL, *distance is set to the edit distance from s to the return value.
 */
wxString GetBestMatch(const wxArrayString& a, const wxString& s, double* distance = 0);
std::string GetBestMatch(const std::vector<std::string>& a, const std::string& s, double* distance = 0);

#endif // SPRINGLOBBY_HEADERGUARD_MISC_H
