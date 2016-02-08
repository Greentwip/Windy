#ifndef windy_numbers
#define windy_numbers

#include <ctype.h>

#include <string>
#include <algorithm>

namespace windy {
	class digits {
	public:
		static bool is_power_of_two(unsigned int x)
		{
			return (x != 0) && ((x & (x - 1)) == 0);
		}

		static bool is_number(const std::string& s)
		{
			return !s.empty() && std::find_if(s.begin(),
				s.end(), [](char c) { return !isdigit(c); }) == s.end();
		}
	};

}


#endif