#ifndef windy_item
#define windy_item

#include <string>
#include <memory>
#include <vector>

namespace windy {
	namespace content {
			class item {
			public:
				item(const std::string& name, 
					 const std::string& uuid);
				virtual std::shared_ptr<class instance> create_instance();
				virtual void remove_instance(std::shared_ptr<class instance> instance);
				std::string name();
				std::string uuid();
				std::vector<std::shared_ptr<class instance> >& instances();

			protected:
				virtual std::shared_ptr<class instance> build() = 0;
				virtual void remove(std::shared_ptr<class instance> instance) = 0;

			protected:
				//@TODO void name(std::string name){ this->_name = name; }
				//@TODO void guid(std::string guid){ this->_guid = guid; }
			private:
				std::string _name;
				std::string _uuid;

			protected:
				std::vector<std::shared_ptr<class instance> > _instances;
			};
	}
}

#endif