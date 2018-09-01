/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "misc.h"

#include <lslutils/misc.h>
#include <wx/arrstr.h>
#include <wx/log.h>
#include <wx/string.h>
#include <vector>

#include "conversion.h"
#include "settings.h"

double LevenshteinDistance(const wxString& _s, const wxString& _t)
{
	const wxString s = _s.Lower(); // case insensitive edit distance
	const wxString t = _t.Lower();

	const int m = s.length(), n = t.length(), _w = m + 1;
	std::vector<unsigned char> _d((m + 1) * (n + 1));
#define D(x, y) _d[(y)*_w + (x)]

	for (int i = 0; i <= m; ++i)
		D(i, 0) = i;
	for (int j = 0; j <= n; ++j)
		D(0, j) = j;

	for (int i = 1; i <= m; ++i) {
		for (int j = 1; j <= n; ++j) {
			const int cost = (s[i - 1] != t[j - 1]);
			D(i, j) = LSL::Util::Min(D(i - 1, j) + 1,	 // deletion
						 D(i, j - 1) + 1,	 // insertion
						 D(i - 1, j - 1) + cost); // substitution
		}
	}
	double d = (double)D(m, n) / std::max(m, n);
	wxLogMessage(_T("LevenshteinDistance('%s', '%s') = %g"), s.c_str(), t.c_str(), d);
	return d;
#undef D
}

#ifndef TEST
#include "lslconversion.h"
std::string GetBestMatch(const std::vector<std::string>& a, const std::string& s, double* distance)
{
	auto arr = lslTowxArrayString(a);
	return STD_STRING(GetBestMatch(arr, TowxString(s), distance));
}
#endif

wxString GetBestMatch(const wxArrayString& a, const wxString& s, double* distance)
{
	const unsigned int count = a.GetCount();
	double minDistance = 1.0;
	int minDistanceIndex = -1;
	for (unsigned int i = 0; i < count; ++i) {
		const double distance_ = LevenshteinDistance(a[i], s);
		if (distance_ < minDistance) {
			minDistance = distance_;
			minDistanceIndex = i;
		}
	}
	if (distance != NULL)
		*distance = minDistance;
	if (minDistanceIndex == -1)
		return wxEmptyString;
	return a[minDistanceIndex];
}
