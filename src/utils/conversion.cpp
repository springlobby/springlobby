/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "conversion.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

StringtokenizerVectorized::StringtokenizerVectorized( wxStringTokenizer tokenizer )
{
    reserve( tokenizer.CountTokens() );
    while ( tokenizer.HasMoreTokens() )
        push_back( tokenizer.GetNextToken() );
}

std::string stdprintf(const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	int count = vsnprintf(buf, 1024, format, args);
	va_end(args);
	return std::string(buf, count);
}
