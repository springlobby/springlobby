#ifndef PLASMARESOURCEINFO_H
#define PLASMARESOURCEINFO_H

#include <vector>

#include "stringserializer.h"

#include <wx/string.h>
#include <wx/arrstr.h>

class PlasmaResourceInfo : public StringSerializer<PlasmaResourceInfo>
{

	public:
		PlasmaResourceInfo();

		//!file name
		wxString m_name;
		//!fn on remote
		wxString m_torrent_filename;
		//!full urls
		wxArrayString m_webseeds;
		//!list of resource names
		wxArrayString m_dependencies;
		//! will only be non-empty when actual download of torrent file succeeded
		wxString m_local_torrent_filepath;
		//! filesize in KB; a value of 0 will be interpreted as unknown
		unsigned int m_size_KB;

		enum ResourceType {
			mod = 0,
			map = 1,
			unknown = 2
		};

		ResourceType m_type;

		bool operator < ( const PlasmaResourceInfo& o ) const {
			return m_name < o.m_name;
		}

		wxString ToString() const;
		void FromString( const wxString& str );
};

#endif // PLASMARESOURCEINFO_H
