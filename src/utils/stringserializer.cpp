
#include <wx/filename.h>
#include <wx/textfile.h>
#include "../settings.h"

template <class ImpType>
StringSerializer<ImpType>::StringSerializer()
{}

template <class ImpType>
StringSerializer<ImpType>::~StringSerializer()
{}

/** @brief VectorFromFile
  *
  * @todo: document this function
  */

template <class ImpType>
std::vector<ImpType> StringSerializer<ImpType>::VectorFromFile(const wxString& filename)
{
    wxString basedir = sett().GetCachePath();
    wxTextFile file( basedir + wxFileName::GetPathSeparator() + filename );
    if ( !file.Open() )
    {
        return std::vector<ImpType>();
    }
    std::vector<ImpType> vector;
    wxString line;
    for ( line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine() )
    {
        ImpType obj;
        obj.FromString( line );
        vector.push_back( obj );
    }

    //this is the last line
    ImpType obj;
    obj.FromString( line );
    vector.push_back( obj );

    return vector;
}

/** @brief VectorToFile
  *
  * @todo: document this function
  */
template <class ImpType>
bool StringSerializer<ImpType>::VectorToFile(const std::vector<ImpType>& vector, const wxString& filename)
{
    wxString basedir = sett().GetCachePath();
    wxTextFile file( basedir + wxFileName::GetPathSeparator() + filename );
    if ( !file.Open() )
    {
        if ( !file.Create() )
            return false;
    }
    file.Clear();
    for ( typename std::vector<ImpType>::const_iterator it = vector.begin(); it != vector.end(); ++it )
    {
        file.AddLine( it->ToString() );
    }
    return file.Write();
}

/** @brief FromFile
  *
  * @todo: document this function
  */
template <class ImpType>
bool StringSerializer<ImpType>::FromFile(const wxString& filename)
{
    wxString basedir = sett().GetCachePath();
    wxTextFile file( basedir + wxFileName::GetPathSeparator() + filename );
    if ( !file.Open() )
    {
        return false;
    }
    wxString line = file.GetFirstLine();
    asImp().FromString( line );
    return true;
}

/** @brief ToFile
  *
  * @todo: document this function
  */
template <class ImpType>
bool StringSerializer<ImpType>::ToFile(const wxString& filename)
{
    wxString basedir = sett().GetCachePath();
    wxTextFile file( basedir + wxFileName::GetPathSeparator() + filename );
    if ( !file.Open() )
    {
        if ( !file.Create() )
            return false;
    }
    file.Clear();
    file.AddLine( asImp().ToString() );
    return file.Write();
}


