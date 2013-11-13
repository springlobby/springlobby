#ifndef SLCONFIG_H
#define SLCONFIG_H

#include <wx/fileconf.h>
#include <map>
#include "../utils/mixins.hh"

class wxFileInputStream;

typedef wxFileConfig
	slConfigBaseType;

//! a proxy class to wxFileConfig
class slConfig : public slConfigBaseType, public SL::NonCopyable
{
	public:
		slConfig ( const wxString& appName = wxEmptyString,
				   const wxString& vendorName = wxEmptyString,
				   const wxString& localFilename = wxEmptyString,
				   const wxString& globalFilename = wxEmptyString,
				   long style = wxCONFIG_USE_LOCAL_FILE,
				   const wxMBConv& conv = wxConvAuto() );

	#if wxUSE_STREAMS
		slConfig ( wxInputStream& in, const wxMBConv& conv = wxConvAuto() );
	#endif // wxUSE_STREAMS

		wxString Read(const wxString& key, const wxString& defVal = wxEmptyString ) const;
		long Read(const wxString& key, long defaultVal) const;

		bool Read(const wxString& key, wxString* str) const;
		bool Read(const wxString& key, long* l) const;
		bool Read(const wxString& key, double* d) const;
		bool Read(const wxString& key, bool* b) const;

		// we don't override alls Read methods, so reintroduce them (see
		// 'name hiding')
		using slConfigBaseType::Read;

		void SetPath(const wxString& strPath);

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
};

#endif // SLCONFIG_H
