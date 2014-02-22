/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "pathlistfactory.h"

#include <wx/string.h>
#include "../defines.h" //to get HAVEWX??
#include "platform.h"
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>

const wxChar sep = wxFileName::GetPathSeparator();
const wxString sepstring = wxString(sep);



wxPathList PathlistFactory::fromSinglePath(wxString path)
{
    wxPathList pl;
    pl.Add( path );
    return pl;
}

wxPathList PathlistFactory::ConfigFileSearchPaths()
{
    wxPathList pl;

    pl.AddEnvList( _T( "%ProgramFiles%" ) );

    pl.AddEnvList( _T( "LDPATH" ) );
    pl.AddEnvList( _T( "LD_LIBRARY_PATH" ) );

    pl.Add( _T( "/usr/local/lib/spring" ) );
    pl.Add( _T( "/usr/local/lib64" ) );
    pl.Add( _T( "/usr/local/games" ) );
    pl.Add( _T( "/usr/local/games/lib" ) );
    pl.Add( _T( "/usr/local/lib" ) );
    pl.Add( _T( "/usr/lib64" ) );
    pl.Add( _T( "/usr/lib" ) );
    pl.Add( _T( "/usr/lib/spring" ) );
    pl.Add( _T( "/usr/games" ) );
    pl.Add( _T( "/usr/games/lib64" ) );
    pl.Add( _T( "/usr/games/lib" ) );

    pl = AdditionalSearchPaths( pl );

    return pl;
}

wxPathList PathlistFactory::UikeysLocations()
{
    wxPathList pl;
    pl.AddEnvList( _T("%ProgramFiles%") );
    pl.AddEnvList( _T("XDG_DATA_DIRS") );
    pl = AdditionalSearchPaths( pl );
    return pl;
}

wxPathList PathlistFactory::EnginePaths(wxPathList& pl, const wxString basedir)
{
	const wxString enginedir = basedir + sep + _T("engine");
	wxDir dir(enginedir);

	if ( dir.IsOpened() ) {
		wxString filename;
		bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS|wxDIR_HIDDEN);
		while ( cont )
		{
			pl.Add(enginedir + sepstring + filename);
			cont = dir.GetNext(&filename);
		}
	}
	return pl;
}

wxPathList PathlistFactory::AdditionalSearchPaths(wxPathList &pl)
{
    wxPathList ret;
    wxStandardPathsBase& sp = wxStandardPathsBase::Get();

    pl.Add( wxFileName::GetCwd() );
    pl.Add( sp.GetExecutablePath().BeforeLast( sep ) );
    pl.Add( wxFileName::GetHomeDir() );
    pl.Add( wxGetOSDirectory() );

#ifdef __WXMSW__
    pl.Add( sp.GetDocumentsDir() + sepstring + wxT("My Games") + sepstring + wxT("Spring") );
    //maybe add more here like:
    //Appdata + \Spring
    //Mydocs + \Spring
#endif

    for ( size_t i = 0; i < pl.GetCount(); i++ )
    {
        wxString path = pl[i];
        if ( !path.EndsWith( sepstring ) )
            path += sepstring;
        ret.Add( path );
        ret.Add( path + _T( "Spring" ) + sepstring );
        ret.Add( path + _T( "spring" ) + sepstring );
        ret.Add( path + _T( "games" ) + sepstring + _T( "Spring" ) + sepstring );
        ret.Add( path + _T( "games" ) + sepstring + _T( "spring" ) + sepstring );
    }

// search in ~/.spring/engine/ , too
#ifdef __WXMSW__
	const wxString userdir = sp.GetDocumentsDir() + sep + _T("My Games") + sep + _T("Spring");
#else
	const wxString userdir = wxFileName::GetHomeDir() + sep + _T(".spring");
#endif

	EnginePaths(ret, userdir);

	return ret;
}
