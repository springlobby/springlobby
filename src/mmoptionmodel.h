#ifndef MMOPTIONMODEL_H_
#define MMOPTIONMODEL_H_

#include <wx/string.h>
#include <wx/arrstr.h>
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
struct listItem
{
	listItem(wxString key_, wxString name_,wxString desc_);
	
		wxString key;
		wxString name;
		wxString desc;
};

typedef std::vector<listItem> ListItemVec;

struct mmOptionModel
{
	mmOptionModel(wxString name_, wxString key_, wxString description_, int type_ = IS_UNDEFINED_OPTION);
	~mmOptionModel();
	mmOptionModel();
	
	wxString name, key, description;
	int type;
};

struct mmOptionBool : public mmOptionModel
{
	mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_);
	mmOptionBool();
	bool def;
	bool value;
};

struct mmOptionFloat : public mmOptionModel
{
	mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_);
	mmOptionFloat();
	
	float def;
	float value;
	
	float stepping;
	float min, max;
};

struct mmOptionString : public mmOptionModel
{
	mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, int max_len_);
	mmOptionString();
	
	wxString def;
	wxString value;
	int max_len;
};

struct mmOptionList : public mmOptionModel
{
	mmOptionList(wxString name_, wxString key_, wxString description_, wxString def_);
	mmOptionList();
	
	void addItem(wxString key_, wxString name_, wxString desc_);
	
	wxString def;
	wxString value;
	ListItemVec listitems;
	wxArrayString cbx_choices;
	
};

#endif /*MMOPTIONMODEL_H_*/
