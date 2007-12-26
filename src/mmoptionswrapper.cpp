#include "mmoptionswrapper.h"

#include "springunitsynclib.h"
#include "utils.h"
#include "settings++/custom_dialogs.h"

mmOptionsWrapper::mmOptionsWrapper()
{
	loadModOptions();
}

mmOptionsWrapper::~mmOptionsWrapper()
{
}

bool mmOptionsWrapper::loadMapOptions(wxString mapname)
{
//	int count = 0;
//		try 
//		{
//			count = susynclib()->GetMapOptionCount(mapname);
//			
//		}
//		catch(...) 
//		{
//			wxLogError(_T("Could not load map Options"));
//			return false;
//		}
//		
//		for (int i = 0; i < count; ++i)
//		{
//			
//			try
//			{
//				switch (susynclib()->GetOptionType(i))
//				{
//				case IS_FLOAT_OPTION:
//					m_map_options_float.push_back(mmOptionFloat(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
//							susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
//							susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i)));
//					break;
//				case IS_BOOL_OPTION:
//					m_map_options_bool.push_back(mmOptionBool(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
//							susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i)));
//					break;
//				case IS_STRING_OPTION:
//					m_map_options_string.push_back(mmOptionString(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
//							susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i)));
//					break;
//				}
//
//			}
//			catch(...)
//			{
//				//TODO add number
//				wxLogWarning(_T("Cannot load map-option number"));
//			}
//
//		}
		return true;
}

bool mmOptionsWrapper::loadModOptions()
{
	int count = 0;
	try 
	{
		count = susynclib()->GetModOptionCount();
	}
	catch(...) 
	{
		wxLogError(_T("Could not load mod Options"));
		return false;
	}
	mmOptionFloat* fg;
	mmOptionBool* bg;
	for (int i = 0; i < count; ++i)
	{
		try
		{
			int ic = susynclib()->GetOptionType(i);
			wxString a; a <<i <<_T(" : ") << ic;
			customMessageBox(0,a,_T("index : type"));
			switch (ic)
			{
			case IS_FLOAT_OPTION:
				fg = new mmOptionFloat(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
						susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
						susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i));
				m_mod_options_float.push_back(*fg);
				break;
			case IS_BOOL_OPTION:
				bg = new mmOptionBool(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
						susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i));
				m_mod_options_bool.push_back(*bg);
				break;
			case IS_STRING_OPTION:
				m_mod_options_string.push_back(mmOptionString(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
						susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i)));
				break;
			}

		}
		catch(...)
		{
			//TODO add number
			wxLogWarning(_T("Cannot load mod-option number"));
		}

	}
	return true;
}

