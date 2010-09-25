#ifndef SLCONFIG_H
#define SLCONFIG_H

#include <wx/fileconf.h>
#include <map>

class wxFileInputStream;

typedef wxFileConfig
	slConfigBaseType;

//! a proxy class to wxFileConfig with additional global config values replacing first tier default values
class slConfig : public slConfigBaseType
{
	public:
		slConfig ( const wxString& appName = wxEmptyString,
				   const wxString& vendorName = wxEmptyString,
				   const wxString& localFilename = wxEmptyString,
				   const wxString& globalFilename = wxEmptyString,
				   long style = wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE,
				   const wxMBConv& conv = wxConvAuto() );

	#if wxUSE_STREAMS
		slConfig ( wxInputStream& in, const wxMBConv& conv = wxConvAuto() );
	#endif // wxUSE_STREAMS

		wxString Read(const wxString& key, const wxString& defVal = wxEmptyString ) const;
		long Read(const wxString& key, long defaultVal) const;

		bool Read(const wxString& key, wxString* str) const;
		bool Read(const wxString& key, wxString* str, const wxString& defaultVal) const;

		bool Read(const wxString& key, long* l) const;
		bool Read(const wxString& key, long* l, long defaultVal) const;

		bool Read(const wxString& key, double* d) const;
		bool Read(const wxString& key, double* d, double defaultVal) const;

		bool Read(const wxString& key, bool* b) const;
		bool Read(const wxString& key, bool* d, bool defaultVal) const;

		//! this could be a seriously bad idea if it's used internally to determine if default is used or some such
//		bool Exists( const wxString& strName) const;

		bool HasSection( const wxString& strName) const;

		void SetPath(const wxString& strPath);
		bool GetFirstEntry(wxString& str, long& index) const;
		bool GetNextEntry(wxString& str, long& index) const;
		bool GetFirstGroup(wxString& str, long& index) const;
		bool GetNextGroup(wxString& str, long& index) const;
		size_t GetNumberOfGroups(bool bRecursive = false) const;
		size_t GetNumberOfEntries(bool bRecursive = false) const;

		class PathGuard {
				slConfig* m_config;
				const wxString m_old_path;
				DECLARE_NO_COPY_CLASS(PathGuard)
			public:
				PathGuard( slConfig* config, const wxString& new_path )
					:m_config( config ),
					m_old_path( m_config ? m_config->GetPath() : _T("") )
				{
					if ( m_config )
						m_config->SetPath( new_path );
				}

				~PathGuard()
				{
					if ( m_config )
						m_config->SetPath( m_old_path );
				}
		};

	protected:
		#ifdef __WXMSW__
			//! on windows writing longs is broken so we redirect this to string
			bool DoWriteLong(const wxString& key, long lValue);
		#endif

		void SetupGlobalconfig();

		wxFileConfig* m_global_config;

		typedef std::map<long,long>
			ForwardsType;
		mutable ForwardsType m_enumerationId_forwards_entries;
		mutable ForwardsType m_enumerationId_forwards_groups;
};

#endif // SLCONFIG_H
