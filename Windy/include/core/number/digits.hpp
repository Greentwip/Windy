#ifndef windy_digits
#define windy_digits

#include <string>
#include <cassert>

namespace windy {
	class digits {
	public:
		static bool is_power_of_two(unsigned int x);
		static bool is_number(const std::string& s);
		
		template<class T>
		static constexpr const T& clamp(const T& v, 
										const T& lo, 
										const T& hi){

			return clamp(v, lo, hi, std::less<T>());

		}

		template<class T, class Compare>
		static constexpr const T& clamp(const T& val, 
										const T& lo, 
										const T& hi, 
										Compare comp){

			return assert(!comp(hi, lo)), comp(val, lo) ? lo : comp(hi, val) ? hi : val;

		}
	};
}

#endif