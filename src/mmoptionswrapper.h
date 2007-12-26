#ifndef MMOPTIONSORAPPER_H_
#define MMOPTIONSORAPPER_H_

#include "mmoptionmodel.h"
#include <vector>
#include <wx/string>

typedef std::vector<mmOptionModel> optionVector;

class mmOptionsWrapper
{
public:
	mmOptionsWrapper();
	virtual ~mmOptionsWrapper();
	bool loadMapOptions(wxString mapname);
	bool loadModOptions();

private:
	optionVector m_mod_options;
	optionVector m_map_options;
};

#endif /*MMOPTIONSORAPPER_H_*/
