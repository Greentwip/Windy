#ifndef windy_uuid
#define windy_uuid

#include <string>
#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid.hpp"          
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp" 

namespace windy {

	class uuid {
	public:
		static std::string generate() {

			auto uuid = boost::uuids::random_generator()();
			return boost::lexical_cast<std::string>(uuid);

		}
	};

}
#endif