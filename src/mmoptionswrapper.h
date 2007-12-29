#ifndef MMOPTIONSORAPPER_H_
#define MMOPTIONSORAPPER_H_

#include "mmoptionmodel.h"
#include <map>
#include <vector>
#include <utility>
#include <wx/string.h>

#define MODOPTS 0
#define MAPOPTS 1

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
	bool keyExists(wxString key,GameOption,bool showError, int* optType);
	bool setOptions(wxStringPairVec*,GameOption);
	void getOptions(wxStringPairVec*,GameOption);
	
	wxString getSingleValue(wxString key);//TODO implement!
	
	bool setSingleOption(wxString,wxString, GameOption modmapFlag);
	bool setSingleOption(wxString,wxString);

//private:
	const static int optionCategoriesCount = 2;
	optionMapBool* m_boolMaps[optionCategoriesCount];
	optionMapFloat* m_floatMaps[optionCategoriesCount];
	optionMapString* m_stringMaps[optionCategoriesCount];
	optionMapList* m_listMaps[optionCategoriesCount];

};

#endif /*MMOPTIONSORAPPER_H_*/
