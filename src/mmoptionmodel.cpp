#include "mmoptionmodel.h"

mmOptionModel::mmOptionModel(wxString name_, wxString key_, wxString description_, OptionType type_,
                             wxString section_ , wxString style_ )
	: name(name_),key(key_),description(description_),type(type_),
      section(section_),ct_type_string(style_)
{
    //set style according to input string
    if ( style_= _T("yaadda") )
        ct_type = ct_someothers;
    else
        ct_type = ct_undefined; //lobby will chooose best fit

}

mmOptionModel::mmOptionModel(wxString name_, wxString key_, wxString description_, OptionType type_,
                             wxString section_ , ControlType style_ )
	: name(name_),key(key_),description(description_),type(type_),
      section(section_), ct_type(style_)
{
}

mmOptionModel::~mmOptionModel()
{}

mmOptionModel::mmOptionModel()
{
	type = opt_undefined;
	name = _T("");
	key = name;
	description = name;
	section = SLGlobals::nosection_name;
	ct_type = ct_undefined;
}

mmOptionBool::mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_,
                           wxString section_ , wxString style_):
	mmOptionModel(name_,key_,description_,opt_bool,section_,style_),
	def(def_),value(def_)
{}

mmOptionBool::mmOptionBool():mmOptionModel()
{
	value = false;
	def = value;
}

mmOptionFloat::mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_,
                             wxString section_ , wxString style_):
	mmOptionModel(name_,key_,description_,opt_float,section_,style_),
	def(def_),value(def_),stepping(stepping_),min(min_),max(max_)
{}

mmOptionFloat::mmOptionFloat():mmOptionModel()
{
	value = 0.0;
	min = value;
	max = value;
	stepping = value;
}

mmOptionString::mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, unsigned int max_len_,
                               wxString section_ , wxString style_):
	mmOptionModel(name_,key_,description_,opt_string,section_,style_),
	def(def_),value(def_),max_len(max_len_)
{}

mmOptionString::mmOptionString():mmOptionModel()
{
	def = _T("");
	value = def;
	max_len = 0;
}

mmOptionList::mmOptionList(wxString name_, wxString key_, wxString description_, wxString def_,
                           wxString section_ , wxString style_):
	mmOptionModel(name_,key_,description_,opt_list,section_,style_),
	def(def_),value(def_)
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
	//make sure current choice is set to default
	if ( this->def == key_ )
        this->cur_choice_index = listitems.size() - 1;
	cbx_choices.Add(name_);
}

listItem::listItem(wxString key_, wxString name_,wxString desc_):
	key(key_),name(name_),desc(desc_)
{

}

mmOptionInt::mmOptionInt(wxString name_, wxString key_, wxString description_, int def_, int stepping_, int min_, int max_,
                         wxString section_ , wxString style_):
	mmOptionModel(name_,key_,description_,opt_int,section_,style_),
	def(def_),value(def_),stepping(stepping_),min(min_),max(max_)
{}

mmOptionInt::mmOptionInt():mmOptionModel()
{
	value = 0;
	min = value;
	max = value;
	stepping = value;
}

 mmOptionSection::mmOptionSection():mmOptionModel()
{

}

 mmOptionSection::mmOptionSection(wxString name_, wxString key_, wxString description_,wxString section_)
    :mmOptionModel(name_,key_,description_,opt_section,section_,ct_undefined)
{

}

