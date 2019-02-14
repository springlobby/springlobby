/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef LSLCONVERSION_H
#define LSLCONVERSION_H

class wxColour;
class wxArrayString;
namespace LSL
{
class lslColor;
typedef std::vector<std::string> StringVector;
} // namespace LSL

wxColour lslTowxColour(const LSL::lslColor& col);
LSL::lslColor wxColourTolsl(const wxColour& col);

LSL::StringVector wxArrayStringToLSL(const wxArrayString& arr);
wxArrayString lslTowxArrayString(const LSL::StringVector& arr);

#endif
