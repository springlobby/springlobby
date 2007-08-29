#ifndef SPRINGLOBBY_HEADERGUARD_UIUTILS_H
#define SPRINGLOBBY_HEADERGUARD_UIUTILS_H

#include <wx/string.h>

class wxColour;


#define bool2yn(b) ((b)?_("Yes"):_("No"))

const wxString colour_choices[] = {
  _T("gold"), _T("dark gray"), _T("dark blue"), _T("bright blue"), _T("dark green"),
  _T("bright green"), _T("dark cyan"), _T("bright cyan"), _T("dark red"), _T("bright red"),
  _T("dark magenta"), _T("bright magenta"), _T("dark yellow"), _T("bright yellow"),
  _T("light gray"), _T("inky blue")
};

const int colour_values[][3] = { {240,210,0}, {128, 128, 128}, {0, 0, 128}, {0, 0, 255},
      {0, 128, 0}, {0, 255, 0}, {0, 128, 128}, {0, 255, 255}, {128, 0, 0}, {255, 0, 0},
      {128, 0, 128}, {255, 0, 255}, {128, 128, 0}, {255, 255, 0}, {192, 192, 192}, {0, 220, 250}
};

wxString RefineMapname( wxString mapname );
wxString RefineModname( wxString modname );
bool AreColoursSimilar( int r1, int g1, int b1, int r2, int g2, int b2, int mindiff = 10 );

void ColourDelta( int& r, int& g, int& b, const int& delta );
wxColour ColourDelta( const wxColour& colour, const int& delta );

#endif // SPRINGLOBBY_HEADERGUARD_UIUTILS_H
