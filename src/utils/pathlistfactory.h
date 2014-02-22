/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_PATHLISTFACTORY_H
#define SPRINGLOBBY_PATHLISTFACTORY_H

class wxString;
class wxPathList;

class PathlistFactory
{
public:
    static wxPathList fromSinglePath( wxString path );
    static wxPathList ConfigFileSearchPaths();
    static wxPathList UikeysLocations();
    static wxPathList AdditionalSearchPaths( wxPathList& pl );
	static wxPathList EnginePaths(wxPathList& pl, const wxString basedir);
};

#endif // SPRINGLOBBY_PATHLISTFACTORY_H
