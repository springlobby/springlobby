/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
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
