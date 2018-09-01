/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SLCONFIG_H
#define SLCONFIG_H

#include <wx/fileconf.h>
#include <map>
#include "utils/mixins.h"
// helper macros to expand __LINE__
#define SLCONFIG__PASTE(a, b) a##b
#define SLCONFIG_PASTE(a, b) SLCONFIG__PASTE(a, b)

//! interface for adding a default value for a config key, inspired by
// the interface in springrts project
// defVal can be wxString, long, double or bool
// the description is not used for now
//
// Examples:
//
//     SLCONFIG("/test/string", "test" , "test string");
//     SLCONFIG("/test/long", 1l, "test long");
//     SLCONFIG("/test/double", 2.0, "test double");
//     SLCONFIG("/test/bool", true, "test bool");
//
// note the "l" when defining the default for long
//
// It is forbidden to define a default for the same key twice
// it will result in an assertion error
//
// if you have defined a default value, you can use ReadString(key),
// ReadLong(key), ReadDouble(key) or ReadBool(keu)
//
// Note: You can use wxConfigBase's Write mehtods to actually write
// a setting
//
// Note: You can use wxConfigBase's Read methods directly without using
// the macro, those methods will not access the defaults set with
// SLCONFIG macro
//
#define SLCONFIG(name, defVal, description) \
	static slConfigDefault SLCONFIG_PASTE(slCfgVar, __LINE__) = slConfigDefault(_T(name), defVal)


// local class to lookup and save defaults
template <class T>
class Default
{
	typedef std::map<wxString, T> DefaultMap;

public:
	Default();
	void Get(const wxString& key, T& defValue) const;
	void Set(const wxString& key, const T& defValue);

private:
	DefaultMap defaultMap;
};

class wxFileInputStream;

//! a proxy class to wxFileConfig
// it allows direct access to the Read and Write mehtods of wxConfigBase
// or to set default values using SLCONFIG() macro and ReadString(key),
// ReadLong(key), ReadDouble(key) or ReadBool(key)
class slConfig : public wxFileConfig, public SL::NonCopyable
{
public:
	slConfig(const wxString& localFilename, const wxString& globalFilename);

	static slConfig* Get();

#if wxUSE_STREAMS
	slConfig(wxInputStream& in, const wxMBConv& conv = wxConvAuto());
#endif // wxUSE_STREAMS

	//		wxString GetFilePath() const;
	void SaveFile();

	//! container for default values
	static Default<wxString>& GetDefaultsString();
	static Default<long>& GetDefaultsLong();
	static Default<double>& GetDefaultsDouble();
	static Default<bool>& GetDefaultsBool();


	//! easy to use methods to read config, IF default is set
	wxString ReadString(const wxString& key) const;
	long ReadLong(const wxString& key) const;
	double ReadDouble(const wxString& key) const;
	bool ReadBool(const wxString& key) const;

	/// list all entries subkeys of a parent group
	wxArrayString GetGroupList(const wxString& base_key);
	/// list all groups subkeys of a parent group
	wxArrayString GetEntryList(const wxString& base_key);
	/// counts all groups subkeys of a parent group
	unsigned int GetGroupCount(const wxString& base_key);

	class PathGuard
	{
		slConfig* m_config;
		const wxString m_old_path;
		DECLARE_NO_COPY_CLASS(PathGuard)
	public:
		PathGuard(slConfig* config, const wxString& new_path)
		    : m_config(config)
		    , m_old_path(config ? config->GetPath() : wxString(wxEmptyString))
		{
			if (m_config)
				m_config->SetPath(new_path);
		}

		~PathGuard()
		{
			if (m_config)
				m_config->SetPath(m_old_path);
		}
	};
	static wxString m_chosen_path;

#ifdef __WXMSW__
protected:
	//! on windows writing longs is broken so we redirect this to string
	bool DoWriteLong(const wxString& key, long lValue);
#endif

private:
	static slConfig* Create();
};


//! Helper class to define defaults for config keys
class slConfigDefault
{
public:
	slConfigDefault(const wxString& key, const wxString& defVal);
	slConfigDefault(const wxString& key, const char* defVal);
	slConfigDefault(const wxString& key, const long& defVal);
	slConfigDefault(const wxString& key, const double& defVal);
	slConfigDefault(const wxString& key, const bool& defVal);
};

slConfig& cfg();

#endif // SLCONFIG_H
