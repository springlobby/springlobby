#include "mmoptionswrapper.h"

#include "springunitsynclib.h"
#include "utils.h"
#include "settings++/custom_dialogs.h"
#include <stdexcept>
mmOptionsWrapper::mmOptionsWrapper()
{
	unLoadOptions();
}

void mmOptionsWrapper::unLoadOptions()
{
	for (GameOption i = 0; i < optionCategoriesCount; ++i)
	{
		unLoadOptions( i );
	}
}

void mmOptionsWrapper::unLoadOptions(GameOption i)
{
	m_boolMaps[i] 	= new optionMapBool;
	m_floatMaps[i]	= new optionMapFloat;
	m_stringMaps[i] = new optionMapString;
	m_listMaps[i]	= new optionMapList;
}

mmOptionsWrapper::~mmOptionsWrapper()
{
}

bool mmOptionsWrapper::loadMapOptions(wxString mapname)
{
	return loadOptions(MapOption,mapname);
}

OptionType mmOptionsWrapper::GetSingleOptionType (wxString key)
{
	OptionType* type = new OptionType(opt_undefined);
	for ( GameOption g = 0; g < optionCategoriesCount; g++ )
	{
		if (keyExists(key,g,false,type))
			return *type;
	}
	return opt_undefined;
}


bool mmOptionsWrapper::loadOptions(GameOption modmapFlag,wxString mapname)
{
	int count = 0;
	wxString singleError;
	unLoadOptions(modmapFlag);
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
		OptionType* dummy = new OptionType(opt_undefined);
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
				case opt_float:
					(*m_floatMaps[modmapFlag])[key] = mmOptionFloat(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
							susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i));
					break;
				case opt_bool:
					(*m_boolMaps[modmapFlag])[key] = mmOptionBool(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i));
					break;
				case opt_string:
					(*m_stringMaps[modmapFlag])[key] = mmOptionString(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i));
					break;
				case opt_list:
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

bool mmOptionsWrapper::keyExists(wxString key, GameOption modmapFlag, bool showError, OptionType* optType)
{
	wxString duplicateKeyError = _T("Please contact the mod's author and tell him\n"
										"to use unique keys in his ModOptions.lua");
	bool exists = false;
	*optType = opt_undefined;
	if ( modmapFlag < ModOption || modmapFlag > LastOption -1 )
		return false;//, wxString::Format(_T("%d"),modmapFlag));
	if ( m_listMaps[modmapFlag]->find(key) !=  m_listMaps[modmapFlag]->end())
	{
		*optType = opt_list;
		exists = true;
	}
	else if ( m_stringMaps[modmapFlag]->find(key) !=  m_stringMaps[modmapFlag]->end())
	{
		*optType = opt_string;
		exists = true;
	}
	else if ( m_boolMaps[modmapFlag]->find(key) !=  m_boolMaps[modmapFlag]->end())
	{
		*optType = opt_bool;
		exists = true;
	}
	else if ( m_floatMaps[modmapFlag]->find(key)!=  m_floatMaps[modmapFlag]->end())
	{
		*optType = opt_float;
		exists = true;
	}

	if (exists && showError)
	{
		customMessageBox(SL_MAIN_ICON,duplicateKeyError,_T("Mod/map option error"),wxOK);
		wxLogWarning(_T("duplicate key in mapmodoptions"));
		return false;
	}
	else if ( exists && !showError )
	{
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
		OptionType* optType = new OptionType(opt_undefined);
		if(!keyExists(key,modmapFlag,false,optType))
			return false;
		else
		{
			if ( !setSingleOptionTypeSwitch( key, value, modmapFlag,  *optType) )
				return false;
		}
	}
	return true;
}

