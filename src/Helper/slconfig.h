#ifndef SLCONFIG_H
#define SLCONFIG_H

#include <wx/fileconf.h>

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

	protected:
		#ifdef __WXMSW__
			//! on windows writing longs is broken so we redirect this to string
			bool DoWriteLong(const wxString& key, long lValue);
		#endif

		void SetupGlobalconfig();

		wxFileConfig* m_global_config;
};

#endif // SLCONFIG_H
