
#include <cassert>
#include <string>
#include <algorithm>

#include "core/number/digits.hpp"

bool windy::digits::is_power_of_two(unsigned int x){

	return (x != 0) && ((x & (x - 1)) == 0);

}

bool windy::digits::is_number(const std::string& s){

	auto it = std::find_if(s.begin(), s.end(), [](char c) { return !isdigit(c); });
	return !s.empty() && it == s.end();

}	