#include "mmoptionmodel.h"

mmOptionModel::mmOptionModel(wxString name_, wxString key_, wxString description_, OptionType type_):
	name(name_),key(key_),description(description_),type(type_)
{}

mmOptionModel::~mmOptionModel()
{}

mmOptionModel::mmOptionModel()
{
	type = opt_undefined;
	name = _T("");
	key = name;
	description = name;
}

mmOptionBool::mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_):
	mmOptionModel(name_,key_,description_,opt_bool),def(def_),value(def_)
{}

mmOptionBool::mmOptionBool():mmOptionModel()
{
	value = false;
	def = value;
}

mmOptionFloat::mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_):
	mmOptionModel(name_,key_,description_,opt_float),def(def_),value(def_),stepping(stepping_),min(min_),max(max_)
{}

mmOptionFloat::mmOptionFloat():mmOptionModel()
{
	value = 0.0;
	min = value;
	max = value;
	stepping = value;
}

mmOptionString::mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, int max_len_):
	mmOptionModel(name_,key_,description_,opt_string),def(def_),value(def_),max_len(max_len_)
{}

mmOptionString::mmOptionString():mmOptionModel()
{
	def = _T("");
	value = def;
	max_len = 0;
}

mmOptionList::mmOptionList(wxString name_, wxString key_, wxString description_, wxString def_):
	mmOptionModel(name_,key_,description_,opt_list),def(def_),value(def_)
{
	cur_choice_index = 0;
}

mmOptionList::mmOptionList():mmOptionModel()
{
	value = _T("");
	def = _T("");
}

void mmOptionList::addItem(wxString key_, wxString name_, wxString desc_)
{
	listitems.push_back(listItem(key_,name_,desc_));
	cbx_choices.Add(name_);
}

listItem::listItem(wxString key_, wxString name_,wxString desc_):
	key(key_),name(name_),desc(desc_)
{

}
