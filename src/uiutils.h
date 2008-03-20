#ifndef SPRINGLOBBY_HEADERGUARD_UIUTILS_H
#define SPRINGLOBBY_HEADERGUARD_UIUTILS_H

class wxString;
class wxColour;

struct ReplayData
{
  long int day;
  long int month;
  long int year;
  wxString MapName;
  wxString SpringVersion;
  wxString ReplayName;
};

#define bool2yn(b) ((b)?_("Yes"):_("No"))

const int colour_values[][3] = { {240,210,0}, {128, 128, 128}, {0, 0, 128}, {0, 0, 255},
      {0, 128, 0}, {0, 255, 0}, {0, 128, 128}, {0, 255, 255}, {128, 0, 0}, {255, 0, 0},
      {128, 0, 128}, {255, 0, 255}, {128, 128, 0}, {255, 255, 0}, {192, 192, 192}, {0, 220, 250}
};

wxString RefineMapname( const wxString& mapname );
wxString RefineModname( const wxString& modname );
wxString RTFtoText( const wxString& rtfinput );
bool AreColoursSimilar( const wxColour& col1, const wxColour& col2, int mindiff = 10 );

void ColourDelta( int& r, int& g, int& b, const int& delta );
wxColour ColourDelta( const wxColour& colour, const int& delta );

ReplayData GetReplayInfos ( wxString& ReplayPath );
wxString GetColorString( const wxColour& color );
wxColour GetColorFromStrng( const wxString color );

#endif // SPRINGLOBBY_HEADERGUARD_UIUTILS_H
