#include <string>

#include "core/number/uuid.hpp"

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid.hpp"          
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp" 

std::string windy::uuid::generate() {

	auto uuid = boost::uuids::random_generator()();
	return boost::lexical_cast<std::string>(uuid);

}