/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
#include "conversion.h"

#include <wx/arrstr.h>

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
