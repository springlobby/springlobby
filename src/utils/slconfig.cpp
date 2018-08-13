/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "slconfig.h"

#include <wx/filename.h>
#include <wx/log.h>
#include <wx/wfstream.h>

#include "log.h"
#include "utils/conversion.h"
#include "utils/platform.h"
#include "utils/slpaths.h"

wxString slConfig::m_chosen_path = wxEmptyString;


slConfig::slConfig(const wxString& strLocal, const wxString& strGlobal)
    : wxFileConfig(wxEmptyString, wxEmptyString, strLocal, strGlobal, wxCONFIG_USE_LOCAL_FILE, wxConvUTF8)
{
	// nop
}

#if wxUSE_STREAMS
slConfig::slConfig(wxInputStream& in, const wxMBConv& conv)
    : wxFileConfig(in, conv)
{
	// nop
}
#endif // wxUSE_STREAMS

//! overwrite default create of wxConfigBase
slConfig* slConfig::Create()
{
	m_chosen_path = TowxString(SlPaths::GetConfigPath());

	// make sure config file & dir is created/writeable when not exists
	wxString configDir;
	wxFileName::SplitPath(m_chosen_path, &configDir, NULL, NULL);
	if (!SlPaths::mkDir(STD_STRING((configDir)))) {
		wxLogError(_T("unable to create config dir"));
		exit(-1);
	}
	if (!wxFileName::FileExists(m_chosen_path)) {
		wxFileOutputStream outstream(m_chosen_path);
		if (!outstream.IsOk()) {
			wxLogError(_T("unable to create config file"));
			exit(-1);
		}
	}


	wxFileInputStream instream(slConfig::m_chosen_path);
	if (!instream.IsOk()) {
		wxLogError(_T( "unable to use config file" ));
		exit(-1);
	}

	slConfig* config = new slConfig(m_chosen_path, wxEmptyString);
	config->SetRecordDefaults(true);
	return config;
}

//! create slConfig on first access
slConfig* slConfig::Get()
{
	static slConfig* cfg = NULL;
	if (cfg == NULL) {
		cfg = Create();
	}
	return cfg;
}

void slConfig::SaveFile()
{
	Flush();
	wxFileOutputStream outstream(slConfig::m_chosen_path);

	if (!outstream.IsOk()) {
		// TODO: error handling
		wxLogError(_T("can not save config: %s"), slConfig::m_chosen_path.c_str());
	}
	Save(outstream);
	wxLogMessage(_T("config file saved: %s"), slConfig::m_chosen_path.c_str());
}

/*
wxString slConfig::GetFilePath() const {
	return slConfig::m_chosen_path;
}
*/

#ifdef __WXMSW__
bool slConfig::DoWriteLong(const wxString& key, long lValue)
{
	return wxFileConfig::DoWriteString(key, wxString::Format("%d", lValue));
}
#endif

Default<wxString>& slConfig::GetDefaultsString()
{
	static Default<wxString> defaultString;
	return defaultString;
}

Default<long>& slConfig::GetDefaultsLong()
{
	static Default<long> defaultLong;
	return defaultLong;
}

Default<double>& slConfig::GetDefaultsDouble()
{
	static Default<double> defaultDouble;
	return defaultDouble;
}

Default<bool>& slConfig::GetDefaultsBool()
{
	static Default<bool> defaultBool;
	return defaultBool;
}


template <class T>
Default<T>::Default()
{
	defaultMap = DefaultMap();
}

//! get default value, return true if default value found
template <class T>
void Default<T>::Get(const wxString& key, T& defValue) const
{
	auto it = defaultMap.find(key);
	ASSERT_LOGIC(
	    it != defaultMap.end(),
	    stdprintf("no default set for: %s", STD_STRING(key).c_str()));
	defValue = it->second;
};

//! return true if default value set, won't overwrite if already exists
template <class T>
void Default<T>::Set(const wxString& key, const T& defValue)
{
	auto ret = defaultMap.insert(std::pair<const wxString, const T>(key, defValue));
	ASSERT_LOGIC(
	    ret.second,
	    stdprintf("default already exists: %s", STD_STRING(key).c_str()));
}


wxString slConfig::ReadString(const wxString& key) const
{
	// read a value with preset default
	// this will return a valid value or fail loudly
	// 1. read the default value to make sure it is set
	// 2. read current value, if set
	wxString value;
	GetDefaultsString().Get(key, value);

	// Read() without default (third parameter) will change value if
	// set, or leave it alone
	wxFileConfig::Read(key, &value);

	// we can assert that value will be valid or the program terminated
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, value);
	}
	return value;
}

long slConfig::ReadLong(const wxString& key) const
{
	long value;
	GetDefaultsLong().Get(key, value);
	wxFileConfig::Read(key, &value);
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, value);
	}
	return value;
}

double slConfig::ReadDouble(const wxString& key) const
{
	double value;
	GetDefaultsDouble().Get(key, value);
	wxFileConfig::Read(key, &value);
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, value);
	}
	return value;
}

bool slConfig::ReadBool(const wxString& key) const
{
	bool value;
	GetDefaultsBool().Get(key, value);
	wxFileConfig::Read(key, &value);
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, value);
	}
	return value;
}

wxArrayString slConfig::GetGroupList(const wxString& base_key)
{
	slConfig::PathGuard pathGuard(this, base_key);
	wxString groupname;
	long dummy;
	wxArrayString ret;
	bool groupexist = GetFirstGroup(groupname, dummy);
	while (groupexist) {
		ret.Add(groupname);
		groupexist = GetNextGroup(groupname, dummy);
	}
	return ret;
}

wxArrayString slConfig::GetEntryList(const wxString& base_key)
{
	slConfig::PathGuard pathGuard(this, base_key);
	wxString entryname;
	long dummy;
	wxArrayString ret;
	bool entryexist = GetFirstEntry(entryname, dummy);
	while (entryexist) {
		ret.Add(entryname);
		entryexist = GetNextEntry(entryname, dummy);
	}
	return ret;
}

unsigned int slConfig::GetGroupCount(const wxString& base_key)
{
	slConfig::PathGuard pathGuard(this, base_key);
	return GetNumberOfGroups(false);
}


slConfigDefault::slConfigDefault(const wxString& key, const wxString& defVal)
{
	slConfig::GetDefaultsString().Set(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const char* defVal)
{
	slConfig::GetDefaultsString().Set(key, wxString::FromUTF8(defVal));
}

slConfigDefault::slConfigDefault(const wxString& key, const long& defVal)
{
	slConfig::GetDefaultsLong().Set(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const double& defVal)
{
	slConfig::GetDefaultsDouble().Set(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const bool& defVal)
{
	slConfig::GetDefaultsBool().Set(key, defVal);
}

slConfig& cfg()
{
	return *slConfig::Get();
}
