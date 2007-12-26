#include "mmoptionmodel.h"

mmOptionModel::mmOptionModel(wxString name_, wxString key_, wxString description_, int type_):
	name(name_),key(key_),type(type_)
{
}

mmOptionModel::~mmOptionModel()
{
}

mmOptionBool::mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_):
	def(def_),value(def_),mmOptionModel(name,key,description_,IS_BOOL_OPTION)
	{
	
	}


mmOptionFloat::mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_):
	def(def_),value(def_),stepping(stepping_),min(min_),max(max_),mmOptionModel(name,key,description_,IS_FLOAT_OPTION)
	{
	
	}

mmOptionString::mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, int max_len_):
	def(def_),value(def_),max_len(max_len_),mmOptionModel(name,key,description_,IS_STRING_OPTION)
	{
	
	}

