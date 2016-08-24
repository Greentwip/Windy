#ifndef windy_environment
#define windy_environment

#include <memory>

namespace windy {
	namespace animation {
		class timeline;
	}

	namespace content {

		class edition;
		class project;
		class properties;

		class environment {

		public:
			environment();

			void trigger_recalculation();

		public:
			std::shared_ptr<class content::edition> edition();
			std::shared_ptr<class content::project> project();
			std::shared_ptr<class content::properties> properties();
			std::shared_ptr<class animation::timeline> timeline();

		private:
			std::shared_ptr<class content::edition>				_edition;
			std::shared_ptr<class content::project>				_project;
			std::shared_ptr<class content::properties>			_properties;
			std::shared_ptr<class animation::timeline>			_timeline;

		};
	}
}

#endif