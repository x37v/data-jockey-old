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

#ifndef DATAJOCKEY_COMMAND_HPP
#define DATAJOCKEY_COMMAND_HPP

#include <string>
#include "object.hpp"

namespace DataJockey {

	typedef int typeID_t;
	typedef std::string groupName_t;

	class Command;

	//this defines the basic interface for commands, they have a group name (string)
	//and within that group, a type id,
	//they also have a status, ready (not acted uppon), completed and rejected
	class Command : public Object {
		public:
			enum status_t { ready, completed, rejected };
			Command();
			virtual ~Command();
			virtual const typeID_t getTypeID(){return -1;}
			virtual const groupName_t getGroupName(){return "group";}
			void setReady();
			void setRejected();
			void setCompleted();
			status_t getStatus();
		private:
			status_t mStatus;
	};
}

#endif

