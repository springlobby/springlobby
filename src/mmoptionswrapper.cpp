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
	int count = 0;
		try 
		{
			count = susynclib()->GetMapOptionCount(mapname);
			
		}
		catch(...) 
		{
			wxLogError(_T("Could not load map Options"));
			return false;
		}
		
		for (int i = 0; i < count; ++i)
		{	
			try
			{
				switch (susynclib()->GetOptionType(i))
				{
				case IS_FLOAT_OPTION:
					m_map_options_float.push_back(mmOptionFloat(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
							susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i)));
					break;
				case IS_BOOL_OPTION:
					m_map_options_bool.push_back(mmOptionBool(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i)));
					break;
				case IS_STRING_OPTION:
					m_map_options_string.push_back(mmOptionString(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i)));
					break;
				}

			}
			catch(...)
			{
				wxString a = _T("Cannot load map-option #");
				wxLogWarning( a << i );
			}

		}
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
	mmOptionList* templist;
	for (int i = 0; i < count; ++i)
	{
		try
		{
			switch (susynclib()->GetOptionType(i))
			{
			case IS_FLOAT_OPTION:
				m_mod_options_float.push_back(mmOptionFloat(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
						susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
						susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i)));
				break;
			case IS_BOOL_OPTION:
				m_mod_options_bool.push_back(mmOptionBool(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
						susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i)));
				break;
			case IS_STRING_OPTION:
				m_mod_options_string.push_back(mmOptionString(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
						susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i)));
				break;
			case IS_LIST_OPTION:
				 templist = new mmOptionList(susynclib()->GetOptionName(i),susynclib()->GetOptionKey(i),
						susynclib()->GetOptionDesc(i),susynclib()->GetOptionListDef(i));
				 int listitemcount = susynclib()->GetOptionListCount(i); 
				 for (int j = 0; j < listitemcount; ++j)
				 {
					 templist->addItem(susynclib()->GetOptionListItemKey(i,j),susynclib()->GetOptionListItemName(i,j),
							 								susynclib()->GetOptionListItemDesc(i,j));
				 }
				 m_mod_options_list.push_back(*templist);
			}

		}
		catch(...)
		{
			wxString a = _T("Cannot load mod-option #");
			wxLogWarning( a << i );
		}

	}
	return true;
}

