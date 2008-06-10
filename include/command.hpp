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

