#ifndef MMOPTIONSORAPPER_H_
#define MMOPTIONSORAPPER_H_

#include "mmoptionmodel.h"
#include <map>
#include <vector>
#include <utility>
#include <wx/string.h>

typedef std::map<wxString,mmOptionBool> optionMapBool;
typedef std::map<wxString,mmOptionFloat> optionMapFloat;
typedef std::map<wxString,mmOptionString> optionMapString;
typedef std::map<wxString,mmOptionList> optionMapList;
typedef std::map<wxString,mmOptionBool> optionMapBool;
typedef std::map<wxString,mmOptionFloat> optionMapFloat;
typedef std::map<wxString,mmOptionString> optionMapString;
typedef std::map<wxString,mmOptionList> optionMapList;

typedef std::map<wxString,mmOptionBool>::iterator optionMapBoolIter;
typedef std::map<wxString,mmOptionFloat>::iterator optionMapFloatIter;
typedef std::map<wxString,mmOptionString>::iterator optionMapStringIter;
typedef std::map<wxString,mmOptionList>::iterator optionMapListIter;
typedef std::map<wxString,mmOptionBool>::iterator optionMapBoolIteIterr;
typedef std::map<wxString,mmOptionFloat>::iterator optionMapFloatIter;
typedef std::map<wxString,mmOptionString>::iterator optionMapStringIter;
typedef std::map<wxString,mmOptionList>::iterator optionMapListIter;

typedef std::pair< wxString,wxString> wxStringPair;
typedef std::vector<wxStringPair> wxStringPairVec;
typedef std::map<wxString,wxString> wxStringMap;

typedef int GameOption;
enum {
	MapOption = 1,
	ModOption = 0
};// should reflect: optionCategoriesCount

class mmOptionsWrapper
{
public:
	mmOptionsWrapper();
	virtual ~mmOptionsWrapper();
	bool loadMapOptions(wxString mapname);
	bool reloadMapOptions(wxString mapname); //recreates the containers and read from unitsync with new mapname
	bool loadOptions(GameOption, wxString mapname = _T(""));
	bool keyExists(wxString key,GameOption,bool showError, OptionType* optType);
	bool setOptions(wxStringPairVec*,GameOption);
	void getOptions(wxStringPairVec*,GameOption);
	void getOptionsMap(wxStringMap*,GameOption);
	void unLoadOptions();
	void unLoadOptions(GameOption);

	wxString getSingleValue(wxString key);
	wxString getSingleValue(wxString key, GameOption modmapFlag);

	bool setSingleOption(wxString,wxString, GameOption modmapFlag);
	bool setSingleOption(wxString,wxString);

	OptionType GetSingleOptionType (wxString key);

//private:
	const static int optionCategoriesCount = 2;
	optionMapBool* m_boolMaps[optionCategoriesCount];
	optionMapFloat* m_floatMaps[optionCategoriesCount];
	optionMapString* m_stringMaps[optionCategoriesCount];
	optionMapList* m_listMaps[optionCategoriesCount];
protected:
	bool setSingleOptionTypeSwitch(wxString key, wxString value, GameOption modmapFlag, OptionType optType);

};

#endif /*MMOPTIONSORAPPER_H_*/
