/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

//this is all about the configuration!
#ifndef DATAJOCKEY_CONFIGURATION_HPP
#define DATAJOCKEY_CONFIGURATION_HPP

#define DEFAULT_OSC_PORT 10001

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
         //returns the path to the config file has been loaded
         static std::string getFile();

			//get database data
			std::string databaseAdapter() throw(std::runtime_error);
			std::string databaseName() throw(std::runtime_error);
			std::string databaseUserName();
			std::string databasePassword();
			unsigned int oscPort();
		private:
			bool databaseGet(std::string entry, std::string &result) throw(std::runtime_error);
         std::string mFile;

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

#endif
