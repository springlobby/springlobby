#include "presets.h"

template <typename T> presetValues<T>::presetValues ( wxString key_arg,const wxString* levelLabels,const T* values_arg)
	{
		for(int i = 0; i< sizeof(levelLabels)+1;++i)
		{
			values[levelLabels[i]]=values_arg[i];
		}
		key = key_arg;
	}
