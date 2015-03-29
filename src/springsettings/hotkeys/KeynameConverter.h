/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef _H_KEYNAME_CONVERTER
#define _H_KEYNAME_CONVERTER

#include <map>
#include <set>

#include <wx/string.h>

#include "HotkeyTypes.h"

/*
 *	This class converts spring's keystrings to wxKeybinder keystrings and back
 */
class KeynameConverter
{
public:
	enum Modifier {
		CTRL,
		ALT,
		SHIFT,
		ANY,
		META
	};
	typedef std::set<Modifier> ModifierList;

	static void initialize();

	static wxString spring2wxKeybinder(const wxString& keystring, bool reverse = false);
	static wxString normalizeSpringKey(const wxString& springKey);

	static wxString convertHexValueToKey(const wxString& hexStr);

	static wxString modifier2String(const KeynameConverter::ModifierList& mod);
	static ModifierList stringToKeyModifier(const wxString& keyModifier);
	static wxString discardModifier(const wxString& keystring);

private:
	typedef std::map<wxString, wxString> KeyMap;

	KeynameConverter();

	static wxString convert(const wxString& keystring, bool reverse = false);

	static void addMapping(const wxString& springKey, const wxString& keybinderKey);

	static KeynameConverter::KeyMap m_spring2keybinder;
	static KeynameConverter::KeyMap m_keybinder2spring;
};

#endif
