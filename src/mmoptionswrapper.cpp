#include "mmoptionswrapper.h"

#include "springunitsynclib.h"
#include "utils.h"
#include "settings++/custom_dialogs.h"

mmOptionsWrapper::mmOptionsWrapper()
{
	for (int i = 0; i < optionCategoriesCount; ++i)
	{
		m_boolMaps[i] 	= new optionMapBool;
		m_floatMaps[i]	= new optionMapFloat;
		m_stringMaps[i] = new optionMapString;
		m_listMaps[i]	= new optionMapList;

	}
}

mmOptionsWrapper::~mmOptionsWrapper()
{
}

bool mmOptionsWrapper::loadMapOptions(wxString mapname)
{
	return loadOptions(MapOption,mapname);
}

bool mmOptionsWrapper::loadOptions(GameOption modmapFlag,wxString mapname)
{
	int count = 0;
	wxString singleError;
	switch (modmapFlag)
	{
		case MapOption:
			try
			{
				count = susynclib()->GetMapOptionCount(mapname);

			}
			catch(...)
			{
				wxLogError(_T("Could not load map Options"));
				return false;
			}
			singleError = _T("Cannot load map-option #");
			break;
		case ModOption:
			try
			{
				count = susynclib()->GetModOptionCount();
			}
			catch(...)
			{
				wxLogError(_T("Could not load mod Options"));
				return false;
			}
			singleError = _T("Cannot load mod-option #");
			break;
	}

	mmOptionList* templist;
	for (int i = 0; i < count; ++i)
	{
		wxString key = susynclib()->GetOptionKey(i);
		int* dummy = new int(0);
		if (keyExists(key,modmapFlag,true,dummy))
		{
			return false;
		}
		else
		{
			try
			{
				switch (susynclib()->GetOptionType(i))
				{
				case IS_FLOAT_OPTION:
					(*m_floatMaps[modmapFlag])[key] = mmOptionFloat(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
							susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i));
					break;
				case IS_BOOL_OPTION:
					(*m_boolMaps[modmapFlag])[key] = mmOptionBool(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i));
					break;
				case IS_STRING_OPTION:
					(*m_stringMaps[modmapFlag])[key] = mmOptionString(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i));
					break;
				case IS_LIST_OPTION:
					 templist = new mmOptionList(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionListDef(i));
					 int listitemcount = susynclib()->GetOptionListCount(i);
					 for (int j = 0; j < listitemcount; ++j)
					 {
						 templist->addItem(susynclib()->GetOptionListItemKey(i,j),susynclib()->GetOptionListItemName(i,j),
								 								susynclib()->GetOptionListItemDesc(i,j));
					 }
					 (*m_listMaps[modmapFlag])[key] = (*templist);
				}

			}
			catch(...)
			{
				wxLogWarning( singleError << i );
				return false;
			}
		}
	}
	return true;
}

bool mmOptionsWrapper::keyExists(wxString key, GameOption modmapFlag, bool showError, int* optType)
{
	wxString duplicateKeyError = _T("Please contact the mod's author and tell him\n"
										"to use unique keys in his ModOptions.lua");
	bool exists = false;
	*optType = IS_UNDEFINED_OPTION;
	if ( (*m_listMaps[modmapFlag]).find(key)!=  (*m_listMaps[modmapFlag]).end())
	{
		*optType = IS_LIST_OPTION;
		exists = true;
	}
	else if ( (*m_stringMaps[modmapFlag]).find(key)!=  (*m_stringMaps[modmapFlag]).end())
	{
		*optType = IS_STRING_OPTION;
		exists = true;
	}
	else if ( (*m_boolMaps[modmapFlag]).find(key)!=  (*m_boolMaps[modmapFlag]).end())
	{
		*optType = IS_BOOL_OPTION;
		exists = true;
	}
	else if ( (*m_floatMaps[modmapFlag]).find(key)!=  (*m_floatMaps[modmapFlag]).end())
	{
		*optType = IS_FLOAT_OPTION;
		exists = true;
	}

	if (exists && showError)
	{
		customMessageBox(SL_MAIN_ICON,duplicateKeyError,_T("Mod/map option error"),wxOK);
		wxLogWarning(_T("duplicate key in mapmodoptions"));
		return true;
	}
	else
		return false;
}

