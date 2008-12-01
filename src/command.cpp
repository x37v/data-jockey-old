/*
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
