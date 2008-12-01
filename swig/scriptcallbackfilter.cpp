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

#include "scriptcallbackfilter.hpp"

ScriptCallbackFilter::ScriptCallbackFilter(std::string name, std::string description, 
		callback_t callback, void * user_data, void * other_data) :
	RemoteWorkFilterModel(name, description)
{
	mCallback = callback;
	mUserData = user_data;
	mOtherData = other_data;
}

void * ScriptCallbackFilter::getUserData(){
	return mUserData;
}

void * ScriptCallbackFilter::getOtherData(){
	return mOtherData;
}

void ScriptCallbackFilter::reportWorks(){
	mCallback(mUserData, mOtherData);
	RemoteWorkFilterModel::reportWorks();
}

