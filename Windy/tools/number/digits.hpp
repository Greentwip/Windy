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

		template<class T>
		static constexpr const T& clamp(const T& v, const T& lo, const T& hi)
		{
			return clamp(v, lo, hi, std::less<T>());
		}

		template<class T, class Compare>
		static constexpr const T& clamp(const T& val, const T& lo, const T& hi, Compare comp)
		{
			return assert(!comp(hi, lo)),
				comp(val, lo) ? lo : comp(hi, val) ? hi : val;
		}
	};

}


#endif