#include "se_utils.h"

#include <string>
#include <sstream>

void toString(std::string& s, int t) {
			std::stringstream ss;
			ss << t;
			ss >> s;
}

int fromString(const std::string& s) {
        std::stringstream ss;
        ss << s;
        int r;
        ss >> r;
        return r;
}

