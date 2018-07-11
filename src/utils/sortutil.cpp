#include <iterator>
#include <ctype.h>
#include <string>

#include "sortutil.h"

// Returns 1 if the second argument is considered greater/newer than the first,
// 0 if they are identical and -1 otherwise.
int CompareVersionStrings(const std::string& lhs, const std::string& rhs)
{
	std::string::const_iterator lhs_it = lhs.begin(), rhs_it = rhs.begin();

	while(true) {
		if (lhs.end() == lhs_it) {
			if (rhs.end() == rhs_it)
				return 0;
			else
				return 1; // rhs has more components than lhs => lhs < rhs
		} else {
			if (rhs.end() == rhs_it)
				return -1; // lhs has more components than rhs => rhs < lhs
			else {
				// If both start with digits then we go into numerical compare
				if (isdigit(*lhs_it) && isdigit(*rhs_it)) {
					std::string::const_iterator lhs_sit = lhs_it, rhs_sit = rhs_it; // start
					// Find end
					while (lhs_it != lhs.end() && isdigit(*lhs_it))
						++lhs_it;
					while (rhs_it != rhs.end() && isdigit(*rhs_it))
						++rhs_it;

					auto lhs_len = std::distance(lhs_sit, lhs_it);
					auto rhs_len = std::distance(rhs_sit, rhs_it);
					if (lhs_len < rhs_len)
						return 1;
					else if (lhs_len > rhs_len)
						return -1;
					else { // Compare digit by digit
						while (lhs_sit != lhs_it) {
							if (*lhs_sit < *rhs_sit)
								return 1;
							else if (*lhs_sit > *rhs_sit)
								return -1;
							++lhs_sit;
							++rhs_sit;
						}
					}
				} else { // use normal alphanumerical compare
					if (*lhs_it < *rhs_it)
						return 1;
					else if (*lhs_it > *rhs_it)
						return -1;
					// else continue onto the next component
					++lhs_it;
					++rhs_it;
				}
			}
		}
	}
}

CompareVersionStringsFunctor compareVersionStringsFunctor;
