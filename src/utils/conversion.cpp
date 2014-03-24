/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "conversion.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

int ConvertWXArrayToC(const wxArrayString& aChoices, wxString **choices)
{
    int n = aChoices.GetCount();
    *choices = new wxString[n];

    for ( int i = 0; i < n; i++ )
    {
        (*choices)[i] = aChoices[i];
    }

    return n;
}

TransformedArrayString::TransformedArrayString( const wxArrayString& original, wxString trans_op (const wxString& ) )
{
    for ( size_t i = 0; i < original.Count(); i++ )
        Add( trans_op( original[i] ) );
}

StringtokenizerVectorized::StringtokenizerVectorized( wxStringTokenizer tokenizer )
{
    reserve( tokenizer.CountTokens() );
    while ( tokenizer.HasMoreTokens() )
        push_back( tokenizer.GetNextToken() );
}

namespace LSL { namespace Util {

wxArrayString vectorToArrayString(const std::vector<std::string>& vec) {
    wxArrayString arr;
    for(const auto& s : vec)
        arr.Add(TowxString(s));
    return arr;
}

std::vector<std::string> arrayStringToVector(const wxArrayString& arr) {
    std::vector<std::string> vec;
    vec.reserve(arr.Count());
    for(auto s : arr)
        vec.push_back(STD_STRING(s));
    return vec;
}

} } //end namespace LSL::Util

std::string stdprintf(const std::string& format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	int count = vsnprintf(buf, 1024, format.c_str(), args);
	va_end(args);
	return std::string(buf, count);
}
