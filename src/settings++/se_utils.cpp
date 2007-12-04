#include "se_utils.h"

inline wxString _S (const std::string str)
{
	return wxString(str.c_str(),*wxConvCurrent);
}

template<typename Type> void toString(std::string& s, Type t) {
	//using namespace std;
			std::stringstream ss;
			ss << t;
			ss >> s;
}

template<typename Type> Type fromString(const std::string& s) {
        std::stringstream ss;
        ss << s;
        Type r;
        ss >> r;
        return r;
}
