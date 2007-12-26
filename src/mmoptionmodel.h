#ifndef MMOPTIONMODEL_H_
#define MMOPTIONMODEL_H_

#include <wx/string.h>
#include <vector>

#define IS_UNDEFINED_OPTION 0
#define IS_FLOAT_OPTION 	1
#define IS_LIST_OPTION		2
#define IS_BOOL_OPTION		3
#define IS_STRING_OPTION	4

struct mmOptionModel
{
	mmOptionModel(wxString name_, wxString key_, wxString description_, int type_ = IS_UNDEFINED_OPTION);
	virtual ~mmOptionModel();
	
	wxString name, key, description;
	int type;
};

struct mmOptionBool : public mmOptionModel
{
	mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_);
	
	bool def;
	bool value;
};

class mmOptionFloat : public mmOptionModel
{
	mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_);
	
	float def;
	float value;
	
	float stepping;
	float min, max;
};

class mmOptionString : public mmOptionModel
{
	mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, int max_len_);
	
	wxString def;
	wxString value;
	int max_len;
};

class mmOptionList : public mmOptionList
{
	mmOptionList(wxString name_, wxString key_, wxString description_, bool def);
	
	wxString def;
	wxString value;
};

#endif /*MMOPTIONMODEL_H_*/
