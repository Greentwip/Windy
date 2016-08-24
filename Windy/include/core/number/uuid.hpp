#ifndef windy_uuid
#define windy_uuid

#include <string>

namespace windy {

	class uuid {
	public:
		static std::string generate();
	};
}

#endif