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

#ifndef SCRIPT_CALLBACK_FILTER_HPP
#define SCRIPT_CALLBACK_FILTER_HPP

#include "remoteworkfiltermodel.hpp"
typedef void (*callback_t) (void *user_data, void* other_data);

class ScriptCallbackFilter : public RemoteWorkFilterModel {
	private:
		callback_t mCallback;
		void * mUserData;
		void * mOtherData;
	public:
		ScriptCallbackFilter(std::string name, std::string description, callback_t callback, void * user_data = NULL, void * other_data = NULL);
		void * getUserData();
		void * getOtherData();
	public slots:
		virtual void reportWorks();
};

#endif

