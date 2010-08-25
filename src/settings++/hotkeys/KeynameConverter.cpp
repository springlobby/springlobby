#include "KeynameConverter.h"

#include <sstream>

KeynameConverter::KeyMap		KeynameConverter::m_spring2keybinder;
KeynameConverter::KeyMap		KeynameConverter::m_keybinder2spring;

void KeynameConverter::initialize()
{
	KeynameConverter::addMapping( "backspace", "BACK" );
	KeynameConverter::addMapping( "esc", "ESCAPE" );
	KeynameConverter::addMapping( "enter", "RETURN" );

	KeynameConverter::addMapping( "numpad*", "* (numpad)" );
	KeynameConverter::addMapping( "numpad+", "+ (numpad)" );
	KeynameConverter::addMapping( "numpad-", "- (numpad)" );
	KeynameConverter::addMapping( "numpad.", ". (numpad)" );
	KeynameConverter::addMapping( "numpad/", "/ (numpad)" );
	KeynameConverter::addMapping( "numpad0", "0 (numpad)" );
	KeynameConverter::addMapping( "numpad1", "1 (numpad)" );
	KeynameConverter::addMapping( "numpad2", "2 (numpad)" );
	KeynameConverter::addMapping( "numpad3", "3 (numpad)" );
	KeynameConverter::addMapping( "numpad4", "4 (numpad)" );
	KeynameConverter::addMapping( "numpad5", "5 (numpad)" );
	KeynameConverter::addMapping( "numpad6", "6 (numpad)" );
	KeynameConverter::addMapping( "numpad7", "7 (numpad)" );
	KeynameConverter::addMapping( "numpad8", "8 (numpad)" );
	KeynameConverter::addMapping( "numpad9", "9 (numpad)" );
	KeynameConverter::addMapping( "numpad=", "= (numpad)" );
	KeynameConverter::addMapping( "numpad_enter", "ENTER (numpad)" );
}

void KeynameConverter::addMapping( const wxString& springKey, const wxString& keybinderKey )
{
	KeynameConverter::m_keybinder2spring[keybinderKey] = springKey;
	KeynameConverter::m_spring2keybinder[springKey] = keybinderKey;
}

wxString KeynameConverter::discardModifier( const wxString& keystring )
{
	wxString result;
	if ( keystring.EndsWith("+") )	//handle stuff like numpad+ or ctrl++
	{
		wxString tmp = keystring;
		result = tmp.RemoveLast().AfterLast('+') + '+';
	}
	else if ( keystring.StartsWith("+") )	//handle stuff like "+ (numpad)"
	{
		result = keystring;
	}
	else 
	{
		size_t lastAdd = keystring.find_last_of('+');
		if ( ( lastAdd != keystring.npos ) && ( keystring.GetChar(lastAdd - 1) == '+' ) )
		{
			assert( (lastAdd > 0) && "character '+' found in unexcepted location!" );
			result = keystring.substr( lastAdd );
		}
		else
		{
			result = keystring.AfterLast('+');
		}
	}
	return result;
}

bool KeynameConverter::compareSpring2wxKeybinder( const wxString& springkey, const wxString& kbKey )
{
	KeynameConverter::ModifierList springMods = stringToKeyModifier( springkey );
	KeynameConverter::ModifierList kbMods = stringToKeyModifier( kbKey );

	//delete if existing
	springMods.erase( ANY );

	if ( springMods != kbMods )
	{
		return false;
	}

	const wxString k1 = KeynameConverter::spring2wxKeybinder( KeynameConverter::discardModifier( springkey ) );
	const wxString k2 = KeynameConverter::discardModifier( kbKey );
	if ( k1 != k2 )
	{
		return false;
	}

	return true;
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
	else if ( key.StartsWith("0x") && key.size() == 4 )
	{
		//raw key code - e.g. 0xa4
		const wxString tmp = key.substr( 2, 2 );
		std::stringstream str;
		str << std::hex << tmp.ToAscii();
		unsigned int c = 0x00;
		str >> c;
		kbKey += c;
	}
	else
	{
		if ( reverse )
		{
			kbKey = key.Lower();
		}
		else
		{
			kbKey = key.Upper();
		}
	}

	return KeynameConverter::modifier2String( modifiers, reverse ) + kbKey;
}

wxString KeynameConverter::modifier2String( const KeynameConverter::ModifierList& mod, bool addAny )
{
	wxString modString;

	bool modFound = false;
	if ( mod.find( CTRL ) != mod.end() )
	{
		modString += wxT("Ctrl+");
		modFound = true;
	}
	if ( mod.find( SHIFT ) != mod.end() )
	{
		modString += wxT("Shift+");
		modFound = true;
	}
	if ( mod.find( ALT ) != mod.end() )
	{
		modString += wxT("Alt+");
		modFound = true;
	}

	if ( !modFound && addAny )
	{
		modString = wxT("any+") + modString;
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

	return modifiers;
}