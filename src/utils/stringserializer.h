#ifndef SPRINGLOBBY_HEADERGUARD_STRINGSERIALIZER_H
#define SPRINGLOBBY_HEADERGUARD_STRINGSERIALIZER_H

#include "crtpbase.h"

class wxString;

template <class ImpType>
//! ImpType will need to provide a default constructor
class StringSerializer : protected CRTPbase<ImpType>
{
    public:
        StringSerializer();
        virtual ~StringSerializer();

        /** \brief saves the data member in string representation to filename
            \param filename will be prepeneded with current cache path
            \return true on success, false on failure
            \note file will contain a single line with the string representation
        **/
        bool ToFile( const wxString& filename );
        //! companion function, needs to be implemented in ImpType
        wxString ToString() const;

        /** \brief loads the data member in string representation to filename
            \param filename will be prepeneded with current cache path
            \return true on success, false on failure
            \note if the file contains more than one string representation/line only the first will be loaded
        **/
        bool FromFile( const wxString& filename );
        //! companion function, needs to be implemented in ImpType
        void FromString( const wxString& );

        /** \brief saves a vector of ImpType objects  to file
            \param filename will be prepeneded with current cache path
            \return true on success, false on failure
            \note file will contain N lines with each line being a string representation
        **/
        static bool VectorToFile( const std::vector<ImpType>& vector, const wxString& filename );

        /** \brief loads the data member in string representation to filename
            \param filename will be prepeneded with current cache path
            \return vector of ImpType objects, possibly empty if error occured, otherwise with NumLine max ImpType objetcs
        **/
        static std::vector<ImpType> VectorFromFile( const wxString& filename );

    protected:
        using CRTPbase<ImpType>::asImp;


};
#include "stringserializer.cpp"
#endif // SPRINGLOBBY_HEADERGUARD_STRINGSERIALIZER_H
