/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


#include <lslutils/misc.h>
#include <wx/colour.h>

#include "utils/conversion.h"

wxColour lslTowxColour(const LSL::lslColor& col)
{
	if (col.Alpha() != 255) {
		return wxColour(col.Red(), col.Green(), col.Blue(), col.Alpha());
	}
	return wxColour(col.Red(), col.Green(), col.Blue());
}

LSL::lslColor wxColourTolsl(const wxColour& col)
{
	if (col.IsOk())
		return LSL::lslColor(col.Red(), col.Green(), col.Blue(), col.Alpha());
	return LSL::lslColor();
}

LSL::StringVector wxArrayStringToLSL(const wxArrayString& arr)
{
	LSL::StringVector ret;
	for (const wxString& str : arr) {
		ret.push_back(STD_STRING(str));
	}
	return ret;
}

wxArrayString lslTowxArrayString(const LSL::StringVector& arr)
{
	wxArrayString ret;
	for (size_t i = 0; i < arr.size(); i++) {
		ret.Add(TowxString(arr[i]));
	}
	return ret;
}