bool  mmOptionsWrapper::setOptions(wxStringPairVec* options, GameOption modmapFlag)
{
	for (wxStringPairVec::iterator it = options->begin(); it != options->end(); ++it)
	{
		wxString key = it->first;
		wxString value = it->second;

		//we don't want to add a key that doesn't already exists
		int* optType = new int(IS_UNDEFINED_OPTION);
		if(!keyExists(key,modmapFlag,false,optType))
			return false;
		else
		{
			switch (*optType)
			{
			case IS_FLOAT_OPTION :
			{
				//test if min < val < max
				double* d_val = new double;
				bool d_conv_ok = value.ToDouble(d_val);
				float fl_value = float(*d_val);
				if( !d_conv_ok || fl_value < (*m_floatMaps[modmapFlag])[key].min || fl_value > (*m_floatMaps[modmapFlag])[key].max )
				{
					wxLogWarning(_T("recieved number option exceeds boundaries"));
					return false;
				}
				else
					(*m_floatMaps[modmapFlag])[key].value = fl_value;
				break;
			}
			case IS_BOOL_OPTION :
			{
				long* l_val = new long;
				bool l_conv_ok = value.ToLong(l_val);
				if( !l_conv_ok || ( *l_val != 1 && *l_val != 0 ) )
				{
					wxLogWarning(_T("recieved bool option that is neither 0 or 1"));
					return false;
				}
				else
					(*m_boolMaps[modmapFlag])[key].value = bool(*l_val);
				break;
			}
			case IS_STRING_OPTION :
			{
				// test if maxlength isn't exceeded
				if ( int(value.Len())> (*m_stringMaps[modmapFlag])[key].max_len )
				{
					wxLogWarning(_T("recieved string option exceeds max_len"));
					return false;
				}
				else
					(*m_stringMaps[modmapFlag])[key].value = value;
				break;
			}
			case IS_LIST_OPTION :
			{
				// test if valid value, aka is in list
				 int listitemcount = (*m_listMaps[modmapFlag])[key].listitems.size();
				 bool valid_string = false;
				 for (int j = 0; j < listitemcount; ++j)
				 {
					 if ( (*m_listMaps[modmapFlag])[key].listitems[j].key == value)
					 {
						 valid_string = true;
						 break;
					 }
				 }

				 if (valid_string)
					 (*m_listMaps[modmapFlag])[key].key = value;
				 else
				 {
					 wxLogWarning(_T("recieved list option is not valid"));
					 return false;
				 }
				break;
			}
			}
		}
	}
	return true;
}

void  mmOptionsWrapper::getOptions(wxStringPairVec* list, GameOption modmapFlag)
{
	for (optionMapBoolIter it = m_boolMaps[modmapFlag]->begin(); it != m_boolMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringPair( (*it).first, wxString::Format(_T("%d"),(*it).second.value) ) );
	}

	for (optionMapStringIter it = m_stringMaps[modmapFlag]->begin(); it != m_stringMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringPair( (*it).first, (*it).second.value) );
	}

	for (optionMapFloatIter it = m_floatMaps[modmapFlag]->begin(); it != m_floatMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringPair( (*it).first, wxString::Format(_T("%f"),(*it).second.value) ) );
	}

	for (optionMapListIter it = m_listMaps[modmapFlag]->begin(); it != m_listMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringPair( (*it).first, (*it).second.value) );
	}
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value,GameOption modmapFlag)
{
	wxStringPairVec temp;
	temp.push_back( wxStringPair(key,value) );
	return setOptions(&temp,modmapFlag);
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value)
{
	int* dummy = new int(0);
	if (keyExists(key,ModOption,false,dummy))
		return setSingleOption(key,value,ModOption);
	else if (keyExists(key,MapOption,false,dummy))
		return setSingleOption(key,value,MapOption);
	else
		return false;
}

wxString mmOptionsWrapper::getSingleValue(wxString key)
{
//	int* optType = new int(0);
//	if (keyExists(key,ModOption,false,optType))	
//	{
//		switch (*optType)
//		{
//		
//			default:
//				return wxEmptyString;
//		}
//	}
//	else if (keyExists(key,MapOption,false,optType))
//	{
//		switch (*optType)
//		{
//		
//			default:
//				return wxEmptyString;
//		}
//	}
	
	return wxEmptyString;
}

bool mmOptionsWrapper::reloadMapOptions(wxString mapname)
{
	m_boolMaps[MapOption] 	= new optionMapBool;
	m_floatMaps[MapOption]	= new optionMapFloat;
	m_stringMaps[MapOption] = new optionMapString;
	m_listMaps[MapOption]	= new optionMapList;
	
	return loadMapOptions(mapname);
}
