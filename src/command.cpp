#include "command.hpp"
#include <iostream>

using namespace DataJockey;


Command::Command() : Object(){
	mStatus = ready;
//	std::cout << "creating command " << getId() << std::endl;
}

Command::~Command(){
//	std::cout << "destroying command " << getId() << std::endl;
}

void Command::setReady(){
	mStatus = ready;
}

void Command::setRejected(){
	mStatus = rejected;
}

void Command::setCompleted(){
	mStatus = completed;
}

Command::status_t Command::getStatus(){
	return mStatus;
};
