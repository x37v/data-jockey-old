//this is all about the configuration!

#include <yamlcpp/yaml.hpp>
#include <stdexcept>

namespace datajockey {
	class Configuration {
		public:
			static Configuration * instance();
			//this finds a config file in a default location
			void loadDefault() throw(std::runtime_error);
			//void load(std::string yaml_data);
			void loadFile(std::string path) throw(std::runtime_error);
			//std::string tostr();
			bool valid();

			//get database data
			std::string databaseAdapter() throw(std::runtime_error);
			std::string databaseName() throw(std::runtime_error);
			std::string databaseUserName();
			std::string databasePassword();
		private:
			bool databaseGet(std::string entry, std::string &result) throw(std::runtime_error);

		protected:
			Configuration();
			Configuration(const Configuration&);
			Configuration& operator=(const Configuration&);
			virtual ~Configuration();
		private:
			static Configuration * cInstance;
			yaml::node mRoot;
			bool mValid;
	};
}
