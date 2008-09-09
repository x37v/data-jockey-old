#include <yamlcpp/parser.hpp>
#include "config.hpp"

using namespace datajockey;

Configuration * Configuration::cInstance = NULL;

Configuration::Configuration(){ 
	mValid = false;
}

Configuration::Configuration(const Configuration&){ }
//Configuration& Configuration::operator=(const Configuration&){ }
Configuration::~Configuration(){ } 

Configuration * Configuration::instance(){
	if(!cInstance)
		cInstance = new Configuration();
	return cInstance;
}

/*
void Configuration::load(std::string yaml_data){
	Configuration * self = Configuration::instance();
	return false;
}
*/

void Configuration::loadFile(std::string path) throw(std::runtime_error) {
	try {
		yaml::Parser p;
		mRoot = p.parseFile(path);
		mValid = true;
	} catch (...){
		mValid = false;
		std::string str("error reading config file: ");
		str.append(path);
		throw std::runtime_error(str);
	}
}

/*
std::string Configuration::tostr(){
	return std::string();
}
*/

bool Configuration::valid(){
	return mValid;
}

bool Configuration::databaseGet(std::string element, std::string &result) throw(std::runtime_error){
	if(!mValid)
		throw std::runtime_error("trying to access data from an invalid configuration");
	try {
		yaml::map_ptr topmap = yaml::get<yaml::map_ptr>(mRoot);
		yaml::map::iterator it = topmap->find(std::string("database"));
		if(it == topmap->end())
			throw std::runtime_error("no \'database\' entry in config file");
		yaml::map_ptr dbmap = yaml::get<yaml::map_ptr>(it->second);
		it = dbmap->find(element);
		if(it == dbmap->end())
			return false;
		result = yaml::get<std::string>(it->second);
		return true;
	} catch(boost::bad_get &e) {
		std::string str("unable to access database information for: ");
		str.append(element);
		throw std::runtime_error(str);
	} catch(std::exception &e){
		throw;
	} catch(...){
		std::string str("unable to access database information for: ");
		str.append(element);
		throw std::runtime_error(str);
	}
}

std::string Configuration::databaseAdapter() throw(std::runtime_error){
	std::string adapter;
	if(!databaseGet("adapter", adapter))
		throw std::runtime_error("could not find database adapter");
	else if (adapter == "mysql")
		return std::string("QMYSQL");
	else
		throw std::runtime_error("invalid database adapter");
}

std::string Configuration::databaseName() throw(std::runtime_error){
	std::string name;
	if(!databaseGet("database", name))
		throw std::runtime_error("could not get database name");
	else
		return name;
}

std::string Configuration::databasePassword() {
	//we don't need to have a password entry
	try {
		std::string pass;
		if(databaseGet("password", pass))
			return pass;
	} catch (...) { /* don't do anything */}
	return std::string("");
}

std::string Configuration::databaseUserName() {
	//we don't need to have a username entry
	try {
		std::string user;
		if(databaseGet("username", user))
			return user;
	} catch (...) { /* don't do anything */}
	return std::string("");
}

