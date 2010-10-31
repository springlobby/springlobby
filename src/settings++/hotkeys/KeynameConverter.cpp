#include "KeynameConverter.h"

#include <sstream>
#include "../../wxkeybinder/keybinder.h"

KeynameConverter::KeyMap		KeynameConverter::m_spring2keybinder;
KeynameConverter::KeyMap		KeynameConverter::m_keybinder2spring;

void KeynameConverter::initialize()
{
	KeynameConverter::addMapping( wxT("backspace"), wxT("BACK") );
	KeynameConverter::addMapping( wxT("esc"), wxT("ESCAPE") );
	KeynameConverter::addMapping( wxT("enter"), wxT("RETURN") );

	KeynameConverter::addMapping( wxT("numpad*"), wxT("* (numpad)") );
	KeynameConverter::addMapping( wxT("numpad+"), wxT("+ (numpad)") );
	KeynameConverter::addMapping( wxT("numpad-"), wxT("- (numpad)") );
	KeynameConverter::addMapping( wxT("numpad."), wxT(". (numpad)") );
	KeynameConverter::addMapping( wxT("numpad/"), wxT("/ (numpad)") );
	KeynameConverter::addMapping( wxT("numpad0"), wxT("0 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad1"), wxT("1 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad2"), wxT("2 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad3"), wxT("3 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad4"), wxT("4 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad5"), wxT("5 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad6"), wxT("6 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad7"), wxT("7 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad8"), wxT("8 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad9"), wxT("9 (numpad)") );
	KeynameConverter::addMapping( wxT("numpad="), wxT("= (numpad)") );
	KeynameConverter::addMapping( wxT("numpad_enter"), wxT("ENTER (numpad)") );
}

void KeynameConverter::addMapping( const wxString& springKey, const wxString& keybinderKey )
{
	KeynameConverter::m_keybinder2spring[keybinderKey] = springKey;
	KeynameConverter::m_spring2keybinder[springKey] = keybinderKey;
}

wxString KeynameConverter::discardModifier( const wxString& keystring )
{
	wxString result;
	if ( keystring.EndsWith(wxT("+")) )	//handle stuff like numpad+ or ctrl++
	{
		wxString tmp = keystring;
		result = tmp.RemoveLast().AfterLast(wxT('+')) + wxT('+');
	}
	else if ( keystring.StartsWith(wxT("+")) )	//handle stuff like "+ (numpad)"
	{
		result = keystring;
	}
	else 
	{
		size_t lastAdd = keystring.find_last_of(wxT('+'));
		if ( ( lastAdd != keystring.npos ) && ( keystring.GetChar(lastAdd - 1) == wxT('+') ) )
		{
			assert( (lastAdd > 0) && "character '+' found in unexcepted location!" );
			result = keystring.substr( lastAdd );
		}
		else
		{
			result = keystring.AfterLast(wxT('+'));
		}
	}
	return result;
}

wxString KeynameConverter::convertHexValueToKey( const wxString& hexStr )
{
	wxString res = hexStr;
	if ( hexStr.StartsWith( wxT("0x") ) )
	{
		long value = 0;
		if ( hexStr.ToLong( &value, 16 ) )
		{
			res = wxChar( value );
			res.MakeLower();
		}
	}
	return res;
}

wxString KeynameConverter::normalizeSpringKey( const wxString& springKey )
{
	//get modifiers
	KeynameConverter::ModifierList modifiers = stringToKeyModifier( springKey );

	wxString key = KeynameConverter::discardModifier( springKey );
	key.MakeLower();

	if ( key == wxT("escape") )
	{
		key = wxT("esc");
	}

	return KeynameConverter::modifier2String( modifiers ) + key;
}

wxString KeynameConverter::spring2wxKeybinder( const wxString& keystring, bool reverse )
{
	//get modifiers
	KeynameConverter::ModifierList modifiers = stringToKeyModifier( keystring );

	//set conversion direction
	const KeynameConverter::KeyMap* pCurKeyMap = &KeynameConverter::m_spring2keybinder;
	if ( reverse )
	{
		pCurKeyMap = &KeynameConverter::m_keybinder2spring;
	}

	//find the correct key
	wxString key = KeynameConverter::discardModifier( keystring );

	wxString kbKey;
	if ( pCurKeyMap->find( key ) != pCurKeyMap->end() )
	{
		kbKey = pCurKeyMap->find( key )->second;
	}
	else if ( key.StartsWith( wxT("0x") ) )
	{
		kbKey = key;
	}
	else
	{
#ifdef __WXMSW__
		if ( key.size() == 1 )
		{
			if ( reverse )
				kbKey = wxMswKeyConverter::ConvertLocalToUs( key[0] );
			else
				kbKey = wxMswKeyConverter::ConvertUsToLocal( key[0] );
		}
		else
		{
#endif
			kbKey = key;
#ifdef __WXMSW__
		}
#endif

		if ( reverse )
			kbKey.MakeLower();
		else
			kbKey.MakeUpper();
	}

	return KeynameConverter::modifier2String( modifiers ) + kbKey;
}

wxString KeynameConverter::modifier2String( const KeynameConverter::ModifierList& mod )
{
	wxString modString;

	if ( mod.find( ANY ) != mod.end() )
	{
		modString += wxT("Any+");
	}
	else
	{
		if ( mod.find( CTRL ) != mod.end() )
		{
			modString += wxT("Ctrl+");
		}
		if ( mod.find( SHIFT ) != mod.end() )
		{
			modString += wxT("Shift+");
		}
		if ( mod.find( ALT ) != mod.end() )
		{
			modString += wxT("Alt+");
		}
		if ( mod.find( META ) != mod.end() )
		{
			modString += wxT("Meta+");
		}
	}

	return modString;
}

KeynameConverter::ModifierList KeynameConverter::stringToKeyModifier(const wxString &keyModifier)
{
	ModifierList modifiers;

	// this search must be case-insensitive
	const wxString str = keyModifier.Upper();

	if (str.Contains(wxT("ALT+")))
	{
		modifiers.insert( ALT );
	}

	if (str.Contains(wxT("CTRL+")))
	{
		modifiers.insert( CTRL );
	}

	if (str.Contains(wxT("SHIFT+")))
	{
		modifiers.insert( SHIFT );
	}

	if (str.Contains(wxT("ANY+")))
	{
		modifiers.insert( ANY );
	}

	if (str.Contains(wxT("META+")))
	{
		modifiers.insert( META );
	}

	return modifiers;
}
