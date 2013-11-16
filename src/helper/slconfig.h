#ifndef SLCONFIG_H
#define SLCONFIG_H

#include <wx/fileconf.h>
#include <map>
#include "../utils/mixins.hh"


// local class to lookup and save defaults
template <class T>
class Default {
	typedef std::map <wxString, T> DefaultMap;
	
	public:
		Default();
		bool Get(const wxString& key, T& defValue) const;
		bool Set(const wxString& key, const T& defValue);

	private:
		DefaultMap defaultMap;
};

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

		// set default value for a config, return true if value set
		// first time, if a value is set, it will not be overwritten
		static bool setDefault(const wxString& key, const wxString& defVal);
		static bool setDefault(const wxString& key, const long& defVal);
		static bool setDefault(const wxString& key, const double& defVal);
		static bool setDefault(const wxString& key, const bool& defVal);

		static Default<wxString>& getDefaultsString();
		static Default<long>& getDefaultsLong();
		static Default<double>& getDefaultsDouble();
		static Default<bool>& getDefaultsBool();

		wxString Read(const wxString& key, const wxString& defVal = wxEmptyString ) const;
		long Read(const wxString& key, long defaultVal) const;

		bool Read(const wxString& key, wxString* str) const;
		bool Read(const wxString& key, long* l) const;
		bool Read(const wxString& key, double* d) const;
		bool Read(const wxString& key, bool* b) const;

		// we don't override alls Read methods, so reintroduce them (see
		// 'name hiding')
		using slConfigBaseType::Read;

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


//! Helper class to define defaults for config keys
class slConfigDefault {
	public:
		slConfigDefault(const wxString& key, const wxString& defVal);
		slConfigDefault(const wxString& key, const long& defVal);
		slConfigDefault(const wxString& key, const double& defVal);
		slConfigDefault(const wxString& key, const bool& defVal);
};

// helper macros to expand __LINE__
#define SLCONFIG__PASTE(a, b) a ## b
#define SLCONFIG_PASTE(a, b) SLCONFIG__PASTE(a, b)

//! interface for adding a default value for a config key, inspired by
// interface in springrts project
// defVal can be wxString, long, double or bool
// the description is not used for now
// 
// Example to crate default value true for key '/config/is/nice'
//
//     SLCONFIG("/config/is/nice", true, "short description");
//
#define SLCONFIG(name, defVal, description) \
	static slConfigDefault SLCONFIG_PASTE(slCfgVar, __LINE__)  = slConfigDefault(_T( #name ), defVal)


#endif // SLCONFIG_H

