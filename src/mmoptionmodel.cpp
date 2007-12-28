#include "mmoptionmodel.h"

mmOptionModel::mmOptionModel(wxString name_, wxString key_, wxString description_, int type_):
	name(name_),key(key_),description(description_),type(type_)
{}

mmOptionModel::~mmOptionModel()
{}

mmOptionModel::mmOptionModel()
{
	type = IS_UNDEFINED_OPTION;
	name = _T("");
	key = name;
	description = name;
}

mmOptionBool::mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_):
	mmOptionModel(name_,key_,description_,IS_BOOL_OPTION),def(def_),value(def_)
{}

mmOptionBool::mmOptionBool():mmOptionModel()
{}

mmOptionFloat::mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_):
	mmOptionModel(name_,key_,description_,IS_FLOAT_OPTION),def(def_),value(def_),stepping(stepping_),min(min_),max(max_)
{}

mmOptionFloat::mmOptionFloat():mmOptionModel()
{}

mmOptionString::mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, int max_len_):
	mmOptionModel(name_,key_,description_,IS_STRING_OPTION),def(def_),value(def_),max_len(max_len_)
{}

mmOptionString::mmOptionString():mmOptionModel()
{}

mmOptionList::mmOptionList(wxString name_, wxString key_, wxString description_, wxString def_):
	mmOptionModel(name_,key_,description_,IS_LIST_OPTION),def(def_),value(def_)
{}

mmOptionList::mmOptionList():mmOptionModel()
{}

void mmOptionList::addItem(wxString key_, wxString name_, wxString desc_)
{
	listitems.push_back(listItem(key_,name_,desc_));
	cbx_choices.Add(name_);
}

listItem::listItem(wxString key_, wxString name_,wxString desc_):
	key(key_),name(name_),desc(desc_)
{

}
