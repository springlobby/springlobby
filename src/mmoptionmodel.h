#ifndef MMOPTIONMODEL_H_
#define MMOPTIONMODEL_H_

#include <wx/string.h>
#include <vector>

#define IS_UNDEFINED_OPTION 0
#define IS_FLOAT_OPTION 	3
#define IS_LIST_OPTION		2
#define IS_BOOL_OPTION		1
#define IS_STRING_OPTION	4

/* for reference, from unitsync 5111
enum OptionType {
	opt_error  = 0,
	opt_bool   = 1,
	opt_list   = 2,
	opt_number = 3,
	opt_string = 4
};*/

class mmOptionModel
{
public:
	mmOptionModel(wxString name_, wxString key_, wxString description_, int type_ = IS_UNDEFINED_OPTION);
	virtual ~mmOptionModel();
	
	wxString name, key, description;
	int type;
};

class mmOptionBool : public mmOptionModel
{
public:
	mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_);
	
	bool def;
	bool value;
};

class mmOptionFloat : public mmOptionModel
{
public:
	mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_);
	
	float def;
	float value;
	
	float stepping;
	float min, max;
};

class mmOptionString : public mmOptionModel
{
public:
	mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, int max_len_);
	
	wxString def;
	wxString value;
	int max_len;
};

class mmOptionList : public mmOptionModel
{
public:
	mmOptionList(wxString name_, wxString key_, wxString description_, bool def);
	
	wxString def;
	wxString value;
};

#endif /*MMOPTIONMODEL_H_*/