void  mmOptionsWrapper::getOptions(wxStringTripleVec* list, GameOption modmapFlag)
{
	for (optionMapBoolIter it = m_boolMaps[modmapFlag]->begin(); it != m_boolMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name , wxString::Format(_T("%d"),(*it).second.value) ) ) );
	}

	for (optionMapStringIter it = m_stringMaps[modmapFlag]->begin(); it != m_stringMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, (*it).second.value) ) );
	}

	for (optionMapFloatIter it = m_floatMaps[modmapFlag]->begin(); it != m_floatMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, wxString::Format(_T("%f"),(*it).second.value) ) ) );
	}

	for (optionMapListIter it = m_listMaps[modmapFlag]->begin(); it != m_listMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, (*it).second.value) ) );
	}
}

void mmOptionsWrapper::getOptionsMap(wxStringMap* map, GameOption modmapFlag)
{
	for (optionMapBoolIter it = m_boolMaps[modmapFlag]->begin(); it != m_boolMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] =  wxString::Format(_T("%d"),(*it).second.value);
	}

	for (optionMapStringIter it = m_stringMaps[modmapFlag]->begin(); it != m_stringMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] = (*it).second.value;
	}

	for (optionMapFloatIter it = m_floatMaps[modmapFlag]->begin(); it != m_floatMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] = wxString::Format(_T("%f"),(*it).second.value);
	}

	for (optionMapListIter it = m_listMaps[modmapFlag]->begin(); it != m_listMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] = (*it).second.value;
	}
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value,GameOption modmapFlag)
{
	OptionType* optType = new OptionType(opt_undefined);
	keyExists( key, modmapFlag, false, optType );
	return setSingleOptionTypeSwitch(key,value,modmapFlag,*optType);
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value)
{
	OptionType* optType = new OptionType(opt_undefined);
	if (keyExists(key,ModOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,ModOption,*optType);
	else if (keyExists(key,MapOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,MapOption,*optType);
	else
		return false;
}

wxString mmOptionsWrapper::getSingleValue(wxString key)
{
	for ( GameOption g = 0; g < optionCategoriesCount; g++ )
	{
		wxString tmp = getSingleValue(key,g);
		if (tmp != wxEmptyString)
			return tmp;
	}
	return wxEmptyString;
}

wxString mmOptionsWrapper::getSingleValue(wxString key, GameOption modmapFlag)
{
	OptionType* optType = new OptionType(opt_undefined);

	if ( keyExists(key,modmapFlag,false,optType) )
	{
		switch (*optType)
		{
		case opt_float:
			return wxString::Format( _T("%f"),(*m_floatMaps[modmapFlag])[key].value );
		case opt_bool:
			return wxString::Format(_T("%d"), (*m_boolMaps[modmapFlag])[key].value );
		case opt_string:
			return (*m_stringMaps[modmapFlag])[key].value;
		case opt_list:
			return (*m_listMaps[modmapFlag])[key].value;
		}
	}

	return wxEmptyString;
}

bool  mmOptionsWrapper::setSingleOptionTypeSwitch(wxString key, wxString value, GameOption modmapFlag, OptionType optType)
{
	switch (optType)
	{
		case opt_float :
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
		case opt_bool :
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
		case opt_string :
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
		case opt_list :
		{
			// test if valid value, aka is in list
			int listitemcount = (*m_listMaps[modmapFlag])[key].listitems.size();
			bool valid_string = false;
			wxString newVal;
			for (int j = 0; j < listitemcount; ++j)
			{
				if ( (*m_listMaps[modmapFlag])[key].listitems[j].name == value)
				{
					valid_string = true;
					newVal = (*m_listMaps[modmapFlag])[key].listitems[j].key;
					break;
				}
			}

			if (valid_string)
				(*m_listMaps[modmapFlag])[key].value = newVal;
			else
			{
				wxLogWarning(_T("recieved list option is not valid"));
				return false;
			}
			break;
		}
		default:
			return false;
	}
	//if we made it here, all is good
	return true;
}

bool mmOptionsWrapper::reloadMapOptions(wxString mapname)
{
	m_boolMaps[MapOption] 	= new optionMapBool;
	m_floatMaps[MapOption]	= new optionMapFloat;
	m_stringMaps[MapOption] = new optionMapString;
	m_listMaps[MapOption]	= new optionMapList;

	return loadMapOptions(mapname);
}
