#ifndef MMOPTIONSORAPPER_H_
#define MMOPTIONSORAPPER_H_

#include "mmoptionmodel.h"
#include <vector>
#include <wx/string.h>

typedef std::vector<mmOptionBool> optionVectorBool;
typedef std::vector<mmOptionFloat> optionVectorFloat;
typedef std::vector<mmOptionString> optionVectorString;
typedef std::vector<mmOptionList> optionVectorList;

class mmOptionsWrapper
{
public:
	mmOptionsWrapper();
	virtual ~mmOptionsWrapper();
	bool loadMapOptions(wxString mapname);
	bool loadModOptions();

private:
	optionVectorBool m_mod_options_bool;
	optionVectorBool m_map_options_bool;
	
	optionVectorFloat m_mod_options_float;
	optionVectorFloat m_map_options_float;
		
	optionVectorString m_mod_options_string;
	optionVectorString m_map_options_string;
	
	optionVectorList m_mod_options_list;
	optionVectorList m_map_options_list;
		
};

#endif /*MMOPTIONSORAPPER_H_*/
