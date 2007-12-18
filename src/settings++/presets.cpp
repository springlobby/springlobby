#include "presets.h"

template <typename T, int valueCount> presetValues<T,valueCount>::presetValues ( wxString key_arg,const wxString* levelLabels,const T* values_arg)
	{
		for(int i = 0; i< valueCount;++i)
		{
			values[levelLabels[i]]=values_arg[i];
		}
		key = key_arg;
	}
